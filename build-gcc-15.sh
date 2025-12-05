#!/bin/bash

set -e
set -x

apt-get update && apt-get upgrade -y
apt-get install -y sudo git build-essential git make gawk flex bison libgmp-dev libmpfr-dev libmpc-dev python3 binutils perl libisl-dev libzstd-dev tar gzip bzip2

export CONFIG_SHELL=/bin/bash

git clone https://gcc.gnu.org/git/gcc.git

cd gcc
git checkout releases/gcc-15.1.0
./contrib/download_prerequisites
mkdir build
cd build

../configure --prefix=/opt/gcc-15 --disable-multilib --enable-languages=c,c++
make -j$(nproc)
sudo make install

udo update-alternatives --install /usr/bin/gcc gcc /opt/gcc-15/bin/gcc 100
sudo update-alternatives --install /usr/bin/g++ g++ /opt/gcc-15/bin/g++ 100

g++ --version
gcc --version