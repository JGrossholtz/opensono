#ifndef NETWORK_CLIENT____H_
#define NETWORK_CLIENT____H_

#include <stdint.h>
#include "network_client_server.h"

int init_multicast_client();
size_t multicast_client_receive(sample * buf);

#endif
