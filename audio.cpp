/*
	mpg123_to_wav.c

	copyright 2007 by the mpg123 project - free software under the terms of the LGPL 2.1
	see COPYING and AUTHORS files in distribution or http://mpg123.org
	initially written by Nicholas Humfrey
*/

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include "audio.h"


void cleanup(mpg123_handle *mh)
{
	/* It's really to late for error checks here;-) */
	mpg123_close(mh);
	mpg123_delete(mh);
	mpg123_exit();
}

VixSound::VixSound(const char *inFilename)
: mh(NULL),
  buffer(NULL),
  buffer_size(0),
  done(0),
  channels(0),
  encoding(0),
  rate(0),
  err(MPG123_OK),
  samples(0),
  pcm_handle(NULL)
{
	filename=strdup(inFilename);
}

VixSound::~VixSound()
{
}

int VixSound::initMpg123()
{
	err = mpg123_init();
	if( err != MPG123_OK || (mh = mpg123_new(NULL, &err)) == NULL
	    /* Let mpg123 work with the file, that excludes MPG123_NEED_MORE messages. */
	    || mpg123_open(mh, filename) != MPG123_OK
	    /* Peek into track and get first output format. */
	    || mpg123_getformat(mh, &rate, &channels, &encoding) != MPG123_OK )
	{
		fprintf( stderr, "Trouble with mpg123: %s\n",
		         mh==NULL ? mpg123_plain_strerror(err) : mpg123_strerror(mh) );
//		cleanup(mh);
		return -1;
	}

	printf("Rate: %ld\nChannels: %d\nEncoding: %d (0x%x)\n", rate, channels, encoding, encoding);

	if(encoding != MPG123_ENC_SIGNED_16)
	{ /* Signed 16 is the default output format anyways; it would actually by only different if we forced it.
	     So this check is here just for this explanation. */
		cleanup(mh);
		fprintf(stderr, "Bad encoding: 0x%x!\n", encoding);
		return -2;
	}
	/* Ensure that this output format will not change (it could, when we allow it). */
	mpg123_format_none(mh);
	mpg123_format(mh, rate, channels, encoding);

	/* Buffer could be almost any size here, mpg123_outblock() is just some recommendation.
	   Important, especially for sndfile writing, is that the size is a multiple of sample size. */
	buffer_size = mpg123_outblock( mh );
	buffer = (unsigned char*)malloc( buffer_size );
}

