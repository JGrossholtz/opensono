#ifndef NETWORK_CLIENT_SERVER____H
#define NETWORK_CLIENT_SERVER____H

#define MULTICAST_GROUP	  "239.0.0.1"
#define OPENSONO_DATA_PORT 5005

typedef	uint32_t	sample;
#define NBR_SAMPLES_IN_PACKET	512
#define SAMPLE_SIZE				sizeof(sample)
#define PAYLOAD_PACKET_SIZE 	NBR_SAMPLES_IN_PACKET*SAMPLE_SIZE
#define PACKET_SIZE				PAYLOAD_PACKET_SIZE 				//for now...

#endif
