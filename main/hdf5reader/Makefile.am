lib_LTLIBRARIES=libhdfReader.la

libhdfReader_la_SOURCES=hdf5SpectrumReader.cpp

INCLUDE_FILES=hdfSpectrumReader.h

libhdfReader_la_CPPFLAGS=@HDF5_CFLAGS@
libhdfReader_la_LDFLAGS=@HDF5_CXXLIBS@


noinst_PROGRAMS = listsample

listsample_SOURCES=listsample.cpp
listsample_LDFLAGS=libhdfReader.la
listsample_CPPFLAGS=@HDF5_CFLAGS@
listsample_DEPENDENCIES=libhdfReader.la

