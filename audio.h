#include <mpg123.h>
#include <pthread.h>
#include <alsa/asoundlib.h>

class VixSound
{
public:
	VixSound(const char *inFilename);
	~VixSound();
	int initMpg123();
	int initAlsa();
	int playFile();
	void threadAudio();
	void waitAudio();

private:
	static void *startThread( void *data );
	pthread_t music_thread;
	mpg123_handle *mh;
	unsigned char* buffer;
	size_t buffer_size;
	size_t done;
	int  channels;
	int encoding;
	long rate;
	int  err;
	off_t samples;
	char *filename;
	snd_pcm_t *pcm_handle;
};
