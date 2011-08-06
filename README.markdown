BahChumbug
==========

Source Code Page: https://github.com/Materdaddy/BahChumbug

Home Page: http://christmasonquiethills.com/bahchumbug/

Overview
--------
Use something more light weight to play Vixen sequence files back than a laptop
capable of running .NET software required for Vixen.  The plan is to have this
always working on Ubuntu and Chumby.  It is likely to work on most flavors of
linux, not just Ubuntu and the custom rolled Chumby.

Requirements
------------
* GNU Compiler Collection (gcc) - To compile the code
* GNU Make - To build the code
* TinyXml - To parse vixen's xml
* libresolv - For the base64 decode of event data

Installation
------------
Currently the installation is simply a compile, no autoconf is used, the GNU
makefile is static and hard-coded, so simply running "make" should produce a
binary called 'bahchumbug' that can be run.  Currently it doesn't do any option
parsing and will simply look for a file called "text.vix" in the current
directory and will only attempt to open/use /dev/ttyUSB0.  These are among the
many things that still need to be coded up to work.

Implementation
--------------
Implementation details will go here if things ever get complicated, but for
now, simply look at the code, there's not a lot of it, and it should all be
simple and commented.

Legal
-----
Copyright 2011 Mathew Mrosko

BahChumbug is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License v2 as published
by the Free Software Foundation.

BahChumbug is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BahChumbug.  If not, see <http://www.gnu.org/licenses/>.
