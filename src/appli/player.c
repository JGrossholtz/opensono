#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <poll.h>
#include <alsa/asoundlib.h>
#include <stdint.h>

#include "network_client.h"
#include "network_client_server.h"

snd_pcm_t *playback_handle;
sample buf[NBR_SAMPLES_IN_PACKET];
FILE *output;	

int playback_callback (snd_pcm_sframes_t nframes){
	int err = 0;
	usleep(10000);
	err = multicast_client_receive( buf );
	printf("just received from network = %d frames\n",err);
	
	if ((err = snd_pcm_writei (playback_handle, buf, err)) < 0) {
		fprintf (stderr, "write failed (%s)\n", snd_strerror (err));
	}

	return err;
}

void start_playback (){
	
		snd_pcm_hw_params_t *hw_params;
		snd_pcm_sw_params_t *sw_params;
		snd_pcm_sframes_t max_frames_to_deliver;
		int nfds;
		int err;
		struct pollfd *pfds;
		unsigned int rate;
		if ((err = snd_pcm_open (&playback_handle, "default", SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
			fprintf (stderr, "cannot open audio device (%s)\n", 
				 snd_strerror (err));
			exit (1);
		}
		   
		if ((err = snd_pcm_hw_params_malloc (&hw_params)) < 0) {
			fprintf (stderr, "cannot allocate hardware parameter structure (%s)\n",
				 snd_strerror (err));
			exit (1);
		}
				 
		if ((err = snd_pcm_hw_params_any (playback_handle, hw_params)) < 0) {
			fprintf (stderr, "cannot initialize hardware parameter structure (%s)\n",
				 snd_strerror (err));
			exit (1);
		}
	
		if ((err = snd_pcm_hw_params_set_access (playback_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
			fprintf (stderr, "cannot set access type (%s)\n",
				 snd_strerror (err));
			exit (1);
		}
	
		if ((err = snd_pcm_hw_params_set_format (playback_handle, hw_params, SND_PCM_FORMAT_S32_LE)) < 0) {
			fprintf (stderr, "cannot set sample format (%s)\n",
				 snd_strerror (err));
			exit (1);
		}
		
		rate = 44100;	
		if ((err = snd_pcm_hw_params_set_rate_near (playback_handle, hw_params, &rate, 0)) < 0) {
			fprintf (stderr, "cannot set sample rate (%s)\n",
				 snd_strerror (err));
			exit (1);
		}
	
		if ((err = snd_pcm_hw_params_set_channels (playback_handle, hw_params, 1)) < 0) {
			fprintf (stderr, "cannot set channel count (%s)\n",
				 snd_strerror (err));
			exit (1);
		}
	
		if ((err = snd_pcm_hw_params (playback_handle, hw_params)) < 0) {
			fprintf (stderr, "cannot set parameters (%s)\n",
				 snd_strerror (err));
			exit (1);
		}
	
		snd_pcm_hw_params_free (hw_params);
	
		/* tell ALSA to wake us up whenever 4096 or more frames
		   of playback data can be delivered. Also, tell
		   ALSA that we'll start the device ourselves.
		*/
	
		if ((err = snd_pcm_sw_params_malloc (&sw_params)) < 0) {
			fprintf (stderr, "cannot allocate software parameters structure (%s)\n",
				 snd_strerror (err));
			exit (1);
		}
		if ((err = snd_pcm_sw_params_current (playback_handle, sw_params)) < 0) {
			fprintf (stderr, "cannot initialize software parameters structure (%s)\n",
				 snd_strerror (err));
			exit (1);
		}
		if ((err = snd_pcm_sw_params_set_avail_min (playback_handle, sw_params, 4096)) < 0) {
			fprintf (stderr, "cannot set minimum available count (%s)\n",
				 snd_strerror (err));
			exit (1);
		}
		if ((err = snd_pcm_sw_params_set_start_threshold (playback_handle, sw_params, 0U)) < 0) {
			fprintf (stderr, "cannot set start mode (%s)\n",
				 snd_strerror (err));
			exit (1);
		}
		if ((err = snd_pcm_sw_params (playback_handle, sw_params)) < 0) {
			fprintf (stderr, "cannot set software parameters (%s)\n",
				 snd_strerror (err));
			exit (1);
		}
	
		/* the interface will interrupt the kernel every 4096 frames, and ALSA
		   will wake up this program very soon after that.
		*/
	
		if ((err = snd_pcm_prepare (playback_handle)) < 0) {
			fprintf (stderr, "cannot prepare audio interface for use (%s)\n",
				 snd_strerror (err));
			exit (1);
		}

		
		printf("client streaming ready... \n");

		while (1) {
	
			/* wait till the interface is ready for data, or 1 second
			   has elapsed.
			*/
			usleep(10000);
			if ((err = snd_pcm_wait (playback_handle, 1000)) < 0) {
			        fprintf (stderr, "poll failed (%s)\n", strerror (errno));
			        break;
			}	           
	
			/* find out how much space is available for playback data */
	
			if ((max_frames_to_deliver = snd_pcm_avail_update (playback_handle)) < 0) {
				if (max_frames_to_deliver == -EPIPE) {
					fprintf (stderr, "an xrun occured\n");
					break;
				} else {
					fprintf (stderr, "unknown ALSA avail update return value (%d)\n", 
						 (int)max_frames_to_deliver);
					break;
				}
			}
	
			max_frames_to_deliver = max_frames_to_deliver > 4096 ? 4096 : max_frames_to_deliver;
	
			/* deliver the data */
	
			if (playback_callback (max_frames_to_deliver) < 0 ) {
			        fprintf (stderr, "playback callback failed\n");
				//break;
			}
		}
	
		snd_pcm_close (playback_handle);
		exit (0);
	}

