#!/usr/bin/env sh

set -eu

echo "Starting install of V4L2 to NDI for Raspberry Pi 4 64-bit...\n"

echo "Installing prerequisites...\n"
sudo bash ./preinstall.sh

echo "Downloading NDI libraries...\n"
sudo bash ./download_NDI_SDK.sh

echo "Building executable for Raspberry Pi 4 64-bit...\n"
sudo bash ./build_rpi4_arm64.sh

echo "Installing in final directory...\n"
sudo bash ./install.sh

echo "Done"
