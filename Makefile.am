SUBDIRS=tinyxml

AM_CPPFLAGS = -Itinyxml
AM_LDFLAGS = -Ltinyxml

bin_PROGRAMS=bahchumbug
bahchumbug_SOURCES=main.cpp serial.cpp serial.h vixen.cpp vixen.h audio.cpp audio.h
bahchumbug_LDADD = -ltinyxml

BUILT_SOURCES = tinyxml.h tinystr.h
tinyxml.h: Makefile
	-cp $(srcdir)/tinyxml/tinyxml.h .
tinystr.h: Makefile
	-cp $(srcdir)/tinyxml/tinystr.h .
CLEANFILES=tinyxml.h tinystr.h
