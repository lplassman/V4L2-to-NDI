#!/usr/bin/env sh

set -eu

echo "Starting install of V4L2 to NDI for Raspberry Pi 4 64-bit..."

echo "Installing prerequisites..."
sudo bash ./preinstall.sh

echo "Downloading NDI libraries..."
sudo bash ./download_NDI_SDK.sh

echo "Building executable for Raspberry Pi 4 64-bit..."
sudo bash ./build_rpi4_arm64.sh

echo "Installing in final directory..."
sudo bash ./install.sh

echo "Done"
