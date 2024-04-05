#!/bin/bash

export CC=/usr/bin/clang
export CXX=/usr/bin/clang++
export BuildOS=Linux

if [ ! -d "bin/" ]; then
    mkdir bin/
fi

pushd bin

cmake ../ -DCMAKE_BUILD_TYPE=Debug

make -j8

popd

printf 'Done.\n'