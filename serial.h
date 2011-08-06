#ifndef __VIXSERIAL_H__
#define __VIXSERIAL_H__

#include "termios.h"

class VixSerial
{
public:
	// Init/Setup Routines
	VixSerial(const char *device, int flags);
	~VixSerial();
	int setBaud(int baud = 57600);
	int setInBaud(int baud = 57600);
	int setOutBaud(int baud = 57600);
	int configure(const char *configuration = "8N1");
	int softFlowControl(bool enable = false );
	int makeRaw(bool enable = true );

	// Operations
	int flush();
	int write(unsigned char *data, int length);

private:
	int setBaudPriv(int (*functor)(termios*, speed_t), int baud);
	int configurationLookup(const char *configuration);

	int fd;
	int baud;
	struct termios config;

	enum { CONFIG_8N1,
	       CONFIG_7E1,
	       CONFIG_7O1,
	       CONFIG_7S1,
	       CONFIG_NONE };
};


#endif //__VIXSERIAL_H__
