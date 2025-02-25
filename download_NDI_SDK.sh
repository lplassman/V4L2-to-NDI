#!/usr/bin/env sh

#download and extract NDI
curl -s https://downloads.ndi.tv/SDK/NDI_SDK_Linux/Install_NDI_SDK_v6_Linux.tar.gz | tar xvz -C /tmp/
yes y | bash /tmp/Install_NDI_SDK_v6_Linux.sh > /dev/null
