#!/bin/csh

setenv OS `uname | sed s/-//`       # the sed is needed for HP*-*UX

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

set instdir=`(cd $instdir;pwd)`      # this trick allows relative paths.

echo "Installing in" $instdir

rm -f SpecTcl/SpecTcl                # force relink.
make -k -f Makefile.quick OS=$OS INSTDIR=$instdir PROFILE=-g
