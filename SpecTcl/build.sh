#!/bin/csh
umask 02
setenv OS `uname | sed s/-//`       # the sed is needed for HP*-*UX
rm -rf SpecTcl/SpecTcl              # Due to bungled initial CVS load.
rm -rf SpecTcl/Exception
rm -rf SpecTcl/doc
rm -rf SpecTcl/Display
rm -rf SpecTcl/EventSource
rm -rf SpecTcl/Events
rm -rf SpecTcl/Gates
rm -rf SpecTcl/Scripts
rm -rf SpecTcl/SpectrumIO
rm -rf SpecTcl/TCL
rm -rf SpecTcl/Tape
rm -rf SpecTcl/TestFiles
rm -rf SpecTcl/Utility
rm -rf SpecTcl/Xamine

source sysdefs.sh

if ( $#argv ==  1 ) then
   set instdir = $1
else if ( $#argv == 0 ) then
   echo -n "Full path to installation directory: "
   set instdir = $<
else
   echo "Usage: "
   echo "    build.sh  [installation_directory_full_path]"
   exit
endif


echo building and installing in $instdir 

make clean
make depend
make -k OS=$OS INSTDIR=$instdir PROFILE=-g

