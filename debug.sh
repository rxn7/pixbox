#!/bin/sh

cmake --build build/debug

if [ $? -ne 0 ]; then
	exit
fi

pushd build/debug >/dev/null
./pixbox
popd >/dev/null
