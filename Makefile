CXX=$(CROSS_COMPILE)g++
AR=$(CROSS_COMPILE)ar

all:
	$(CXX) -c serial.cpp
	$(CXX) -c vixen.cpp -Itinyxml -lresolv
	$(CXX) -c tinyxml/tinyxml.cpp
	$(CXX) -c tinyxml/tinyxmlerror.cpp
	$(CXX) -c tinyxml/tinystr.cpp
	$(CXX) -c tinyxml/tinyxmlparser.cpp
	$(CXX) main.cpp -Itinyxml vixen.o serial.o tinyxml.o tinyxmlerror.o tinystr.o tinyxmlparser.o -o bahchumbug -lresolv

clean:
	rm -f serial.o vixen.o tinyxml.o tinyxmlerror.o tinystr.o tinyxmlparser.o bahchumbug
