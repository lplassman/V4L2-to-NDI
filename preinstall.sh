#!/usr/bin/env sh

apt-get update

#install prerequisites
apt-get -y install \
    avahi-daemon \
    avahi-discover \
    avahi-utils \
    libssl-dev \
    libconfig++-dev \
    || exit 1

