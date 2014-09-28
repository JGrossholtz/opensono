#include <stdio.h>
#include <string.h>

#include "sample_ring_buffer.h"

#include "network_server.h"
#include "network_client.h"


#define RING_BUFFER_SIZE NBR_SAMPLES_IN_PACKET*50

#define SERVER "--server"

int main( int argc, char *argv[] ){
	
	if(argv[1] != NULL && strncmp(argv[1],SERVER,strlen(SERVER)) == 0){
		printf("Starting openson as Server (recorder) \n");
		init_multicast_server();
		start_acquisition();
	}else{
		ring_buffer_T* buffer =  init_sample_ring_buffer(RING_BUFFER_SIZE);
		printf("Starting openson as client (player)\n");
		init_multicast_client(buffer);
		printf("client network started\n");
		sleep(1);//wait until we receive some data
 		start_playback (buffer);
	}

}
