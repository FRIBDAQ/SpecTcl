#!/bin/csh

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

# Setup the operating system dependent commands:

# C compiler:

setenv  LinuxCC "gcc -g"
setenv  OSF1CC  "gcc -g"
setenv  HPUXCC  "gcc -g"
setenv  DarwinCC "cc -g -Dunix"
setenv  CYGWINCC "gcc -g"

# C++ compiler

setenv LinuxCPP "g++ -g"
setenv OSF1CPP  "g++ -g"
setenv HPUXCPP   "g++ -g"
setenv DarwinCPP "c++ -g -Dunix"
setenv CYGWINCPP "g++ -g -Dunix"

# Who does the LD?

setenv LinuxLD  "g++ -L/usr/X11/lib"
setenv OSF1LD   "g++ -L/usr/X11/lib"
setenv HPUXLD   "g++ -L/usr/X11/lib"
setenv DarwinLD "c++ -undefined warning -L/usr/X11R6/lib"
setenv CYGWINLD "g++ -L/usr/X11/lib -L/usr/local/tcl/lib"

# Where are the Motif includes?

setenv LinuxMOTIFINCLUDES "-I/usr/X11/include"
setenv OSF1MOTIFINCLUDES "-I/usr/X11/include"
setenv HPUXMOTIFINCLUDES "-I/usr/X11/include"
setenv DarwinMOTIFINCLUDES "-I/usr/X11R6/include"
setenv CYGWINMOTIFINCLUDES "-I/usr/X11/include"

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

set instdir=`(cd $instdir;pwd)`      # this trick allows relative paths.

echo "Installing in" $instdir

make clean
make depend
make -k OS=$OS INSTDIR=$instdir PROFILE=-g
