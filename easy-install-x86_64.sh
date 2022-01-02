#!/usr/bin/env sh

set -eu

echo "Starting install of V4L2 to NDI for x86_64-bit..."

echo "Installing prerequisites..."
sudo bash ./preinstall.sh

echo "Downloading NDI libraries..."
sudo bash ./download_NDI_SDK.sh

echo "Building executable for x86_64-bit..."
sudo bash ./build_x86_64.sh

echo "Installing in final directory..."
sudo bash ./install.sh

echo "Done"
