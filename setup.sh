#!/bin/bash

echo "builing submodules"

cd submodules/netcdf-cxx4/
mkdir build
cd build
cmake ..
cmake --build .
cd ../../yaml-cpp
mkdir build
cd build
cmake ..
cmake --build .
cd ../../..
scons