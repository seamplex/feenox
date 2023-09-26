#!/bin/bash

if [ ! -d sparselizard ]; then
  git clone https://github.com/halbux/sparselizard/
else
  cd sparselizard
  git pull
  cd ..
fi


mkdir -p sparselizard/simulations/le10
cp le10.cpp sparselizard/simulations/le10
sed s/default/le10/ sparselizard/simulations/default/CMakeLists.txt > sparselizard/simulations/le10/CMakeLists.txt
sed -i s/default/le10/ sparselizard/simulations/CMakeLists.txt

mkdir -p sparselizard/build
cd sparselizard/build
cmake ..
make -j4
cd ../..
cp sparselizard/build/simulations/le10/le10 sparselizard-le10

