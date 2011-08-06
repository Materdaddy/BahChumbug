#include <fcntl.h> //open
#include <stdio.h> //printf
#include <stdlib.h> //exit
#include <termios.h> //termios
#include <unistd.h> //usleep
#include <errno.h> //EINVAL
#include <string.h> //strcmp

#include "serial.h"

VixSerial::VixSerial(const char *device, int flags)
: fd(0)
{
	fd = open(device, flags);
	if (fd == -1)
		printf( "failed to open port\n" );

	if ( !isatty(fd) )
	{
		printf("Failed tty check\n");
		exit(1);//TODO:NOTTY?
	}

	if (tcgetattr(fd, &config) < 0)
	{
		printf("Failed getting term config\n");
		exit(1);//TODO:NOTTY?
	}

	//TODO?
	//http://www.linuxforums.org/forum/miscellaneous/130106-tcflush-not-flushing-serial-port.html
	//
	//The delay seems to help flushing, but this could just be related to the problems I've seen with
	//anything using the mct_u232 (Belkin) kernel module instead of the pl232 (Prolific) kernel module
	usleep(200000);
	tcflush(fd, TCOFLUSH);
}

VixSerial::~VixSerial()
{
	tcflush(fd, TCOFLUSH);
	close(fd);
	fd = 0;
}

int VixSerial::setBaud(int baud)
{
	return setInBaud(baud) || setOutBaud(baud);
}

int VixSerial::setInBaud(int baud)
{
	return setBaudPriv(cfsetispeed, baud);
}

int VixSerial::setOutBaud(int baud)
{
	return setBaudPriv(cfsetospeed, baud);
}

int VixSerial::configure(const char *configuration)
{
	switch (configurationLookup(configuration))
	{
		case CONFIG_8N1:
			config.c_cflag &= ~PARENB;
			config.c_cflag &= ~CSTOPB;
			config.c_cflag &= ~CSIZE;
			config.c_cflag |= CS8;
			break;
		case CONFIG_7E1:
			config.c_cflag |= PARENB;
			config.c_cflag &= ~PARODD;
			config.c_cflag &= ~CSTOPB;
			config.c_cflag &= ~CSIZE;
			config.c_cflag |= CS7;
			break;
		case CONFIG_7O1:
			config.c_cflag |= PARENB;
			config.c_cflag |= PARODD;
			config.c_cflag &= ~CSTOPB;
			config.c_cflag &= ~CSIZE;
			config.c_cflag |= CS7;
			break;
		case CONFIG_7S1:
			config.c_cflag &= ~PARENB;
			config.c_cflag &= ~CSTOPB;
			config.c_cflag &= ~CSIZE;
			config.c_cflag |= CS8;
			break;
		default:
			return -1;
	}
	
	return tcsetattr(fd, TCSANOW, &config); //TCSAFLUSH?
}

int VixSerial::softFlowControl(bool enable)
{
	if ( enable )
		config.c_iflag |= (IXON | IXOFF | IXANY);
	else
		config.c_iflag &= ~(IXON | IXOFF | IXANY);

	return tcsetattr(fd, TCSANOW, &config); //TCSAFLUSH?
}

int VixSerial::makeRaw(bool enable)
{
	struct termios config;
	if(!isatty(fd)) { printf("Failed tty check\n"); exit(1); }
	if(tcgetattr(fd, &config) < 0) { printf("Failed getting term config\n"); exit(1); }

	// Call posix function to set raw (can also be done by unsetting the OPOST bit for c_oflag)
	cfmakeraw( &config );

	return tcsetattr(fd, TCSANOW, &config);
}

int VixSerial::flush()
{
	// Flush... what exactly am I flushing here?
	return tcflush( fd, TCOFLUSH );
}

int VixSerial::write(unsigned char *data, int length)
{
//TODO: buffer data?
//TODO: 100th character pad byte for renards
//TODO: axe control characters
	return ::write(fd, data, length);
}

int VixSerial::setBaudPriv(int (*functor)(termios*, speed_t), int baud)
{
	int ret = 0;

	switch (baud)
	{
		case 0:
			ret = (functor(&config, B0) < 0);
			break;
		case 50:
			ret = (functor(&config, B50) < 0);
			break;
		case 75:
			ret = (functor(&config, B75) < 0);
			break;
		case 110:
			ret = (functor(&config, B110) < 0);
			break;
		case 134:
			ret = (functor(&config, B134) < 0);
			break;
		case 150:
			ret = (functor(&config, B150) < 0);
			break;
		case 200:
			ret = (functor(&config, B200) < 0);
			break;
		case 300:
			ret = (functor(&config, B300) < 0);
			break;
		case 600:
			ret = (functor(&config, B600) < 0);
			break;
		case 1200:
			ret = (functor(&config, B1200) < 0);
			break;
		case 1800:
			ret = (functor(&config, B1800) < 0);
			break;
		case 2400:
			ret = (functor(&config, B2400) < 0);
			break;
		case 4800:
			ret = (functor(&config, B4800) < 0);
			break;
		case 9600:
			ret = (functor(&config, B9600) < 0);
			break;
		case 19200:
			ret = (functor(&config, B19200) < 0);
			break;
		case 38400:
			ret = (functor(&config, B38400) < 0);
			break;
		case 57600:
			ret = (functor(&config, B57600) < 0);
			break;
		case 115200:
			ret = (functor(&config, B115200) < 0);
			break;
		default:
			ret = -1;//TODO:EINVAL?
	}

	if ( ret == 0 )
		this->baud = baud;

	return ret;
}

int VixSerial::configurationLookup(const char *configuration)
{
    if( strcmp(configuration, "8N1") == 0 )
        return CONFIG_8N1;
    else if( strcmp(configuration, "7E1") == 0 )
        return CONFIG_7E1;
    else if( strcmp(configuration, "7O1") == 0 )
        return CONFIG_7O1;
    else if( strcmp(configuration, "7S1") == 0 )
        return CONFIG_7S1;
    else
        return CONFIG_NONE;
}

