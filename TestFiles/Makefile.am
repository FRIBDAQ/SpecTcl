INCLUDES= -I. -I../EventSource \
	@LIBTCLPLUS_CFLAGS@	\
	-I../Utility @TCL_CPPFLAGS@ -I../Tape

LDADD = -lEventSource  -ltape \
	 -L. -L../Events -L../EventSource  \
	-L../Tape \
	-L../Filter -L../SpecTcl \
	@LIBTCLPLUS_LDFLAGS@ @TCL_LIBS@



#AM_LDFLAGS = -L. -L../Events -L../EventSource -L../TCL -L../NSCLException \
#	-L../Tape \
#	-L../Filter -L../SpecTcl -Wl,"-rpath=$(libdir)"


bin_PROGRAMS = testfile ltwrite

testfile_SOURCES = testfile.cpp



ltwrite_SOURCES = ltwrite.cpp

testfile_LDFLAGS=-Wl,"-rpath=$(libdir)"
ltwrite_LDFLAGS=-Wl,"-rpath=$(libdir)"
