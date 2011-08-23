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
#include <unistd.h> //usleep

#include "config.h"
#include "vixen.h"


#ifdef CHUMDEBUG
#define DEBUG_LOG printf
#else
#define DEBUG_LOG
#endif //DEBUG


// http://www.lemoda.net/unix/base64/index.html
#define b64_pton __b64_pton
/* There are no header files for these functions. */
extern "C" {
	int b64_pton(char const *src, unsigned char *target, size_t targsize);
}


VixenSequenceFile::VixenSequenceFile(const char *filename)
: doc(filename),
  time(0),
  eventPeriod(0),
  numChannels(0),
  buff_offset(1),
  channel_count(0),
  channelData(NULL)
{
	bool loadOkay = doc.LoadFile();
	if (!loadOkay)
		printf("Failed to load file \"%s\"\n", filename);

	TiXmlHandle hDoc(&doc);
	TiXmlElement *pElem = NULL;
	TiXmlHandle hRoot(0);

	// Parse Time
	pElem = hDoc.FirstChildElement().FirstChild( "Time" ).Element();
	if ( pElem )
		time = atoi(pElem->GetText());

	// Parse EventPeriod
	pElem = hDoc.FirstChildElement().FirstChild( "EventPeriodInMilliseconds" ).Element();
	if ( pElem )
		eventPeriod = atoi(pElem->GetText());

	// Parse Channels
	// TODO: Do we need to store these differently?  Do we care about anything but count?
	pElem = hDoc.FirstChildElement().FirstChild( "Channels" ).Element();
	if ( pElem )
		for ( TiXmlNode* pChild = pElem->FirstChild(); pChild; pChild = pChild->NextSibling() )
			numChannels++;

	// Parse Event Values
	pElem = hDoc.FirstChildElement().FirstChild( "EventValues" ).Element();
	if ( pElem )
	{
		// Base64 decode our data.  We know the resulting size from previously
		// parsed data which we can use for our buffer.
        channelData = (unsigned char *)malloc(time/eventPeriod*numChannels);
        b64_pton(pElem->GetText(), channelData, time/eventPeriod*numChannels);
	}
	else
		printf("No event values found\n");

	// Start our buffer
	buff[0] = 0x7E; //sync byte
	buff[1] = 0x80; //address 0x80
}

VixenSequenceFile::~VixenSequenceFile()
{
	free(channelData);
	channelData = NULL;
}

void VixenSequenceFile::debug()
{
	printf("\nlength: %d\n", time/eventPeriod*numChannels);
	printf("time: %d\n", time);
	printf("eventPeriod: %d\n", eventPeriod);
	printf("Channel Data: %d\n", time / eventPeriod);

	channelizeData();
}

void VixenSequenceFile::print_data( unsigned char *buff, int length )
{
	int i;
	for ( i = 0; i < length; i++ )
	{
		printf(" 0x%02x", buff[i]);
		if ( (i+1) % 10 == 0 )
			printf("\n");
		else if ( (i+1) % 5 == 0 )
			printf(" ");
	}

	// Print a newline if we didn't just print one
	if ( ( i+1) % 5 != 0 )
		printf("\n");
}

void VixenSequenceFile::channelizeData()
{
	for (int i = 0; i < numChannels; i++)
	{
		printf("Channel %d:\n", i+1);
		print_data(&channelData[i*time/eventPeriod], time/eventPeriod);
	}
}

void VixenSequenceFile::addByte(unsigned char byte, VixSerial *serial)
{
	buff_offset++;

	// Special Character Handling
	switch(byte)
	{
		// Pad Byte
		case 0x7D:
			buff[buff_offset] = 0x7F;
			buff_offset++;
			buff[buff_offset] = 0x2F;
			break;
		// Sync Byte
		case 0x7E:
			buff[buff_offset] = 0x7F;
			buff_offset++;
			buff[buff_offset] = 0x30;
			break;
		// Escape Byte
		case 0x7F:
			buff[buff_offset] = 0x7F;
			buff_offset++;
			buff[buff_offset] = 0x31;
			break;
		default:
			buff[buff_offset] = byte; // Set our dimming value
	}
	channel_count++;

	// If we're resetting channels, we'll reset address and everything
	if ( channel_count >= numChannels )
	{
		serial->write(buff, buff_offset+1);
		usleep(49000); // wait 50 ms !!TODO!!

		channel_count=0;

		// Set our buffer address
		buff[1] = 0x80;
		// Reset buffer offset for next round of data
		buff_offset=1;
	}
	// We're an 8-channel boundry, send our current data and reset
	// buffers, but incriment address
	else if ( channel_count % 8 == 0 )
	{
		serial->write(buff, buff_offset+1);
		usleep(49000); // wait 50 ms !!TODO!!

		// Set our buffer address
		buff[1] = 0x80 + (channel_count/8);
		// Reset buffer offset for next round of data
		buff_offset=1;
	}
}

void VixenSequenceFile::serializeData(VixSerial *serial)
{
	for (int i = 0; i < time/eventPeriod; i++)
		for (int j = 0; j < numChannels; j++)
			addByte(channelData[((j*time/eventPeriod)+i)], serial);
}
