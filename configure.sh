#!/bin/sh

cmake -Bbuild/debug -GNinja -DCMAKE_BUILD_TYPE=Debug .
cmake -Bbuild/release -GNinja -DCMAKE_BUILD_TYPE=Release .
