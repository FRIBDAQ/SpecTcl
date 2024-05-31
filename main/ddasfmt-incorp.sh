#!/bin/bash

## Incorporate the DDAS format library:

# Most recently used version: 1.0

REPOSITORY="https://github.com/FRIBDAQ/DDASFormat.git"
TAG=$1

if [[ ! $TAG ]]
then
  TAG=`cat DDASFORMAT_VERSION`
fi

rm -rf ddasformat
git clone $REPOSITORY ddasformat
(cd ddasformat; git checkout $TAG)
