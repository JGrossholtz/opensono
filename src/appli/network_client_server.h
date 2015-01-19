/*
 * This file contains the shared configurations between client and server.
 * Later some of those configs will be replaced by a protocol for dynamic configuration
 *
 */


#ifndef NETWORK_CLIENT_SERVER____H
#define NETWORK_CLIENT_SERVER____H

#define MULTICAST_GROUP	  "239.0.0.1"
#define OPENSONO_DATA_PORT 5005

typedef	int32_t	sample;
#define ALSA_SAMPLE_PCM_FORMAT  SND_PCM_FORMAT_S32_LE
#define ALSA_SAMPLE_RATE		44100

#define NBR_CHANNELS			2
#define NBR_SAMPLES_IN_PACKET	128
#define NBR_FRAMES_IN_PACKET	(NBR_SAMPLES_IN_PACKET/NBR_CHANNELS)
#define SAMPLE_SIZE				sizeof(sample)
#define PAYLOAD_PACKET_SIZE 	NBR_SAMPLES_IN_PACKET*SAMPLE_SIZE
#define PACKET_SIZE				PAYLOAD_PACKET_SIZE 				//for now...

#endif
