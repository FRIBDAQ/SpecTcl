#!/bin/bash

##
#  Shell script to do an export of a specific tag of the libtcl
#  Usage:
#    tcl++incorp.sh tag 
#
#  The export is done to the libtcl directory.
#
#
#  The idea is that libtcl/libexception could be or not be centrally
#  installed, but if not, built from the export directory.
#
# Last used with libtclplus-v2.0-000

baseURL="http://git.code.sf.net/p/nscldaq/git"

tag="$1"

rm -rf libtcl 

git clone $baseURL libtcl
(cd libtcl; git checkout tags/$1)
(cd libtcl; autoreconf -i)
