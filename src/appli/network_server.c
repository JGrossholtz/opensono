#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "network_client_server.h"
#include "util.h"
#include "common.h"

struct sockaddr_in clientsSock;
int socket_descriptor;



int init_multicast_server(){
	//We want to send data to multiple clients, so we use multicast and UDP
	struct in_addr ServerInterface;
	char srvIPstr[IPADDR_STR_LEN];

	socket_descriptor =  socket(AF_INET,SOCK_DGRAM,0); // IPV4, UDP and protocol
	if(socket_descriptor < 0){
		printf("ERROR in socket creation\n");
		return -1;
	}

	//prepare the group socket
	memset(&clientsSock,0, sizeof(clientsSock));
	clientsSock.sin_family = AF_INET;
	clientsSock.sin_addr.s_addr = inet_addr(MULTICAST_GROUP);
	clientsSock.sin_port = htons(OPENSONO_DATA_PORT);


	//Set server's own IP
	printf("Server's IP = %s\n",srvIPstr);
	ServerInterface.s_addr = inet_addr("192.168.1.20");


	//Tell that we want to send data in multicast
	if(setsockopt(socket_descriptor, IPPROTO_IP, IP_MULTICAST_IF, (char *)&ServerInterface, sizeof(ServerInterface)) < 0){
		printf("Cannot set options to socket\n");
		return -3;
	}
	
	printf("server network initialized...\n");
	return 1;
}


int multicast_server_send(char * buf, size_t length){
	ssize_t sended = 0;

	while(sended  < length){
		if((sended = sendto(socket_descriptor, buf, length - sended, 0, (struct sockaddr*)&clientsSock, sizeof(clientsSock))) < 0){
			printf("error while sending data !\n");
			return -1;
		}
		printf("data sent = %zu/%zu\n",sended,length);
		buf = buf + sended;
	}

}


