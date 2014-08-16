#include <stdio.h>
#include <string.h>

#include "network_server.h"
#include "network_client.h"


#define SERVER "--server"

int main( int argc, char *argv[] ){
	
	if(argv[1] != NULL && strncmp(argv[1],SERVER,strlen(SERVER)) == 0){
		printf("Starting openson as Server (recorder) \n");
		init_multicast_server();
		start_acquisition();
	}else{
		printf("Starting openson as client (player)\n");
		init_multicast_client();
		printf("client network started\n");
 		start_playback ();
	}


}
