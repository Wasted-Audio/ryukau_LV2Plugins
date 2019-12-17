#!/bin/bash
#
# Requires [libsndfile](http://www.mega-nerd.com/libsndfile/).
#

function compile_simd() {
  echo Compiling "$1"
  local base
  base=$(basename "$1")
  g++ -DTEST_BUILD -O3 -fPIC -mavx512f -mfma -mavx512vl -mavx512bw -mavx512dq -std=c++17 -c "$1" -o"$base.avx512.o"
  g++ -DTEST_BUILD -O3 -fPIC -mavx2 -mfma -std=c++17 -c "$1" -o"$base.avx2.o"
  g++ -DTEST_BUILD -O3 -fPIC -msse4.1 -std=c++17 -c "$1" -o"$base.sse41.o"
  g++ -DTEST_BUILD -O3 -fPIC -msse2 -std=c++17 -c "$1" -o"$base.sse2.o"
}

compile_simd ../../IterativeSinCluster/dsp/oscillator.cpp
compile_simd ../../IterativeSinCluster/dsp/note.cpp

echo Compiling main.cpp
g++ -std=c++17 -O3 -Wall -lsndfile -DTEST_BUILD -o instrset \
  ../../lib/vcl/instrset_detect.cpp \
  ./*.o \
  ../../IterativeSinCluster/dsp/dspcore.cpp \
  ../../IterativeSinCluster/parameter.cpp \
  main.cpp

echo Running benchmark
./instrset
