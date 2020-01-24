CC=g++
CPPFLAGS=-std=c++11 -g -pthread -I/usr/include/python3.4
LDLIBS=-L/usr/local/lib/python3.4 -lpython3.4m -Wl,-rpath,/usr/local/lib/python3.4
TARGET=test_shm

DEPS = dataRetriever.h dataAccess.h
OBJ = test.o dataRetriever.o dataAccess.o

%.o: %.c $(DEPS)
	$(CC) -c  -o $@ $< $(CPPFLAGS) 

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(CPPFLAGS) $(LDLIBS)

clean:
	rm -rf *.o $(TARGET)
