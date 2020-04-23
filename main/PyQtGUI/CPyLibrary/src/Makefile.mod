CC    = g++
CFLAGS       = -std=c++11 -c -g -fPIC -I/usr/include/python3.7
LDFLAGS      = -shared -Wl,-soname
LDLIBS=-L/usr/local/lib/python3.7 -lpython3.7m -Wl,-rpath,/usr/local/lib/python3.7

# c++ classes
NAME    = CPyConverter
TARGET  = lib$(NAME).so
SOURCES = $(wildcard $(NAME).cpp) dataRetriever.cpp dataAccess.cpp
HEADERS = $(wildcard $(NAME).h) dataRetriever.h dataAccess.h dataType.h
OBJECTS = $(SOURCES:.cpp=.o)
	   
all: $(TARGET) config

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS),lib$(NAME).so -g -export-dynamic -o $(TARGET) $(OBJECTS) $(LDLIBS)

$(OBJECTS): $(SOURCES)
	$(CC) $(SOURCES) $(CFLAGS)
	
config:
	python3 configure.py
	$(MAKE)

.PHONY: clean

clean:
	if [ -f $(TARGET) ]; then rm $(TARGET) $(OBJECTS) && $(MAKE) clean && rm Makefile sip* *.sbf; fi;