int VixSound::initAlsa()
{

    /* Playback stream */
    snd_pcm_stream_t stream = SND_PCM_STREAM_PLAYBACK;

    /* This structure contains information about    */
    /* the hardware and can be used to specify the  */      
    /* configuration to be used for the PCM stream. */ 
    snd_pcm_hw_params_t *hwparams;

//The most important ALSA interfaces to the PCM devices are the "plughw" and the "hw" interface. If you use the "plughw" interface, you need not care much about the sound hardware. If your soundcard does not support the sample rate or sample format you specify, your data will be automatically converted. This also applies to the access type and the number of channels. With the "hw" interface, you have to check whether your hardware supports the configuration you would like to use.

    /* Name of the PCM device, like plughw:0,0          */
    /* The first number is the number of the soundcard, */
    /* the second number is the number of the device.   */
    char *pcm_name;

//Then we initialize the variables and allocate a hwparams structure:

    /* Init pcm_name. Of course, later you */
    /* will make this configurable ;-)     */
    pcm_name = strdup("plughw:0,0");

    /* Allocate the snd_pcm_hw_params_t structure on the stack. */
    snd_pcm_hw_params_alloca(&hwparams);

//Now we can open the PCM device:

    /* Open PCM. The last parameter of this function is the mode. */
    /* If this is set to 0, the standard mode is used. Possible   */
    /* other values are SND_PCM_NONBLOCK and SND_PCM_ASYNC.       */ 
    /* If SND_PCM_NONBLOCK is used, read / write access to the    */
    /* PCM device will return immediately. If SND_PCM_ASYNC is    */
    /* specified, SIGIO will be emitted whenever a period has     */
    /* been completely processed by the soundcard.                */
    if (snd_pcm_open(&pcm_handle, pcm_name, stream, 0) < 0) {
      fprintf(stderr, "Error opening PCM device %s\n", pcm_name);
      return(-1);
    }

//Before we can write PCM data to the soundcard, we have to specify access type, sample format, sample rate, number of channels, number of periods and period size. First, we initialize the hwparams structure with the full configuration space of the soundcard.

    /* Init hwparams with full configuration space */
    if (snd_pcm_hw_params_any(pcm_handle, hwparams) < 0) {
      fprintf(stderr, "Can not configure this PCM device.\n");
      return(-1);
    }

//Information about possible configurations can be obtained with a set of functions named
//    snd_pcm_hw_params_can_<capability>
//    snd_pcm_hw_params_is_<property>
//    snd_pcm_hw_params_get_<parameter>
//
//The availability of the most important parameters, namely access type, buffer size, number of channels, sample format, sample rate and number of periods, can be tested with a set of functions named
//    snd_pcm_hw_params_test_<parameter> 
//
//These query functions are especially important if the "hw" interface is used. The configuration space can be restricted to a certain configuration with a set of functions named
//    snd_pcm_hw_params_set_<parameter>
//
//For this example, we assume that the soundcard can be configured for stereo playback of 16 Bit Little Endian data, sampled at 44100 Hz. Accordingly, we restrict the configuration space to match this configuration:
    int exact_rate;   /* Sample rate returned by */
                      /* snd_pcm_hw_params_set_rate_near */ 
    int dir;          /* exact_rate == rate --> dir = 0 */
                      /* exact_rate < rate  --> dir = -1 */
                      /* exact_rate > rate  --> dir = 1 */
    int periods = 2;       /* Number of periods */
    snd_pcm_uframes_t periodsize = 8192; /* Periodsize (bytes) */

//The access type specifies the way in which multichannel data is stored in the buffer. For INTERLEAVED access, each frame in the buffer contains the consecutive sample data for the channels. For 16 Bit stereo data, this means that the buffer contains alternating words of sample data for the left and right channel. For NONINTERLEAVED access, each period contains first all sample data for the first channel followed by the sample data for the second channel and so on.

    /* Set access type. This can be either    */
    /* SND_PCM_ACCESS_RW_INTERLEAVED or       */
    /* SND_PCM_ACCESS_RW_NONINTERLEAVED.      */
    /* There are also access types for MMAPed */
    /* access, but this is beyond the scope   */
    /* of this introduction.                  */
    if (snd_pcm_hw_params_set_access(pcm_handle, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED) < 0) {
      fprintf(stderr, "Error setting access.\n");
      return(-1);
    }

    /* Set sample format */
    if (snd_pcm_hw_params_set_format(pcm_handle, hwparams, SND_PCM_FORMAT_S16_LE) < 0) {
      fprintf(stderr, "Error setting format.\n");
      return(-1);
    }

    /* Set sample rate. If the exact rate is not supported */
    /* by the hardware, use nearest possible rate.         */
    exact_rate = rate;
    if (snd_pcm_hw_params_set_rate_near(pcm_handle, hwparams, (unsigned int *)&exact_rate, 0) < 0) {
      fprintf(stderr, "Error setting rate.\n");
      return(-1);
    }
    if (rate != exact_rate) {
      fprintf(stderr, "The rate %ld Hz is not supported by your hardware.\n"
                      " ==> Using %d Hz instead.\n", rate, exact_rate);
    }

    /* Set number of channels */
    if (snd_pcm_hw_params_set_channels(pcm_handle, hwparams, 2) < 0) {
      fprintf(stderr, "Error setting channels.\n");
      return(-1);
    }

    /* Set number of periods. Periods used to be called fragments. */
    if (snd_pcm_hw_params_set_periods(pcm_handle, hwparams, periods, 0) < 0) {
      fprintf(stderr, "Error setting periods.\n");
      return(-1);
    }

//The unit of the buffersize depends on the function. Sometimes it is given in bytes, sometimes the number of frames has to be specified. One frame is the sample data vector for all channels. For 16 Bit stereo data, one frame has a length of four bytes.

    /* Set buffer size (in frames). The resulting latency is given by */
    /* latency = periodsize * periods / (rate * bytes_per_frame)     */
    if (snd_pcm_hw_params_set_buffer_size(pcm_handle, hwparams, (periodsize * periods)>>2) < 0) {
      fprintf(stderr, "Error setting buffersize.\n");
      return(-1);
    }

//If your hardware does not support a buffersize of 2^n, you can use the function snd_pcm_hw_params_set_buffer_size_near. This works similar to snd_pcm_hw_params_set_rate_near. Now we apply the configuration to the PCM device pointed to by pcm_handle. This will also prepare the PCM device.

    /* Apply HW parameter settings to */
    /* PCM device and prepare device  */
    if (snd_pcm_hw_params(pcm_handle, hwparams) < 0) {
      fprintf(stderr, "Error setting HW params.\n");
      return(-1);
    }
}

int VixSound::playFile()
{
	usleep(55000);
//After the PCM playback is started, we have to make sure that our application sends enough data to the soundcard buffer. Otherwise, a buffer underrun will occur. After such an underrun has occured, snd_pcm_prepare should be called. A simple stereo saw wave could be generated this way:
    int pcmreturn;
    int frames;

    frames = buffer_size >> 2;
	do
	{
		err = mpg123_read( mh, buffer, buffer_size, &done );
        while ((pcmreturn = snd_pcm_writei(pcm_handle, buffer, frames)) < 0)
        {
            snd_pcm_prepare(pcm_handle);
            fprintf(stderr, "<<<<<<<<<<<<<<< Buffer Underrun >>>>>>>>>>>>>>>\n");
        }
		samples += done/sizeof(short);
		/* We are not in feeder mode, so MPG123_OK, MPG123_ERR and MPG123_NEW_FORMAT are the only possibilities.
		   We do not handle a new format, MPG123_DONE is the end... so abort on anything not MPG123_OK. */
	} while (err==MPG123_OK);

	if(err != MPG123_DONE)
	fprintf( stderr, "Warning: Decoding ended prematurely because: %s\n",
	         err == MPG123_ERR ? mpg123_strerror(mh) : mpg123_plain_strerror(err) );

	samples /= channels;
	printf("%li samples written.\n", (long)samples);
//	cleanup(mh);
	return 0;
}

void *VixSound::startThread( void *data )
{
	VixSound *blah = (VixSound *)data;
	return (void *)blah->playFile();
}

void VixSound::threadAudio()
{
	pthread_create( &music_thread, NULL, &VixSound::startThread, (void *)this);
}

void VixSound::waitAudio()
{
	pthread_join( music_thread, NULL );
}
