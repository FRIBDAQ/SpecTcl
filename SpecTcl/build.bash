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
