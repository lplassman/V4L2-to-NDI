#!/usr/bin/env sh

INSTALL_DIR="/opt/v4l2ndi"
BIN_DIR="$INSTALL_DIR/bin"
LIB_DIR="/usr/lib"

if [ ! -d "$INSTALL_DIR" ]; then
  mkdir "$INSTALL_DIR"
fi

if [ ! -d "$LIB_DIR" ]; then
  mkdir "$LIB_DIR"
fi

if [ ! -d "$BIN_DIR" ]; then
  mkdir "$BIN_DIR"
fi

cp lib/* "$LIB_DIR"

cp build/v4l2ndi "$BIN_DIR"

chmod +x "$BIN_DIR/v4l2ndi"

#symlink to the /usr/bin directory
ln -s "$BIN_DIR/v4l2ndi" /usr/bin/