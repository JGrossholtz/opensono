#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <poll.h>
#include <alsa/asoundlib.h>
#include <stdint.h>


#include "sample_ring_buffer.h"

#include "network_client.h"
#include "network_client_server.h"

#define MAX_FRAMES_DELIVERED_TO_ALSA 1024 

static snd_pcm_t *playback_handle;
static ring_buffer_T *ring_buffer = NULL;

static sample *data_for_alsa = NULL;

int deliver_samples_to_sound_iface (snd_pcm_sframes_t nframes){
	int err = 0;

	nframes = nframes > MAX_FRAMES_DELIVERED_TO_ALSA ? MAX_FRAMES_DELIVERED_TO_ALSA : nframes;

	nframes =  sample_ring_buffer_read(ring_buffer, data_for_alsa, nframes);

	if ((err = snd_pcm_writei (playback_handle,data_for_alsa ,nframes)) < 0) {
		fprintf (stderr, "write failed (%s) (expect:%d)\n", snd_strerror (err),(int)nframes);
	}


	return err;
}

void start_playback (ring_buffer_T *buffer){
		snd_pcm_hw_params_t *hw_params;
		snd_pcm_sw_params_t *sw_params;
		snd_pcm_sframes_t space_left_in_hw_buffer;
		int nfds;
		int err;
		struct pollfd *pfds;
		unsigned int rate;

		data_for_alsa = malloc( MAX_FRAMES_DELIVERED_TO_ALSA * sizeof(sample));

		//first allocate some memory : maximum requested size from alsa. this is the buffer we will pass to alsa
		ring_buffer	= buffer;

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
		if ((err = snd_pcm_sw_params_set_avail_min (playback_handle, sw_params, 8192)) < 0) {
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
			//We wait until the audio interface is ready to receive more data. This may make us wait a bit 
			//if the alsa driver fifo(ring buffer) is full. Normally we are never in this case.
			//In worst case we wait 300ms but it should not happen, the appli is working even with 1ms timeout.
			if ((err = snd_pcm_wait (playback_handle, 300)) < 0) {
			        fprintf (stderr, "poll failed (%s)\n", strerror (errno));
			        break;
			}	           
	
			// Here we check if there is some space into the driver fifo.
			// Should always be the case. The fifo is several seconds long.
			if ((space_left_in_hw_buffer = snd_pcm_avail_update (playback_handle)) < 0) {
				if (space_left_in_hw_buffer == -EPIPE) {
					fprintf (stderr, "an xrun occured\n");
					break;
				} else {
					fprintf (stderr, "unknown ALSA avail update return value (%d)\n", 
						 (int)space_left_in_hw_buffer);
					break;
				}
			}
	
	
			/*Request samples to deliver to the sound interface */
			if (deliver_samples_to_sound_iface (space_left_in_hw_buffer) < 0 ) {
			        fprintf (stderr, "playback callback failed\n");
			}
		}
	
		snd_pcm_close (playback_handle);
		exit (0);
	}

