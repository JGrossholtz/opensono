#include <sys/types.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>

#include "sample_ring_buffer.h"

#include "network_client_server.h"
#include "network_client.h"

//for debug purpose :
static long int count = 0;

// variables related with multicast network
static int sock_descr;
static struct sockaddr_in clientsSock;
static socklen_t clients_sock_size;

//variables related with receiver thread
pthread_t multicast_reception_thread;
pthread_attr_t attr;
static ring_buffer_T *ring_buffer = NULL;

int init_multicast_client(ring_buffer_T *buffer)
{
	int retval;
	struct ip_mreq group;

	//Get access to the buffer
	if(buffer == NULL){
		return -1;
	}
	ring_buffer = buffer;
	
	/*
	* First init the multicast networking to reiceive samples from the server
	*/
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

	if (setsockopt(sock_descr, SOL_SOCKET, SO_REUSEADDR, &group, sizeof(group)) < 0) {
		perror("setsockopt mreq");
		exit(1);
	}         

	if(bind(sock_descr,(struct sockaddr*)  &clientsSock,clients_sock_size) < 0){
		printf("error while binding : %d (%s)\n",errno,strerror(errno));
		exit(1);
	}

	//Here we set the multicats group if the clients, all clients within this group will receive the OpenSono packets 
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
	pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED); //The reception thread will handle it's stop by itself (TODO)

	retval =  pthread_create(&multicast_reception_thread, &attr, multicast_data_reception_thread, NULL); 

}


//Reception main thread
void *multicast_data_reception_thread(void * param){
	sample buf[PACKET_SIZE];
	ssize_t count;

	for(;;){
		bzero(buf,PACKET_SIZE);
		if(count = recvfrom(sock_descr,buf,PACKET_SIZE,0,(struct sockaddr*) &clientsSock,&clients_sock_size) < 0){
			perror("Reading datagram message error"); //TODO get remaining data instead of leaving.
			close(sock_descr);
			exit(1);
		}
		count++; //debug
		printf("packet count = %ld\r",count);
	 	//We add the samples received from network to a ring buffer
 		sample_ring_buffer_write(ring_buffer, buf,  NBR_SAMPLES_IN_PACKET); //Put received data tothe ring buffer
	}
}


