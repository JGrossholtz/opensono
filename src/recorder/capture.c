	#include <stdio.h>
	#include <stdlib.h>
	#include <alsa/asoundlib.h>
	#include <stdint.h>
	
	#define BUFFSIZE 8
	main (int argc, char *argv[])
	{
		int i;
		int err;
		uint32_t buf[BUFFSIZE];
		snd_pcm_t *capture_handle;
		snd_pcm_hw_params_t *hw_params;
		snd_pcm_sframes_t frames_to_deliver;
		
		if ((err = snd_pcm_open (&capture_handle, argv[1], SND_PCM_STREAM_CAPTURE, 0)) < 0) {
			fprintf (stderr, "cannot open audio device %s (%s)\n", 
				 argv[1],
				 snd_strerror (err));
			exit (1);
		}
		   
		if ((err = snd_pcm_hw_params_malloc (&hw_params)) < 0) {
			fprintf (stderr, "cannot allocate hardware parameter structure (%s)\n",
				 snd_strerror (err));
			exit (1);
		}
				 
		if ((err = snd_pcm_hw_params_any (capture_handle, hw_params)) < 0) {
			fprintf (stderr, "cannot initialize hardware parameter structure (%s)\n",
				 snd_strerror (err));
			exit (1);
		}
	
		if ((err = snd_pcm_hw_params_set_access (capture_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
			fprintf (stderr, "cannot set access type (%s)\n",
				 snd_strerror (err));
			exit (1);
		}
	
		if ((err = snd_pcm_hw_params_set_format (capture_handle, hw_params, SND_PCM_FORMAT_S32_LE)) < 0) {
			fprintf (stderr, "cannot set sample format (%s)\n",
				 snd_strerror (err));
			exit (1);
		}
		unsigned int rate = 44100;	
		if ((err = snd_pcm_hw_params_set_rate_near (capture_handle, hw_params, &rate, 0)) < 0) {
			fprintf (stderr, "cannot set sample rate (%s)\n",
				 snd_strerror (err));
			exit (1);
		}
	
		if ((err = snd_pcm_hw_params_set_channels (capture_handle, hw_params, 1)) < 0) {
			fprintf (stderr, "cannot set channel count (%s)\n",
				 snd_strerror (err));
			exit (1);
		}
	
		if ((err = snd_pcm_hw_params (capture_handle, hw_params)) < 0) {
			fprintf (stderr, "cannot set parameters (%s)\n",
				 snd_strerror (err));
			exit (1);
		}
	
		snd_pcm_hw_params_free (hw_params);
	
		if ((err = snd_pcm_prepare (capture_handle)) < 0) {
			fprintf (stderr, "cannot prepare audio interface for use (%s)\n",
				 snd_strerror (err));
			exit (1);
		}
		
		FILE *output = fopen("output.raw","w");

		
		frames_to_deliver =  BUFFSIZE;//snd_pcm_avail_update(capture_handle);

		for (;;) {
			if ((err = snd_pcm_readi (capture_handle, buf, frames_to_deliver)) != frames_to_deliver) {
				fprintf (stderr, "read from audio interface failed (%s)\n",
					 snd_strerror (err));
				exit (1);
			}
			fwrite(buf,sizeof(uint32_t),frames_to_deliver,output);
			fflush(output);
			printf("read=%ld pos=%ld\n",frames_to_deliver,ftell(output));
		}
		fclose(output);
	
		snd_pcm_close (capture_handle);
		exit (0);
	}
