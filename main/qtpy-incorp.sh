#!/bin/bash

#  Incorporate CutiePie into SpecTcl's build.
#
# Usage: qtpy-incorp.sh tag
# Where:
#    tag is the tag to incorporate.
#
#  last tag was v1.5-005

REPOSITORY=https://github.com/FRIBDAQ/CutiePie.git
TARGET=PyQtGUI
VERSION=$1

rm -rf ${TARGET}
git clone ${REPOSITORY} ${TARGET}

if [[ ! $VERSION ]]
then
  VERSION=$(cd ${TARGET}; git ls-remote --tags --sort=committerdate | grep -o 'v.*' | sort -r | head -1)
fi

(cd ${TARGET}; git checkout ${VERSION})
(cd ${TARGET}/main; autoreconf -if)
echo CutiePie ${VERSION} ready to build with SpecTcl
