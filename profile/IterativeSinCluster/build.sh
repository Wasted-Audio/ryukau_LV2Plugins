#!/bin/bash
#
# Requires [libsndfile](http://www.mega-nerd.com/libsndfile/).
#

g++ -std=c++17 -O3 -mavx2 -mfma -Wall -lsndfile -DTEST_BUILD -o withavx2\
  ../../IterativeSinCluster/dsp/dspcore.cpp \
  ../../IterativeSinCluster/parameter.cpp \
  main.cpp
echo "With AVX2"
./withavx2

g++ -std=c++17 -O3 -Wall -lsndfile -DTEST_BUILD -o withoutavx2\
  ../../IterativeSinCluster/dsp/dspcore.cpp \
  ../../IterativeSinCluster/parameter.cpp \
  main.cpp
echo "Without AVX2"
./withoutavx2
