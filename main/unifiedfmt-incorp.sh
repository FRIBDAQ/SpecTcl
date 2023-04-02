#!/bin/bash

##  Incorporate the unfied formatting library:

# Most recently used version 1.1-005

REPOSITORY="git@github.com:FRIBDAQ/unifiedformat.git"
TAG=$1

rm -rf unifiedformat
git clone $REPOSITORY
(cd unifiedformat; git checkout $TAG)
 
