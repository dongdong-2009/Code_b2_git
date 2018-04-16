#!/bin/sh


rm -rf ./lib
rm -rf ./libs
mkdir ./lib
mkdir ./libs
mkdir ./lib/winnt
mkdir ./lib/SOLARIS_5.10_i86pc
mkdir ./lib/SOLARIS_5.10_sun4

./configure --with-oracle-headers-path=/u03/tabuild2/cots/oracle/10.2.0/SOLARIS_5.10_i86pc/inc


make


rm -rf ./lib/SOLARIS_5.10_i86pc

cp -rf ./src/.libs ./libs

cp -rf ./src/.libs ./lib/SOLARIS_5.10_i86pc

cd ./libs
rm -f *.o
cd ..

cd ./lib/SOLARIS_5.10_i86pc
rm -f *.o
cd ../../

pwd
