#!/bin/bash
rm -r build_brew
mkdir build_brew
cd build_brew
git clone --recursive git@github.com:ovilab/atomify
cd atomify
rm -rf .git
cd src/examples
rm -rf .git
cd ../../..
tar -czvf atomify-2.1.0-src.tar.gz atomify
cp atomify-2.1.0-src.tar.gz ..