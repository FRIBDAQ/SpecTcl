#!/bin/bash

##  Incorporate the DDAS format library:

# Most recently used version: 1.0-005

REPOSITORY="https://github.com/FRIBDAQ/DDASFormat.git"
TAG=$1
TARGET="ddasformat"

if [[ ! $TAG ]]
then
  TAG="1.0-005"
fi

rm -rf $TARGET
git clone $REPOSITORY $TARGET
(cd $TARGET; git checkout $TAG)
