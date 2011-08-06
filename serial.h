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
