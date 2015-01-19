#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "network_client_server.h"
#include "common.h"

struct sockaddr_in adresse;
static int socket_descriptor;
static long int count = 0;
int socket_to_client;

int init_multicast_server(){
	socket_descriptor = socket(AF_INET, SOCK_DGRAM, 0);
	if (socket_descriptor < 0) {
		perror("socket");
		exit(1);
	}
	bzero((char *)&adresse, sizeof(adresse));
	adresse.sin_family = AF_INET;
	adresse.sin_addr.s_addr = htonl(INADDR_ANY);
	adresse.sin_port = htons(OPENSONO_DATA_PORT);

 	adresse.sin_addr.s_addr = inet_addr(MULTICAST_GROUP);

	printf("server network initialized...\n");
	return 1;
}


int multicast_server_send(sample * buf){
	ssize_t sended;
	sended = sendto(socket_descriptor, buf, PACKET_SIZE,  0, (struct sockaddr*)&adresse, sizeof(adresse));
	if(sended < 0){
		printf("We where not able to send data (TODO try to fix network in this case) \n");
	}
	count++;
	printf("packet count = %ld\r",count);

	return sended;
}




int init_tcp_socket(){
	socket_descriptor = socket(AF_INET,SOCK_STREAM,0);
	if(socket_descriptor < 0){
		perror("cannot create socket");
		return -1;
	}

	memset(&adresse,0, sizeof(struct sockaddr_in));
	adresse.sin_family = AF_INET;
	adresse.sin_addr.s_addr = htonl(INADDR_ANY);
	adresse.sin_port = htons(OPENSONO_DATA_PORT);

	if( bind( socket_descriptor, (struct sockaddr *) &adresse, sizeof(struct sockaddr_in )) < 0 ){
		close(socket_descriptor);
		perror("bind");
		exit(1);
	}

	printf("Our IP is : %s\n", inet_ntoa(adresse.sin_addr));
	listen(socket_descriptor,5);

	socket_to_client = accept(socket_descriptor,NULL,NULL);

	return 1;

}


int tcp_server_send(sample * buf){
	ssize_t ret;
	ret = write(socket_to_client,buf,PACKET_SIZE);
	if(ret!=PACKET_SIZE)
		printf("WE did not write a full packet \n"); //TODO handle this case smoothy
}
