#!/usr/bin/env sh

INSTALL_DIR="/opt/v4l2ndi"
BIN_DIR="$INSTALL_DIR/bin"
LIB_DIR="$INSTALL_DIR/lib"

mkdir "$INSTALL_DIR"
mkdir "$LIB_DIR"
mkdir "$BIN_DIR"

cp lib/* "$LIB_DIR"

cp build/v4l2ndi "$BIN_DIR"

chmod +x "$BIN_DIR/v4l2ndi"

#symlink to the /usr/bin directory
ln -s "$BIN_DIR/v4l2ndi" /usr/bin/