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

make -k -f Makefile.quick OS=$OS INSTDIR=$instdir PROFILE=-g