#include <sys/types.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>


#include "network_client_server.h"


int sock_descr;
struct sockaddr_in clientsSock;
socklen_t clients_sock_size;

int init_multicast_client(){

	struct ip_mreq group;

	sock_descr = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock_descr < 0) {
		perror("socket");
		exit(1);
	}
	bzero((char *)&clientsSock, sizeof(clientsSock));
	clientsSock.sin_family = AF_INET;
	clientsSock.sin_addr.s_addr = htonl(INADDR_ANY);
	clientsSock.sin_port = htons(OPENSONO_DATA_PORT);
	clients_sock_size = sizeof(clientsSock);

	if(bind(sock_descr,(struct sockaddr*)  &clientsSock,clients_sock_size) < 0){
		printf("error while binding...\n");
		exit(1);
	}


	group.imr_multiaddr.s_addr = inet_addr(MULTICAST_GROUP);         
	group.imr_interface.s_addr = htonl(INADDR_ANY);         
	if (setsockopt(sock_descr, IPPROTO_IP, IP_ADD_MEMBERSHIP,
				&group, sizeof(group)) < 0) {
		perror("setsockopt mreq");
		exit(1);
	}         

	printf("client network ready\n");
}


size_t multicast_client_receive(sample * buf){
	ssize_t count;
	if(count = recvfrom(sock_descr,buf,PACKET_SIZE,0,(struct sockaddr*) &clientsSock,&clients_sock_size) < 0){
		perror("Reading datagram message error");
		close(sock_descr);
		exit(1);
	}
	
	return count;
}

