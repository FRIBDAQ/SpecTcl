CC    = g++
CFLAGS       = -std=c++11 -c -g -fPIC -I/usr/include/python3.4
LDFLAGS      = -shared -Wl,-soname
LDLIBS=-L/usr/local/lib/python3.4 -lpython3.4m -Wl,-rpath,/usr/local/lib/python3.4

# c++ classes
NAME    = CPyConverter
TARGET  = lib$(NAME).so
SOURCES = $(wildcard $(NAME).cpp)
HEADERS = $(wildcard $(NAME).h)
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
	rm $(TARGET) $(OBJECTS) && $(MAKE) clean && rm Makefile sip* *.sbf

