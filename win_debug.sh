#!/bin/sh

cmake --build ./build/win/debug

if [ $? -ne 0 ]; then
	exit
fi

pushd build/win/debug >/dev/null
wine ./pixbox.exe
popd >/dev/null
