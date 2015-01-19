#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>
#include <stdint.h>
	
#include "network_client_server.h"


int start_acquisition(){
	int i;
	int err;
	unsigned int rate = ALSA_SAMPLE_RATE;	
	sample buf[NBR_SAMPLES_IN_PACKET];
	snd_pcm_t *capture_handle;
	snd_pcm_hw_params_t *hw_params;
	snd_pcm_sframes_t frames_to_deliver;
	snd_pcm_sw_params_t *sw_params;

	printf("nbr sample in packet = %d (%d frames) nbr channels = %d\n",NBR_SAMPLES_IN_PACKET,NBR_FRAMES_IN_PACKET,NBR_CHANNELS);

	/***************************************************************************************************

	  PLAYER PCM device : Hardware configuration  TODO : do a function of this : it's the same code as the player

	 ***************************************************************************************************/

	//Open a PCM (Pulse Code Modulation) device (sound card) for playback. The "default" argument means we will
	// use the default pcm device. This device can be set by the user (if there are more than one device)
	if ((err = snd_pcm_open (&capture_handle, "plughw:0,0", SND_PCM_STREAM_CAPTURE, 0)) < 0) {
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

	//Here we set the sample rate at 44100Hz. The problem here is it's only a request : the real sample rate can be slightly different.
	// This is the purpose of the last parameter of this funcition : it can give you the offset. TODO : take care of this offset value.
	if ((err = snd_pcm_hw_params_set_rate_near (capture_handle, hw_params, &rate, 0)) < 0) {
		fprintf (stderr, "cannot set sample rate (%s)\n",
				snd_strerror (err));
		exit (1);
	}

	printf("REAL rate = %d\n",rate);

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










	/***************************************************************************************************

	  					PLAYER PCM device : Software configuration

	 ***************************************************************************************************/

	//Allocate the Alsa software parameters structure
	if ((err = snd_pcm_sw_params_malloc (&sw_params)) < 0) {
		fprintf (stderr, "cannot allocate software parameters structure (%s)\n",
				snd_strerror (err));
		exit (1);
	}

	//rRetreive the current SW configuration
	if ((err = snd_pcm_sw_params_current (capture_handle, sw_params)) < 0) {
		fprintf (stderr, "cannot initialize software parameters structure (%s)\n",
				snd_strerror (err));
		exit (1);
	}

	//Here we configure what is the minimal available frames count in the PCM fifo to wake up and write data.
	//As we will keep the data into this buffer vry low we will never reach this minimal value and we will be active permanently.
	if ((err = snd_pcm_sw_params_set_avail_min (capture_handle, sw_params, 2048)) < 0) {
		fprintf (stderr, "cannot set minimum available count (%s)\n",
				snd_strerror (err));
		exit (1);
	}

	//We set here when alsa should start playing : how may samples must be in the buffer before it start to emit a sound. 
	//As we handle a ring buffer by ourselves into this applucation we do not want to wait at all.
	if ((err = snd_pcm_sw_params_set_start_threshold (capture_handle, sw_params, 2048)) < 0) {
		fprintf (stderr, "cannot set start mode (%s)\n",
				snd_strerror (err));
		exit (1);
	}

	//Apply the SW configuration to the PCM device.
	if ((err = snd_pcm_sw_params (capture_handle, sw_params)) < 0) {
		fprintf (stderr, "cannot set software parameters (%s)\n",
				snd_strerror (err));
		exit (1);
	}

	//Prepare the PCM device to be used.
	if ((err = snd_pcm_prepare (capture_handle)) < 0) {
		fprintf (stderr, "cannot prepare audio interface for use (%s)\n",
				snd_strerror (err));
		exit (1);
	}






	 snd_pcm_nonblock(capture_handle,0);







	snd_pcm_sframes_t space_left_in_hw_buffer;


	for (;;) {
		
		
		//We wait until the audio interface is ready to receive more data. This may make us wait a bit 
		//if the alsa driver fifo(ring buffer) is full. Normally we are never in this case.
		//In worst case we wait 300ms but it should not happen, the appli is working even with 1ms timeout.
	/*	if ((err = snd_pcm_wait (capture_handle, 300)) < 0) {
			switch(err){
				case -EPIPE:
					fprintf (stderr, "error -EPIPE : xrun (%s)\n", strerror (errno));
				break;
				case -ESTRPIPE:
					fprintf (stderr, "-ESTRPIPE :suspended  (%s)\n", strerror (errno));
				break;
				default:
					fprintf (stderr, "poll failed (%s)\n", strerror (errno));
				break;	
			}
			break;
		}*/	           

		// Here we check if there is some space into the driver fifo.
		// Should always be the case. The fifo is several seconds long.
		// This function returns the numbers of frames : number of samples multiplied by the number of channels.
		if ((space_left_in_hw_buffer = snd_pcm_avail_update (capture_handle)) < 0) {
			if (space_left_in_hw_buffer == -EPIPE) {
				fprintf (stderr, "an xrun occured\n");
				break;
			} else {
				fprintf (stderr, "unknown ALSA avail update return value (%d)\n", 
						(int)space_left_in_hw_buffer);
				break;
			}
		}
	
		printf("available=%d (%d)\n",space_left_in_hw_buffer,NBR_FRAMES_IN_PACKET);
		if(space_left_in_hw_buffer<NBR_FRAMES_IN_PACKET){
			printf("oups !\n");
			usleep(300000);
		}


//Read frames from the PCM interface
		if ((err = snd_pcm_readi (capture_handle, buf, NBR_FRAMES_IN_PACKET)) != NBR_FRAMES_IN_PACKET) {
			fprintf (stderr, "read from audio interface failed (%s)\n",
					snd_strerror (err));
			exit (1);
		}
		tcp_server_send(buf);
		//			multicast_server_send(buf);
	}

	snd_pcm_close (capture_handle);
	exit (0);
}
