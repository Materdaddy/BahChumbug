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
#include <fcntl.h>

#include "serial.h"
#include "vixen.h"
#include "audio.h"


int main(int argc, char **argv)
{
	VixSerial *serial = new VixSerial("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY);
	serial->setBaud();
	serial->configure();
	serial->softFlowControl();
	serial->makeRaw();
	serial->flush();

	VixenSequenceFile *file = new VixenSequenceFile("test.vix");

	VixSound *snd = new VixSound(file->getMusicFilename());
	snd->initMpg123();
	snd->initAlsa();

//	file->debug();
	snd->threadAudio();
	file->serializeData(serial);
	snd->waitAudio();

	delete snd;
	delete file;
	delete serial;
}
