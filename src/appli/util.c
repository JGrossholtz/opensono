#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>

#include "common.h"

/* private data*/
#define SIOCGIFADDR_DEFAULT_RETURN "0.0.0.0"



/*
 * Utility function : get the IP adress from the network interface name.
 */
int get_wireless_iface_ipaddr(char * ipstr , int sockfd){
	 struct ifreq ifr;
	 strncpy(ifr.ifr_name, "wlan0", IFNAMSIZ-1); 

	 ioctl(sockfd, SIOCGIFADDR, &ifr);

	 strncpy(ipstr, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr),IPADDR_STR_LEN );

	 if(strncmp(ipstr, SIOCGIFADDR_DEFAULT_RETURN, IPADDR_STR_LEN) == 0 ) {
	 	printf("The requested interface does not have an IP");
		return -1;
	 }

	 return 1;
}
