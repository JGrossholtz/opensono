#include <stdio.h>

#include "util.h"

int main(){
	init_multicast_server();

	int data = 0;
	char buff[100];

	sleep(2);
	while(1){
		
		snprintf(buff,100,"data=%d",data);
		data++;

		printf("sending %d\n",data -1);
		multicast_server_send(buff,100 * sizeof(char));

		sleep(1);
	}

}
