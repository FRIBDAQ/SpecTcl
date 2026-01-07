lib_LTLIBRARIES=libMPIUtilities.la libMPITclPackage.la
libMPIUtilities_la_CPPFLAGS=@LIBTCLPLUS_CFLAGS@ @TCL_CPPFLAGS@ @MPI_FLAGS@
libMPIUtilities_la_LDFLAGS=@LIBTCLPLUS_LDFLAGS@ @TCL_LIBS@ @LIBEXCEPTION_LDFLAGS@ @MPI_FLAGS@

libMPIUtilities_la_SOURCES= TclPump.cpp TclPump.h MPITclCommand.cpp MPITclCommandAll.cpp \
    MPITclPackagedCommand.cpp MPITclPackagedCommandAll.cpp

include_HEADERS = MPITclCommand.h MPITclCommandAll.h MPITclPackagedCommand.h \
    MPITclPackagedCommandAll.h TclPump.h


libMPITclPackage_la_SOURCES= mpiTclPackage.cpp TclPump.h\
    MPISendCommand.cpp MPISendCommand.h
    
libMPITclPackage_la_CPPFLAGS=@LIBTCLPLUS_CFLAGS@ @TCL_CPPFLAGS@ @ROOT_CFLAGS@ @MPI_FLAGS@ \
    -I@top_srcdir@/Core -I@top_srcdir@/Utility
libMPITclPackage_la_LDFLAGS=@LIBTCLPLUS_LDFLAGS@ @TCL_LIBS@ @LIBEXCEPTION_LDFLAGS@ @MPI_FLAGS@
libMPITclPackage_la_LIBADD=libMPIUtilities.la


check_PROGRAMS= testApp



testApp_SOURCES= testApp.cpp TclPump.cpp MPITclCommand.cpp MPITclCommandAll.cpp
testApp_CPPFLAGS=$(libMPIUtilities_la_CPPFLAGS)
testApp_LDFLAGS= $(libMPIUtilities_la_LDFLAGS)
