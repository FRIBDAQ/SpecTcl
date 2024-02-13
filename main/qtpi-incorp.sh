#!/bin/bash

#  Incorporate CutiePie into SpecTcl's build.
#
# Usage: qtpi-incorp.sh tag
# Where:
#    tag is the tag to incorporate.
#

REPOSITORY=https://github.com/FRIBDAQ/CutiePie.git
TARGET=PyQtGUI
VERSION=$1

rm -rf ${TARGET}
git clone ${REPOSITORY} ${TARGET}
(cd ${TARGET}; git checkout ${VERSION})
(cd ${TARGET}/main; autoreconf -if)

echo Cutie PI ${VERSION} ready to build with SpecTcl

