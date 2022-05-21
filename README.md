# V4L2 to NDI

V4L2 to NDI is an application that connects to attached V4L2 devices and encodes the video signal into NDI

It uses the NDI library, allowing for auto-discovery of streams on the local
network.

## Supported devices

This software is tested with Raspberry Pi 4 and Intel Mini PCs. It typically runs well on a dual or quad core CPU that is clocked at or above 1.4 GHz, though results vary. This also works well with the TC358743 HDMI to CSI-2 adapter for Raspberry Pi. This particular HDMI adapter handles an HDMI input up to 1080p30 with no framedrops. 

### Download required installation files

Make sure git is installed.

```
sudo apt update -y
sudo apt install git -y
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
Run this script to fix EDID on boot (installs fix-edid.service):
```
sudo bash ./install-edid.sh
```
By default this service file runs v4l2ndi as the root user with realtime CPU scheduling.
Edit the service file with this command before copying it
```
sudo nano ./v4l2ndi_csi.service
```
Copy the service and enable it
```
sudo cp ./v4l2ndi_csi.service /etc/systemd/system/
sudo systemctl enable v4l2ndi_csi.service
```

```
sudo reboot
```


## Suggested Hardware for HDMI Input

Hornettek HDMI to USB 3.0 Capture Card (1080p60)
https://www.amazon.com/s?k=hornettek+hdmi+capture&crid=IL1SBF4A56VF&sprefix=hornettek+hdmi+capture%2Caps%2C134&ref=nb_sb_noss

Waveshare HDMI to CSI-2 Adapter (1080p30)
https://www.amazon.com/s?k=waveshare+hdmi+to+csi&crid=24ROTNS3MO1F5&sprefix=waveshare+hdmi+to+csi%2Caps%2C136&ref=nb_sb_noss

## Latency Notes (using the suggested HDMI input hardware)
These tests were performed using a Raspberry Pi 4B with the latest Raspberry Pi OS (64-bit) software

- Hornettek HDMI capture card at 1080p60: ~110ms
- Hornettek HDMI capture card at 1080p30: ~230ms
- Waveshare HDMI to CSI-2 adapter at 1080p50: ~160ms
- Waveshare HDMI to CSI-2 adapter at 1080p30: ~230ms

## Current Limitations
- Raspberry Pi Cameras are not supported when using libcamera (use this project instead: https://github.com/raspberry-pi-camera/raspindi)
- Supports 6 NDI client connections when input video format is UYVY at 1080p30
- Supports 5 NDI client connections when input video format is UYVY at 1080p60
- Supports 2 NDI client connections when input video format is YUV2 at 1080p60

## Helpful links

https://wiki.geekworm.com/CSI-2-Software
