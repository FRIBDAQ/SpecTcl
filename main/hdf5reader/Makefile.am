lib_LTLIBRARIES=libhdfReader.la

libhdfReader_la_SOURCES=hdf5SpectrumReader.cpp

include_HEADERS=hdf5spectrumReader.h

libhdfReader_la_CPPFLAGS=@HDF5_CFLAGS@
libhdfReader_la_LDFLAGS=@HDF5_CXXLIBS@


noinst_PROGRAMS = listsample

listsample_SOURCES=listsample.cpp
listsample_LDFLAGS=libhdfReader.la
listsample_CPPFLAGS=@HDF5_CFLAGS@
listsample_DEPENDENCIES=libhdfReader.la

#  Build the docs right into the destination directory.
#  put the example and sample file where it goes too:
install-exec-local:
	$(mkinstalldirs) @datarootdir@/hdf5
	$(mkinstalldirs) @datarootdir@/hdf5example
	@HTMLDOCBOOK@ -o @datarootdir@/hdf5 @srcdir@/hdf5.xml
	$(INSTALL_DATA) @srcdir@/listsample.cpp @datarootdir@/hdf5example
	$(INSTALL_DATA) @srcdir@/sample.hdf @datarootdir@/hdf5example

EXTRA_DIST=sample.hdf hdf5.xml
