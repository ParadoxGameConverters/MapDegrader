#!/bin/bash

export CC=/usr/bin/gcc-9 &&
export CXX=/usr/bin/g++-9 &&

cd imageMagick &&
tar xvf im7.10-linux-source.tar &&
cd ImageMagick-7.0.10 &&
./configure --with-quantum-depth=8 --enable-hdri=no --with-x=no --with-utilities=no &&
sudo make install &&
cd ../../ &&
rm -rf Release/ &&
cd MapDegrader &&
rm -rf build &&
rm -rf Release-Linux &&
cmake -H. -Bbuild &&
cmake --build build -- -j3 &&
mv Release-Linux ../Release &&
cd .. &&
tar -cjf MapDegrader.tar.bz2 Release
