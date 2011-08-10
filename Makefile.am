SUBDIRS=tinyxml

AM_CPPFLAGS = -I$(srcdir)/tinyxml
AM_LDFLAGS = -L$(srcdir)/tinyxml

bin_PROGRAMS=bahchumbug
bahchumbug_SOURCES=main.cpp serial.cpp serial.h vixen.cpp vixen.h
bahchumbug_LDADD = -ltinyxml
