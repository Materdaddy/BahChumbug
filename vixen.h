#ifndef __VIXEN_H__
#define __VIXEN_H__

#include <tinyxml.h>
#include "serial.h"

/*
 * Max size for 8 bytes of channel data is 18 bytes:
 * 1 sync byte, 1 address byte, then 8 channel values
 * that could be escaped for 16 additional bytes,
 * totaling our maximum of 18 bytes.
 */
#define MAX_BUFFER	18


class VixenSequenceFile
{
public:
	VixenSequenceFile(const char *filename);
	~VixenSequenceFile();
	void channelizeData();
	void serializeData(VixSerial *serial);

	void debug();

private:
	void print_data( unsigned char *buff, int length );
	TiXmlDocument doc;
	int time;
	int eventPeriod;
	int numChannels;
	unsigned char *channelData;

	void addByte(unsigned char byte, VixSerial *serial);

	int channel_count;
	int buff_offset;
	unsigned char buff[MAX_BUFFER];
};

#endif //__VIXEN_H__
