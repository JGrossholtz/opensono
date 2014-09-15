#include <sys/types.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "sample_ring_buffer.h"

#include "network_client_server.h"
#include "network_client.h"

// variables related with multicast network
int sock_descr;
struct sockaddr_in clientsSock;
socklen_t clients_sock_size;

//variables related with receiver thread
pthread_t multicast_reception_thread;
pthread_attr_t attr;

int init_multicast_client(){
	int retval;
	/*
	 * First init the multicast networking to reiceive samples from the server
	 */
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


	/*
	 * Start receiver thread
	 */
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED); //The reception thread will handle it's stop by itself

	retval =  pthread_create(&multicast_reception_thread, &attr, multicast_data_reception_thread, NULL); 

}


void *multicast_data_reception_thread(void * param){
	sample buf[PACKET_SIZE];
	ssize_t count;

	
	ring_buffer_T* buffer =  init_sample_ring_buffer(4096);

	for(;;){
		bzero(buf,PACKET_SIZE);
		if(count = recvfrom(sock_descr,buf,PACKET_SIZE,0,(struct sockaddr*) &clientsSock,&clients_sock_size) < 0){
			perror("Reading datagram message error");
			close(sock_descr);
			exit(1);
		}
		
		sample_ring_buffer_write(buffer, &buf,  NBR_SAMPLES_IN_PACKET);
		//TODO : add data to ring buffer here
	
	}

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

