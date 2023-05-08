#!/bin/bash

##  Incorporate the unfied formatting library:

# Most recently used version 1.2

REPOSITORY="https://github.com/FRIBDAQ/UnifiedFormat.git"
TAG=$1

rm -rf unifiedformat
git clone $REPOSITORY unifiedformat
(cd unifiedformat; git checkout $TAG)
 
