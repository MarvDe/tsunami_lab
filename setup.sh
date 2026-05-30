#!/bin/bash

echo "builing submodules"

git submodule update --init

cd submodules/yaml-cpp
mkdir build
cd build
cmake ..
cmake --build .
cd ../../..
scons