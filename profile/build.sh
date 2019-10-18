#!/bin/bash
#
# Requires [libsndfile](http://www.mega-nerd.com/libsndfile/).
#

g++ -g -Og -fno-inline -lsndfile -DTEST_BUILD \
  ../SyncSawSynth/dsp/dspcore.cpp \
  ../SyncSawSynth/parameter.cpp \
  main.cpp

valgrind --read-inline-info=yes --tool=callgrind ./a.out
