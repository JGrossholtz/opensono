#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sample_ring_buffer.h"


/*
 *
 * Allocate the memory needed to contain the giver number of samples.
 * Initialize all pointers and fields for this ring buffer
 *
 */
ring_buffer_T* init_sample_ring_buffer(uint32_t size){
	ring_buffer_T *rb = malloc(sizeof(ring_buffer_T));
	if(rb == NULL){
		fprintf (stderr, "cannot allocate memory file:%s l%d\n",__FILE__,__LINE__); 
	}

	rb->size = size;
	rb->buffer = malloc(sizeof(sample) * size);
	rb->buffer_end = rb->buffer +  size ;
	rb->read_ptr  = rb->buffer;
	rb->write_ptr = rb->buffer;
	

	return rb;
}


/*
 * Write data to a ring buffer
 */
uint32_t sample_ring_buffer_write(ring_buffer_T *rb, sample* data, uint32_t count){// TODO : very basic for now : should be optimized, check if the read pointer is not in write range and add a mutex.
	//printf("writing %d sample starting at %p\n buffer [%p-----%p]\n",count,rb->write_ptr, rb->buffer,rb->buffer+rb->size);

	/*
	 *  The data we want to write will be between the write pointer and the end of the fifo
	 *  (no need to jump back to start).
	 */
	if( rb->write_ptr + count <= rb->buffer_end ){
		memcpy(rb->write_ptr,data,count*sizeof(sample));
		rb->write_ptr += count; 
	}
	else{  //We need to write our data in 2 parts : from the write pointer to the end adress of the fifo. And from the start adress.
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
uint32_t sample_ring_buffer_read(ring_buffer_T *rb, sample* data, uint32_t requested){
	//first we avoid overrruns :
	uint32_t max_samples;
	if(rb->read_ptr == rb->write_ptr){
		usleep(20000); //20 mseconds
		return 0;
	}else if(rb->read_ptr < rb->write_ptr){ //test if the read ptr is behind the write ptr in the ring buffer : |   R---------------W         |
		max_samples = rb->write_ptr - rb->read_ptr;
	}else{ //Last possible case :              |-----W       R--------------|
		max_samples = rb->size - (uint32_t)(rb->read_ptr - rb->write_ptr) ;
	}

	 //We have calculated the maximum amount of samples we can deliver, if it ecceed this value we will return less samples than expected.
	if(max_samples < requested)
		requested = max_samples;


	//printf("reading %d samples, samples left : \n",requested,max_samples-requested);
	if( rb->read_ptr + requested <= rb->buffer + rb->size ){
		memcpy(data,rb->read_ptr,requested*sizeof(sample));
		rb->read_ptr += requested; 
	}
	else{
		uint32_t readlength = 	rb->buffer + rb->size - rb->read_ptr;

		memcpy(data,rb->read_ptr,readlength*sizeof(sample));
		memcpy(data + readlength,rb->buffer,requested - readlength);
		rb->read_ptr = rb->buffer + requested - readlength; 
	}

	return requested;
}


/*
 * Free memory when application is closed.
 */
void terminate_sample_ring_buffer(ring_buffer_T *rb){
	free(rb->buffer);
	free(rb);
	return;
}
