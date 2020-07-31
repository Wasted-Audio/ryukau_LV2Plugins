#!/bin/bash
#
# Build script for GitHub Actions.
#

set -e

# Without update, some package will be 404.
sudo apt-get update
sudo apt-get install git pkg-config libjack-jackd2-dev libgl-dev liblo-dev

make

mkdir test
make install DESTDIR=test
make installHome
make installHomeVST2
