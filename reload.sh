#!/usr/bin/env bash

cd build/
make -j$(nproc)
cd ..
sudo bash extract.sh
sudo bash inject.sh
