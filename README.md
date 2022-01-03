# V4L2 to NDI

V4L2 to NDI is an application that connects to attached V4L2 devices and encodes the video signal into NDI

It uses the NDI library, allowing for auto-discovery of streams on the local
network.

## Supported devices

This software is tested with Raspberry Pi 4 and Intel Mini PCs. It typically runs well on a dual or quad core CPU that is clocked at or above 1.4 GHz, though results vary. This also works well with the TC358743 HDMI to CSI-2 adapter for Raspberry Pi. This particular HDMI adapter handles an input up to 1080p30 with no framedrops. 

### Install on Raspberry Pi OS 64-bit (recommended)

Make sure git is installed.

```
sudo apt update
sudo apt install git
```
Clone this repository and `cd` into it.

```
git clone https://github.com/windows10luke/V4L2-to-NDI.git && cd V4L2-to-NDI
```

Compile and install

```
sudo bash ./easy-install-rpi-aarch64.sh
```
Installation is now complete!


### Install on Raspberry Pi OS 32-bit

Make sure git is installed.

```
sudo apt update
sudo apt install git
```
Clone this repository and `cd` into it.

```
git clone https://github.com/windows10luke/V4L2-to-NDI.git && cd V4L2-to-NDI
```

Compile and install

```
sudo bash ./easy-install-rpi-armhf.sh
```
Installation is now complete!



### Install on x86_64 bit (Intel/AMD)

Make sure git is installed.

```
sudo apt update
sudo apt install git
```
Clone this repository and `cd` into it.

```
git clone https://github.com/windows10luke/V4L2-to-NDI.git && cd V4L2-to-NDI
```

Compile and install

```
sudo bash ./easy-install-x86_64.sh
```
Installation is now complete!



