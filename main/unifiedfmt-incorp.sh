#!/bin/bash

##  Incorporate the unfied formatting library:

# Most recently used version 1.1

REPOSITORY="https://git.frib.msu.edu/daqdev/unifiedformat.git"
TAG=$1

rm -rf unifiedformat
git clone $REPOSITORY
(cd unifiedformat; git checkout $TAG)
 
