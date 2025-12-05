#!/bin/bash

set -e
set -x

./build.sh

mkdir -p /usr/local/bin
mkdir -p /usr/local/lib/cxpm/toolchains/cxpm-toolchain-g++-default
mkdir -p /usr/local/lib/cxpm/toolchains/cxpm-toolchain-gcc-default
mkdir -p /usr/local/include/cxpm-base

cp -rf build-standalone/cxpm /usr/local/bin
cp -rf build-standalone/cxpm-toolchain-g++.so /usr/local/lib/cxpm/toolchains/cxpm-toolchain-g++-default
cp -rf build-standalone/cxpm-toolchain-gcc.so /usr/local/lib/cxpm/toolchains/cxpm-toolchain-gcc-default
cp -rf cxpm-base/src/* /usr/local/include/cxpm-base
