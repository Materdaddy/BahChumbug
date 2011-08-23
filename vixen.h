/*
 * Copyright 2011 Mathew Mrosko
 *
 * BahChumbug is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License v2 as published
 * by the Free Software Foundation.
 *
 * BahChumbug is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with BahChumbug.  If not, see <http://www.gnu.org/licenses/>.
 */
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
	char *getMusicFilename();

	void debug();

private:
	void print_data( unsigned char *buff, int length );
	TiXmlDocument doc;
	int time;
	int eventPeriod;
	int numChannels;
	unsigned char *channelData;

	char *audio;

	void addByte(unsigned char byte, VixSerial *serial);

	int channel_count;
	int buff_offset;
	unsigned char buff[MAX_BUFFER];
};

#endif //__VIXEN_H__
