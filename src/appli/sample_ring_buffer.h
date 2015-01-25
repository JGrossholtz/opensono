#ifndef __SAMPLE_RING_BUFFER
#define __SAMPLE_RING_BUFFER
#include <stdint.h>
#include "network_client_server.h"


/*
 * This structure defines a sample ring buffer
 */
typedef struct{
	sample *buffer;				// Pointer to the sample buffer itself
	uint32_t size;				// Size of the buffer
	sample *buffer_end;			// Pointer to the end of the samples buffer, for comparisons
	sample *write_ptr;			// Writer pointer
	sample *read_ptr;			// Read pointer
} ring_buffer_T;


ring_buffer_T* init_sample_ring_buffer(uint32_t size);
uint32_t sample_ring_buffer_write(ring_buffer_T *rb, sample* data, uint32_t count);
uint32_t sample_ring_buffer_read(ring_buffer_T *rb, sample* data, uint32_t requested);
#endif
