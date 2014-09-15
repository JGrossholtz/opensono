#include <stdio.h>
#include <stdlib.h>
#include "sample_ring_buffer.h"


ring_buffer_T* init_sample_ring_buffer(uint32_t size){
	ring_buffer_T *rb = malloc(sizeof(ring_buffer_T));
	if(rb == NULL){
		fprintf (stderr, "cannot allocate memory file:%s l%d\n",__FILE__,__LINE__); 
	}

	rb->size = size;
	rb->buffer = malloc(sizeof(sample) * size);
	rb->read_ptr  = rb->buffer;
	rb->write_ptr = rb->buffer;
	

	return rb;
}

uint32_t sample_ring_buffer_write(ring_buffer_T *rb, sample* data, uint32_t count){// TODO : very basic for now : should be optimized, check if the read pointer is not in write range and add a mutex.
	printf("writing %d sample starting at %p\n buffer [%p-----%p]\n",count,rb->write_ptr, rb->buffer,rb->buffer+rb->size);

	if( rb->write_ptr + count <= rb->buffer + rb->size ){
		memcpy(rb->write_ptr,data,count*sizeof(sample));
		rb->write_ptr += count; 
	}
	else{
		printf("\n\n Jumping back to start !!\n\n");
		uint32_t writelength = 	rb->buffer + rb->size - rb->write_ptr;

		memcpy(rb->write_ptr,data,writelength*sizeof(sample));
		memcpy(rb->buffer,data + writelength,count - writelength);
		rb->write_ptr = rb->buffer + count - writelength; 
	}

	return count;
}


void terminate_sample_ring_buffer(ring_buffer_T *rb){
	free(rb->buffer);
	free(rb);
	return;
}
