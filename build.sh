#!/bin/bash

mkdir build-standalone
g++ -std=c++23 -fPIE -O0 -g -Wall -Werror -Wno-error=unused-parameter -Wextra -pedantic -pthread -I $(pwd)/cxpm-base/src -I $(pwd)/cxpm/src -o build-standalone/cxpm cxpm/src/cxpm.cpp
g++ -std=c++23 -fPIC -shared -O0 -g -Wall -Werror -Wno-error=unused-parameter -Wextra -pedantic -pthread -I $(pwd)/cxpm-base/src -I $(pwd)/cxpm/src -o build-standalone/cxpm-toolchain-g++.so toolchains/g++/src/toolchain.cpp
g++ -std=c++23 -fPIC -shared -O0 -g -Wall -Werror -Wno-error=unused-parameter -Wextra -pedantic -pthread -I $(pwd)/cxpm-base/src -I $(pwd)/cxpm/src -o build-standalone/cxpm-toolchain-gcc.so toolchains/gcc/src/toolchain.cpp