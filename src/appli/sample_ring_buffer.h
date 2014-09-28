#ifndef __SAMPLE_RING_BUFFER
#define __SAMPLE_RING_BUFFER
#include <stdint.h>
#include "network_client_server.h"

typedef struct{
	sample *buffer;
	uint32_t size;
	sample *buffer_end;
	sample *write_ptr;
	sample *read_ptr;
} ring_buffer_T;


ring_buffer_T* init_sample_ring_buffer(uint32_t size);
uint32_t sample_ring_buffer_write(ring_buffer_T *rb, sample* data, uint32_t count);
uint32_t sample_ring_buffer_read(ring_buffer_T *rb, sample* data, uint32_t requested);
#endif
