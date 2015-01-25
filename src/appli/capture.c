#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>
#include <stdint.h>

#include "network_client_server.h"

	
int start_acquisition()
{
	int err;
	sample buf[NBR_SAMPLES_IN_PACKET];
	snd_pcm_t *capture_handle;
	snd_pcm_hw_params_t *hw_params;
	snd_pcm_sframes_t frames_to_deliver;
	unsigned int rate; 
	printf("Samples in packet = %d (%d frames) nbr channels = %d\n",NBR_SAMPLES_IN_PACKET,NBR_FRAMES_IN_PACKET,NBR_CHANNELS);

	/***********************************************************************************************************

	 PLAYER PCM device : Hardware configuration  TODO : do a function of this : it's the same code as the player

	 ***********************************************************************************************************/

	//Open a PCM (Pulse Code Modulation) device (sound card) for playback. The "default" argument means we will
	// use the default pcm device. This device can be set by the user (if there are more than one device)
	if ((err = snd_pcm_open (&capture_handle, "default", SND_PCM_STREAM_CAPTURE, 0)) < 0) {
		fprintf (stderr, "cannot open audio device default (%s)\n", 
				snd_strerror (err));
		exit (1);
	}
	  
	//Allocate the Alsa hardware parameters structure
	if ((err = snd_pcm_hw_params_malloc (&hw_params)) < 0) {
		fprintf (stderr, "cannot allocate hardware parameter structure (%s)\n",
			 snd_strerror (err));
		exit (1);
	}
			 
	//This Alsa function set defautl values to the HW configuration structure for the given PCM device.	 
	if ((err = snd_pcm_hw_params_any (capture_handle, hw_params)) < 0) {
		fprintf (stderr, "cannot initialize hardware parameter structure (%s)\n",
			 snd_strerror (err));
		exit (1);
	}

	//This change the sample input/output format.It just works with interleaved data but TODO it may worth the try with 
	//other configurations. in Interleaved mode the we get one sample from teh first channel and then one
	// sample for the second channel etc... In non interleaved mode we receive all samples from one chanel for a period
	// and then all samples from the second chanel for the same period.
	// (Remark : a period is the number of frames in between each hardware interrupt).
	if ((err = snd_pcm_hw_params_set_access (capture_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
		fprintf (stderr, "cannot set access type (%s)\n",
			 snd_strerror (err));
		exit (1);
	}

	//Set the sample format data to SND_PCM_FORMAT_S32_LE : Signed 32 bit Little Endian.
	//TODO : worth the try with other options such as SND_PCM_FORMAT_FLOAT64_LE for better quality
	if ((err = snd_pcm_hw_params_set_format (capture_handle, hw_params, ALSA_SAMPLE_PCM_FORMAT  )) < 0) {
		fprintf (stderr, "cannot set sample format (%s)\n",
				snd_strerror (err));
		exit (1);
	}


	rate= ALSA_SAMPLE_RATE;	
	//Here we set the sample rate at 44100Hz. The problem here is it's only a request : the real sample rate can be slightly different.
	// This is the purpose of the last parameter of this funcition : it can give you the offset. TODO : take care of this offset value.
	if ((err = snd_pcm_hw_params_set_rate_near (capture_handle, hw_params, &rate, 0)) < 0) {
		fprintf (stderr, "cannot set sample rate (%s)\n",
			 snd_strerror (err));
		exit (1);
	}

	
	//Set the number of channels : we want stereo so we set 2 channels
	if ((err = snd_pcm_hw_params_set_channels (capture_handle, hw_params, NBR_CHANNELS)) < 0) {
		fprintf (stderr, "cannot set channel count (%s)\n",
			 snd_strerror (err));
		exit (1);
	}

	//Set the HW configuration we have just prepared to the device. This function also prepare the device.
	if ((err = snd_pcm_hw_params (capture_handle, hw_params)) < 0) {
		fprintf (stderr, "cannot set parameters (%s)\n",
			 snd_strerror (err));
		exit (1);
	}
	
	
	//free the HW param structure.
	snd_pcm_hw_params_free (hw_params);

	if ((err = snd_pcm_prepare (capture_handle)) < 0) {
		fprintf (stderr, "cannot prepare audio interface for use (%s)\n",
			 snd_strerror (err));
		exit (1);
	}
	
	
	for (;;) {
		//Read frames from the PCM interface
		if ((err = snd_pcm_readi (capture_handle, buf, NBR_FRAMES_IN_PACKET)) != NBR_FRAMES_IN_PACKET) {
			fprintf (stderr, "read from audio interface failed (%s)\n",
			snd_strerror (err));
			exit (1);
		}
		//Send those samples to the network part of this program
		multicast_server_send(buf);
	}

	snd_pcm_close (capture_handle);
	exit (0);
}
