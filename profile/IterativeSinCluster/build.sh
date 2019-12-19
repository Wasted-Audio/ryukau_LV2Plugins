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

compile_simd ../../IterativeSinCluster/dsp/dspcore.cpp

echo Compiling main.cpp

# If CPU doesn't support AVX512, changing order of *.o file cause SIGILL (illegal instruction).
# See: https://stackoverflow.com/questions/15406658/cpu-dispatcher-for-visual-studio-for-avx-and-sse
g++ -std=c++17 -O3 -Wall -lsndfile -DTEST_BUILD -o instrset2 \
  ../../lib/vcl/instrset_detect.cpp \
  ./dspcore.cpp.sse2.o \
  ./dspcore.cpp.sse41.o \
  ./dspcore.cpp.avx2.o \
  ./dspcore.cpp.avx512.o \
  ../../IterativeSinCluster/parameter.cpp \
  main.cpp

echo Running benchmark
./instrset2
