#!/bin/bash
set -e
./raspberry-dev/init.sh
cmake . -DCMAKE_TOOLCHAIN_FILE=./raspberry-dev/toolchain.cmake
make