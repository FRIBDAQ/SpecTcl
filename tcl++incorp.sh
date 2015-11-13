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
# Last used with libtclplus-v2.0-002

baseURL="https://svn.code.sf.net/p/nscldaq/code/tags"

tag="$1"

URL="$baseURL/$tag"

echo $URL

svn export $URL libtcl

(cd libtcl; autoreconf -i)

