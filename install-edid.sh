#!/usr/bin/env sh

INSTALL_DIR="/opt/v4l2ndi"
BIN_DIR="$INSTALL_DIR/bin"

if [ ! -d "$INSTALL_DIR" ]; then
  mkdir "$INSTALL_DIR"
fi

if [ ! -d "$BIN_DIR" ]; then
  mkdir "$BIN_DIR"
fi

cp edid.txt "$BIN_DIR"
cp fix-edid.service /etc/systemd/system/
systemctl enable fix-edid.service