TARGET = CPyConverter.so
OFILES = sipCPyConvertercmodule.o sipCPyConverterstdstring.o sipCPyConverterstdvector0100stdstring.o sipCPyConverterstdvector2100.o sipCPyConverterstdvector2000.o sipCPyConverterstdvector1900.o sipCPyConverterstdvector1800.o sipCPyConverterstdvector2200.o sipCPyConverterstdvector2400.o sipCPyConverterCPyConverter.o
HFILES = sipAPICPyConverter.h 

CC = gcc
CXX = g++
LINK = g++
CPPFLAGS = -DNDEBUG -I. -I/usr/include/python3.7m
CFLAGS =  -g -O2 -fdebug-prefix-map=/build/sip4-aFRT60/sip4-4.19.14+dfsg=. -fstack-protector-strong -Wformat -Werror=format-security  -Wdate-time -D_FORTIFY_SOURCE=2 -fPIC -Wall -W
CXXFLAGS = -std=c++11  -g -O2 -fdebug-prefix-map=/build/sip4-aFRT60/sip4-4.19.14+dfsg=. -fstack-protector-strong -Wformat -Werror=format-security  -Wdate-time -D_FORTIFY_SOURCE=2 -fPIC -Wall -W
LFLAGS = -L/usr/opt/spectcl/SpecTcl/lib -lCPyConverter -Wl,-rpath=/usr/opt/spectcl/SpecTcl/lib  -Wl,-z,relro -shared -Wl,--version-script=CPyConverter.exp
LIBS = -lCPyConverter
.SUFFIXES: .c .o .cpp .cc .cxx .C


.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) -o $@ $<

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) -o $@ $<

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) -o $@ $<

.C.o:
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) -o $@ $<

.c.o:
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -o $@ $<

all: $(TARGET)

$(OFILES): $(HFILES)

$(TARGET): $(OFILES)
	@echo '{ global: PyInit_CPyConverter; local: *; };' > CPyConverter.exp
	$(LINK) $(LFLAGS) -o $(TARGET) $(OFILES) $(LIBS)

install: $(TARGET)
	@test -d $(DESTDIR)/usr/lib/python3/dist-packages || mkdir -p $(DESTDIR)/usr/lib/python3/dist-packages
	cp -f $(TARGET) $(DESTDIR)/usr/lib/python3/dist-packages/$(TARGET)

clean:
	-rm -f $(TARGET)
	-rm -f sipCPyConvertercmodule.o
	-rm -f sipCPyConverterstdstring.o
	-rm -f sipCPyConverterstdvector0100stdstring.o
	-rm -f sipCPyConverterstdvector2100.o
	-rm -f sipCPyConverterstdvector2000.o
	-rm -f sipCPyConverterstdvector1900.o
	-rm -f sipCPyConverterstdvector1800.o
	-rm -f sipCPyConverterstdvector2200.o
	-rm -f sipCPyConverterstdvector2400.o
	-rm -f sipCPyConverterCPyConverter.o
	-rm -f CPyConverter.exp
