#!/bin/sh

pushd build/debug >/dev/null
gdb ./pixbox -q
popd >/dev/null
