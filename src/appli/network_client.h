#ifndef NETWORK_CLIENT____H_
#define NETWORK_CLIENT____H_

#include <stdint.h>
#include "network_client_server.h"



int init_multicast_client(ring_buffer_T *buffer);
size_t multicast_client_receive(sample * buf);
void *multicast_data_reception_thread(void *param);

int init_tcp_client(ring_buffer_T *buffer);


#endif
