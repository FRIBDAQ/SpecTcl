#!/bin/bash
#
#  Needed for Cygwin builds where csh doesn't exist.
#

export PATH=$PATH:/usr/X11R6/bin    # makedepend can be here.
export OS=`uname | sed s/-//`       # the sed is needed for HP*-*UX

# CYGWin will now look like: CYGWIN_winveresion.
# We want to get CYGWIN out of it. 
case $OS in 
  CYGWIN*)
    export OS=CYGWIN;;
esac

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

# Setup operating system dependent variables:

#
# C compiler:

export  LinuxCC="gcc -g"
export  OSF1CC="gcc -g"
export  HPUXCC="gcc -g"
export  DarwinCC="cc -g -Dunix"
export  CYGWINCC="gcc -g"

# C++ compiler

export LinuxCPP="g++ -g"
export OSF1CPP="g++ -g"
export HPUXCPP="g++ -g"
export DarwinCPP="c++ -g -Dunix"
export CYGWINCPP="g++ -g -Dunix"

# Who does the LD?

export LinuxLD="g++ -L/usr/X11/lib"
export OSF1LD="g++ -L/usr/X11/lib"
export HPUXLD="g++ -L/usr/X11/lib"
export DarwinLD="c++ -undefined warning -L/usr/X11R6/lib"
export CYGWINLD="g++ -L/usr/X11R6/lib -L/usr/local/tcl/lib"

# Where are the Motif includes?

export LinuxMOTIFINCLUDES="-I/usr/X11/include"
export OSF1MOTIFINCLUDES="-I/usr/X11/include"
export HPUXMOTIFINCLUDES="-I/usr/X11/include"
export DarwinMOTIFINCLUDES="-I/usr/X11R6/include"
export CYGWINMOTIFINCLUDES="-I/usr/X11R6/include"




#

case $# in
   0)
     echo -n "Installation dir: "
     read instdir;;
   1)
     instdir=$1;;
   *)
    echo "Usage:"
    echo "  build.bash [installation_directory]"
    exit;;
esac

instdir=`(cd $instdir;pwd)`      # this trick allows relative paths.

echo "Installing in" $instdir

make clean
make depend
make -k OS=$OS INSTDIR=$instdir PROFILE=-g
