#!/bin/bash

#  Incorporate CutiePie into SpecTcl's build.
#
# Usage: qtpy-incorp.sh tag
# Where:
#    tag is the tag to incorporate.
#
#  last tag was v1.5-002

REPOSITORY=https://github.com/FRIBDAQ/CutiePie.git
TARGET=PyQtGUI
VERSION=$1

if [[ ! $VERSION ]]
then
  VERSION="v1.5-002"
fi

rm -rf ${TARGET}
git clone ${REPOSITORY} ${TARGET}
(cd ${TARGET}; git checkout ${VERSION})
(cd ${TARGET}/main; autoreconf -if)
echo CutiePie ${VERSION} ready to build with SpecTcl

