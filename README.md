# Dongle Dumper.
Dump the firmware off a Original Xbox DVD Movie Playback Dongle straight from the console. The dongle contains a ROM with an XBE which provides some functions for the DVD playback application.

* See https://xboxdevwiki.net/Xbox_DVD_Movie_Playback_Kit
* Code adapted from https://github.com/XboxDev/dump-dvd-kit/blob/master/dump-dvd-kit.py

## Compile
Install nxdk dependencies and get usb host fork
```
sudo apt-get install flex bison clang lld
git clone https://github.com/Ryzee119/nxdk.git
cd nxdk
git checkout usbh
git submodule init
git submodule update
```
Then
```
git clone https://github.com/Ryzee119/Dongle_Dumper.git
cd Dongle_Dumper
make NXDK_DIR=/path/to/nxdk
```

## Usage
* Copy the `default.xbe` to a folder on your xbox and launch it. See [releases](https://github.com/Ryzee119/Dongle_Dumper/releases).
* Connect a Xbox DVD Movie Playback Dongle
* The firmware file will be written to the same directory as the xbe.
