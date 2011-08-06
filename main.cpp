#include <fcntl.h>

#include "serial.h"
#include "vixen.h"


int main(int argc, char **argv)
{
	VixSerial *serial = new VixSerial("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY);
	serial->setBaud();
	serial->configure();
	serial->softFlowControl();
	serial->makeRaw();
	serial->flush();

	VixenSequenceFile *file = new VixenSequenceFile("test.vix");

//	file->debug();
	file->serializeData(serial);
}
