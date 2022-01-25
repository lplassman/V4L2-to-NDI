#!/usr/bin/env sh

apt-get update

#install prerequisites
apt-get -y install \
    g++ \
    avahi-daemon \
    avahi-discover \
    avahi-utils \
    libssl-dev \
    libconfig++-dev \
    curl \
    || exit 1

