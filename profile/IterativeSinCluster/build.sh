#!/bin/bash
#
# Requires [libsndfile](http://www.mega-nerd.com/libsndfile/).
#

g++ -std=c++17 -O3 -Wall -lsndfile -DTEST_BUILD -o master\
  ../../IterativeSinCluster/dsp/dspcore.cpp \
  ../../IterativeSinCluster/parameter.cpp \
  main.cpp
echo "Running ./master"
./master
