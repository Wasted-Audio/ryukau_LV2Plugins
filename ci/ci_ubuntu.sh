#!/bin/bash
#
# Build script for GitHub Actions.
#

set -e

# Without update, some package will be 404.
sudo apt-get update
sudo apt-get install git pkg-config libjack-jackd2-dev libgl-dev liblo-dev

# Temporary patch to DPF. See: https://github.com/DISTRHO/DPF/issues/216
cp patch/NanoVG.cpp lib/DPF/dgl/src/NanoVG.cpp

make
make install
