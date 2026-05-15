#!/bin/bash

echo "builing submodules"

cd submodules/yaml-cpp
mkdir build
cd build
cmake ..
cmake --build .
cd ../../..
scons