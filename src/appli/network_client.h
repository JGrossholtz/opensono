#ifndef NETWORK_CLIENT____H_
#define NETWORK_CLIENT____H_

#include <stdint.h>

int init_multicast_client();
size_t multicast_client_receive(uint32_t * buf, size_t length);

#endif
