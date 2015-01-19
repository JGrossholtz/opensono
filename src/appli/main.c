#include <stdio.h>
#include <string.h>

#include "sample_ring_buffer.h"

#include "network_server.h"
#include "network_client.h"


#define RING_BUFFER_SIZE NBR_SAMPLES_IN_PACKET*5000

#define SERVER "--server"



/*
 * The same binary is used for both client and server.
 * For server the --server option must be used. No option is needed for clients
 *
 */
int main( int argc, char *argv[] ){
	
	if(argv[1] != NULL && strncmp(argv[1],SERVER,strlen(SERVER)) == 0){ //Server mode
		printf("Starting openson as Server (recorder) \n");
		init_tcp_socket();
		/*init_multicast_server();*/
		start_acquisition();
	}else{
		ring_buffer_T* buffer =  init_sample_ring_buffer(RING_BUFFER_SIZE);
		printf("Starting openson as client (player)\n");
		init_tcp_client(buffer);
//		init_multicast_client(buffer);
		printf("client network started\n");
		sleep(1);//wait until we receive some data
 		start_playback (buffer);
	}

}
