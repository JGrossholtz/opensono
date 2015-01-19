#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sample_ring_buffer.h"



/*
 * Allocate some memory and set default value to create a ring buffer for alsa samples.
 *
 * */
ring_buffer_T* init_sample_ring_buffer(uint32_t size){
	ring_buffer_T *rb = malloc(sizeof(ring_buffer_T));
	if(rb == NULL){
		fprintf (stderr, "cannot allocate memory file:%s l%d\n",__FILE__,__LINE__);
		return NULL;
	}

	rb->size = size;
	rb->buffer = malloc(sizeof(sample) * size);
	rb->buffer_end = rb->buffer +  size ;
	rb->read_ptr  = rb->buffer;
	rb->write_ptr = rb->buffer;
	
	return rb;
}


/*
 * Write data to the ring buffer
 */
uint32_t sample_ring_buffer_write(ring_buffer_T *rb, sample* data, uint32_t count){// TODO : very basic for now : should be optimized, check if the read pointer is not in write range and add a mutex.
	//printf("writing %d sample starting at %p\n buffer [%p-----%p]\n",count,rb->write_ptr, rb->buffer,rb->buffer+rb->size);

	if( rb->write_ptr + count <= rb->buffer_end ){     		// The data we want to read is between the read pointer and the end of the fifo (no need to jump back to start).
		memcpy(rb->write_ptr,data,count*sizeof(sample));
		rb->write_ptr += count; 
	}
	else{													//We need to write our data in 2 parts : from the write pointer to the end adress of the fifo. And from the start adress.
		uint32_t writelength = 	rb->buffer + rb->size - rb->write_ptr;
		memcpy(rb->write_ptr,data,writelength*sizeof(sample));
		memcpy(rb->buffer,data + writelength,count - writelength);
		rb->write_ptr = rb->buffer + count - writelength; 
	}

	return count;
}

/*
 * Read data from the ring buffer
 */
uint32_t sample_ring_buffer_read(ring_buffer_T *rb, sample* data, uint32_t requested){//extremely basic : really risk to overlap writer pointer
//	printf("reading %d sample starting at %p\n buffer [%p-----%p]\n",requested,rb->read_ptr, rb->buffer,rb->buffer+rb->size);

	uint32_t max_samples;

	/*
	 * First part we calculate how many samples we can read. max_samples is the maximum number
	 * of samples available.
	 */
	if(rb->read_ptr == rb->write_ptr){
		//In this case we don't have data at all we must wait until we receive some. No need to consume too much cpu time by re-entering again here
		usleep(20000); //20 mseconds
		return 0;
	}else if(rb->read_ptr < rb->write_ptr){ //test if the read ptr is behind the write ptr in the ring buffer : |   R---------------W         |
		max_samples = rb->write_ptr - rb->read_ptr;
	}else{ 									//Last possible case :              |-----W       R--------------|	->  data is splited in 2 parts : from the curent read pointer to the "end" of the fifo and from the start to read size.
		max_samples = rb->size - (uint32_t)(rb->read_ptr - rb->write_ptr) ;
	}
	
	//printf("samples left=%d\r",max_samples);
	//We don't have enough samples available
	if(max_samples < requested)
		requested = max_samples;


	//printf("reading %d samples, samples left : \n",requested,max_samples-requested);
	if( rb->read_ptr + requested <= rb->buffer + rb->size ){ //We can read data without reaching the end adress of the fifo
		memcpy(data,rb->read_ptr,requested*sizeof(sample));
		rb->read_ptr += requested; 
	}
	else{													//We have to jump back to start, so we need to read in 2 parts.
		uint32_t readlength = 	rb->buffer + rb->size - rb->read_ptr;

		memcpy(data,rb->read_ptr,readlength*sizeof(sample));
		memcpy(data + readlength,rb->buffer,requested - readlength);
		rb->read_ptr = rb->buffer + requested - readlength; 
	}

	return requested;
}

uint32_t  sample_ring_buffer_get_samples_left(ring_buffer_T *rb){
	uint32_t max_samples;

	/*
	 * First part we calculate how many samples we can read. max_samples is the maximum number
	 * of samples available.
	 */
	if(rb->read_ptr == rb->write_ptr){
		//In this case we don't have data at all we must wait until we receive some. No need to consume too much cpu time by re-entering again here
		usleep(20000); //20 mseconds
		return 0;
	}else if(rb->read_ptr < rb->write_ptr){ //test if the read ptr is behind the write ptr in the ring buffer : |   R---------------W         |
		max_samples = rb->write_ptr - rb->read_ptr;
	}else{ 									//Last possible case :              |-----W       R--------------|	->  data is splited in 2 parts : from the curent read pointer to the "end" of the fifo and from the start to read size.
		max_samples = rb->size - (uint32_t)(rb->read_ptr - rb->write_ptr) ;
	}
	return max_samples;
}

/*
 * Free everything we have allocated.
 */
void terminate_sample_ring_buffer(ring_buffer_T *rb){
	free(rb->buffer);
	free(rb);
	return;
}
