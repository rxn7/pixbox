#!/bin/sh

cmake --build build/debug
pushd build/debug >/dev/null
./pixbox
popd >/dev/null
