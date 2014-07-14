#ifndef NETWORK__SERVER_H__
#define NETWORK__SERVER_H__

int init_multicast_server();
int multicast_server_send(char * buf, size_t length);

#endif 
