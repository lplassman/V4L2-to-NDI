#!/usr/bin/env sh

if [ ! -d "build" ]; then
  mkdir build
fi

if [ ! -d "lib" ]; then
  mkdir lib
fi

cp "NDI SDK for Linux"/include/* include/
cp "NDI SDK for Linux"/lib/aarch64-rpi4-linux-gnueabi/* lib/

g++ -std=c++11 -pthread  -Wl,--allow-shlib-undefined -Wl,--as-needed -Iinclude/ -L lib/ndi -o build/v4l2ndi main.cpp PixelFormatConverter.cpp -lndi -ldl