#!/bin/bash

##  Incorporate the unfied formatting library:

# Most recently used version 2.2-002
#  2.2 allows us to use the FragmentIndex and fragment.{h,cpp}
# in unified format rather than local maintenance headaches
#  See issue#166

REPOSITORY="https://github.com/FRIBDAQ/UnifiedFormat.git"
TAG=$1
TARGET="unifiedformat"

if [[ ! $TAG ]]
then
  TAG="2.2-002"
fi

rm -rf $TARGET
git clone $REPOSITORY $TARGET
(cd $TARGET; git checkout $TAG)
