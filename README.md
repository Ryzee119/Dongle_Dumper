# Dongle Dumper.
Dump the firmware off a Original Xbox DVD Movie Playback Dongle. The dongle contains a ROM with an XBE which provides some functions for the DVD playback application.

* See https://xboxdevwiki.net/Xbox_DVD_Movie_Playback_Kit
* Code adapted from https://github.com/XboxDev/dump-dvd-kit/blob/master/dump-dvd-kit.py

## Compile
-------------------------
First setup and install [nxdk](https://github.com/XboxDev/nxdk).
Then do this from your working directoy:  
```
make NXDK_DIR=/path/to/nxdk
```