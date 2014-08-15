#include <sys/types.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>


#include "network_client_server.h"


int sock_descr;


int init_multicast_client(){
	struct ip_mreq group;
	struct sockaddr_in localSock;

	sock_descr = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock_descr < 0){
		perror("Cannot open socket");
		exit(1);
	}


	/* Enable SO_REUSEADDR to allow multiple instances of this */
	/* application to receive copies of the multicast datagrams. */
	int reuse = 1;

	if(setsockopt(sock_descr, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse)) < 0)	{
		perror("Setting SO_REUSEADDR error");
		close(sock_descr);
		exit(1);
	}

	/* Bind to the proper port number with the IP address */
	/* specified as INADDR_ANY. */
	memset((char *) &localSock, 0, sizeof(localSock));
	localSock.sin_family = AF_INET;
	localSock.sin_port = htons(OPENSONO_DATA_PORT);
	localSock.sin_addr.s_addr = INADDR_ANY;

	if(bind(sock_descr, (struct sockaddr*)&localSock, sizeof(localSock)))	{
		perror("Binding datagram socket error");
		close(sock_descr);
		exit(1);
	}



	/* Join the multicast group 226.1.1.1 on the local 203.106.93.94 */
	/* interface. Note that this IP_ADD_MEMBERSHIP option must be */
	/* called for each local interface over which the multicast */
	/* datagrams are to be received. */
	group.imr_multiaddr.s_addr = inet_addr(MULTICAST_GROUP);
	group.imr_interface.s_addr = inet_addr("192.168.1.20");

	if(setsockopt(sock_descr, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&group, sizeof(group)) < 0){
		perror("Adding multicast group error");
		close(sock_descr);
		exit(1);
	}
	printf("client network ready\n");

}


size_t multicast_client_receive(uint32_t * buf, size_t length){
	ssize_t count;
	printf("would like to read:%d\n",length);
	if(count = /*read(sock_descr, buf, 32)*/ recv(sock_descr, buf, 32,0) < 0){
		perror("Reading datagram message error");
		close(sock_descr);
		exit(1);
	}

}
