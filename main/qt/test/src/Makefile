CPPFLAGS=-pthread -I/usr/include/python3.4 -I../CPyLibrary
LDFLAGS=-std=c++11
LDLIBS=-L/usr/local/lib/python3.4 -lpython3.4m -Wl,-rpath,/usr/local/lib/python3.4
TARGET=GUIapp

all: $(TARGET)

$(TARGET): gui.o 
	LD_LIBRARY_PATH=`pwd` g++ $(LDFLAGS) -o $(TARGET) gui.o $(LDLIBS)

gui.o: gui.cpp CPyHelper.h
	g++ $(LDFLAGS) $(CPPFLAGS) -c gui.cpp

.PHONY: clean

clean:
	rm -rf gui.o $(TARGET) 
