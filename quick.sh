#!/bin/csh

setenv OS `uname | sed s/-//`       # the sed is needed for HP*-*UX

# C compiler:

setenv  LinuxCC "gcc -g"
setenv  OSF1CC  "gcc -g"
setenv  HPUXCC  "gcc -g"
setenv  DarwinCC "cc -g -Dunix"

# C++ compiler

setenv LinuxCPP "g++ -g"
setenv OSF1CPP  "g++ -g"
setenv HPUXCPP   "g++ -g"
setenv DarwinCPP "c++ -g -Dunix"
setenv CYGWINCC "gcc -g"

# Who does the LD?

setenv LinuxLD  "g++ -L/usr/X11/lib"
setenv OSF1LD   "g++ -L/usr/X11/lib"
setenv HPUXLD   "g++ -L/usr/X11/lib"
setenv DarwinLD "c++ -undefined warning -L/usr/X11R6/lib"
setenv CYGWINLD "g++ -L/usr/X11R6/lib -L/usr/local/tcl/lib"

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

rm -f SpecTcl/SpecTcl                # force relink.
make -k -f Makefile.quick OS=$OS INSTDIR=$instdir PROFILE=-g
