#!/bin/bash

CURR_PATH=$( cd "$(dirname "$BASH_SOURCE[0]}" )" && pwd )

#Break on error
set -e

mkdir -p  ${CURR_PATH}/build
mkdir -p  ${CURR_PATH}/installed
mkdir -p  ${CURR_PATH}/installed/include/rapidxml
cp -f ${CURR_PATH}/dependencies/rapidxml-1.13/*.hpp ${CURR_PATH}/installed/include/rapidxml
cp -f ${CURR_PATH}/dependencies/stb/*.h ${CURR_PATH}/installed/include/


#glm
mkdir -p  ${CURR_PATH}/build/glm && pushd ${CURR_PATH}/build/glm
cmake  ${CURR_PATH}/dependencies/glm -DCMAKE_INSTALL_PREFIX=${CURR_PATH}/installed && make -j2 && make install && popd

#assimp
mkdir -p  ${CURR_PATH}/build/assimp && pushd ${CURR_PATH}/build/assimp
cmake  ${CURR_PATH}/dependencies/assimp-5.3.1 -DCMAKE_CXX_FLAGS=-DASSIMP_BUILD_NO_XFILE_EXPORTER=1 -DASSIMP_BUILD_ASSIMP_TOOLS=off -DASSIMP_BUILD_TESTS=OFF -DASSIMP_BUILD_X_IMPORTER=off -DCMAKE_INSTALL_PREFIX=${CURR_PATH}/installed && make -j2 && make install && popd

#freetype
mkdir -p  ${CURR_PATH}/build/freetype && pushd ${CURR_PATH}/build/freetype
cmake  ${CURR_PATH}/dependencies/freetype-2.3.11  -DCMAKE_INSTALL_PREFIX=${CURR_PATH}/installed && make -j2 && make install && popd

#vrlib
cmake  ${CURR_PATH}  -DCMAKE_INSTALL_PREFIX=${CURR_PATH}/installed && make -j2 && make install
