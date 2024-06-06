#!/bin/bash

##  Incorporate the unfied formatting library:

# Most recently used version 1.2

REPOSITORY="https://github.com/FRIBDAQ/UnifiedFormat.git"
TAG=$1
TARGET="unifiedformat"

if [[ ! $TAG ]]
then
  TAG="1.2"
fi

rm -rf $TARGET
git clone $REPOSITORY $TARGET
(cd $TARGET; git checkout $TAG)
