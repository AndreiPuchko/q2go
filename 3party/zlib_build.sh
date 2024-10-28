#!/bin/bash
cmake=/usr/bin/cmake
cd ./zlib-1.3.1

if ! test  -d  build; then
	mkdir build
fi

cd build

${cmake} -S .. -B . -DCMAKE_INSTALL_PREFIX="../../zlib.install"
${cmake} --build . --config Release
${cmake} --install . --config Release

cd ../..
