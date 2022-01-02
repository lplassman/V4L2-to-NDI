#!/usr/bin/env sh

set -eu

echo "Starting install of V4L2 to NDI for Raspberry Pi 4 64-bit...\n"

echo "Installing prerequisites...\n"
sudo ./preinstall.sh

echo "Downloading NDI libraries...\n"
sudo ./download_NDI_SDK.sh

echo "Building executable for Raspberry Pi 4 64-bit...\n"
sudo ./build_rpi4_arm64.sh

echo "Installing in final directory...\n"
sudo ./install.sh

echo "Done"
