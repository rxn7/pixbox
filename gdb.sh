#!/bin/sh

pushd build/debug >/dev/null
gdb ./pixbox
popd >/dev/null
