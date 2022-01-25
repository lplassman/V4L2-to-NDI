# V4L2 to NDI

V4L2 to NDI is an application that connects to attached V4L2 devices and encodes the video signal into NDI

It uses the NDI library, allowing for auto-discovery of streams on the local
network.

## Supported devices

This software is tested with Raspberry Pi 4 and Intel Mini PCs. It typically runs well on a dual or quad core CPU that is clocked at or above 1.4 GHz, though results vary. This also works well with the TC358743 HDMI to CSI-2 adapter for Raspberry Pi. This particular HDMI adapter handles an HDMI input up to 1080p30 with no framedrops. 

### Download required installation files

Make sure git is installed.

```
sudo apt update
sudo apt install git
```
Clone this repository and `cd` into it.

```
git clone https://github.com/windows10luke/V4L2-to-NDI.git && cd V4L2-to-NDI
```


### Install on Raspberry Pi OS 64-bit (recommended)

Run this compile and install script

```
sudo bash ./easy-install-rpi-aarch64.sh
```
Installation is now complete!



### Install on Raspberry Pi OS 32-bit (not recommended)

Run this compile and install script

```
sudo bash ./easy-install-rpi-armhf.sh
```
Installation is now complete!



### Install on x86_64 bit (Intel/AMD)

Run this compile and install script

```
sudo bash ./easy-install-x86_64.sh
```
Installation is now complete!



### Usage

Once the installation process is complete, it will create an executable file located at /opt/v4l2ndi/bin/v4l2ndi

The installer also creates a symlink to /usr/bin so that it can be run from a normal terminal.

To see all the available options run the following

```
v4l2ndi -h
```

Typically, this app is run with a 30fps input with a pixel format input of YUV2. This command creates the NDI stream for that given input

```
v4l2ndi -d /dev/video0 -f
```

In case a 60fps input is needed, this command would be used instead

```
v4l2ndi -d /dev/video0 -f -n 60000 -e 1001
```

### Usage with TC358743 HDMI to CSI-2 adapter and Raspberry Pi 4

Edit the boot configuration and add the boot overlay for TC358743
```
sudo nano /boot/config.txt
```
Add the following line into the boot configuration file:
```
dtoverlay=tc358743
```
Run this script to fix EDID on boot:
```
sudo bash ./install-edid.sh
```
By default this service file runs v4l2ndi as the root user with realtime CPU scheduling.
```
sudo cp ./v4l2ndi_csi.service /etc/systemd/system/
sudo systemctl enable v4l2ndi_csi.service
```

```
sudo reboot
```

## Suggested Hardware

The following latency observations are based on a Raspberry Pi 4 running Raspberry Pi OS 64-bit.

If needing 1080p60, a USB 3.0 HDMI capture interface is required. In this case, I would suggest an Elgato capture card or one that I used made by Hornettek. Latency will be a little higher when using this method compared to the CSI input (about 120ms). Framedrops are also likely when using 1080p60 and the YUYV image format (due to image format conversion).

If only using 1080p30, a HDMI to CSI adapter works good and provides lower latency (about 90ms).


## Helpful links

https://wiki.geekworm.com/CSI-2-Software