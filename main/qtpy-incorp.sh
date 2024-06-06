#!/bin/bash

#  Incorporate CutiePie into SpecTcl's build.
#
# Usage: qtpy-incorp.sh tag
# Where:
#    tag is the tag to incorporate.
#
#  last tag was v1.5

REPOSITORY=https://github.com/FRIBDAQ/CutiePie.git
TARGET=PyQtGUI
VERSION=$1

if [[ ! $VERSION ]]
then
  VERSION="v1.5"
fi

rm -rf ${TARGET}
git clone ${REPOSITORY} ${TARGET}
(cd ${TARGET}; git checkout ${VERSION})
(cd ${TARGET}/main; autoreconf -if)
(cd ${TARGET}/main; ./tcl++incorp libtclplus-v4.3-001)
echo CutiePie ${VERSION} ready to build with SpecTcl

