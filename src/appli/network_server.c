#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "network_client_server.h"
#include "common.h"

struct sockaddr_in clientsSock;
int socket_descriptor;



int init_multicast_server(){

	socket_descriptor = socket(AF_INET, SOCK_DGRAM, 0);
	if (socket_descriptor < 0) {
		perror("socket");
		exit(1);
	}
	bzero((char *)&clientsSock, sizeof(clientsSock));
	clientsSock.sin_family = AF_INET;
	clientsSock.sin_addr.s_addr = htonl(INADDR_ANY);
	clientsSock.sin_port = htons(OPENSONO_DATA_PORT);

 	clientsSock.sin_addr.s_addr = inet_addr(MULTICAST_GROUP);

	printf("server network initialized...\n");
	return 1;
}

static long int count = 0;

int multicast_server_send(sample * buf){
	ssize_t sended;
	sended = sendto(socket_descriptor, buf, PACKET_SIZE,  0, (struct sockaddr*)&clientsSock, sizeof(clientsSock));
	if(sended < 0){
		printf("We where not able to send data (TODO try to fix network in this case) \n");
	}
	count++;
	printf("packet count = %ld\r",count);

	return sended;
}

