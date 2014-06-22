opensono
========

A Raspberry-pi based audio streamer for home.

CURRENT STATE : Not usable yet

The purpose of this project is to create a low cost audio streaming system for home.

The idea is to use a simple jack input with an audio adaptor connected to a raspberry-pi, take this audio stream, send it to others raspberry-pi's and play it.

Audio source --> Jack Cable --> Raspberry-Pi --> Wifi network --> Raspberry-Pi --> Jack cable --> Audio Output

The purpose may not be to support a huge number of hardware. When everything will be tested I will put here a list of the required materials. I will try to keep the price as low as possible. Of course later it will be possible to add other hardware.


Planned features :
- Set up a minimal test program to read/write a raw audio file on a PC from an audio adaptor : OK
- Set up an optimized Buildroot for the Rapsberry and add script to facilitate further development : OK
- Cross compile driver for USB wifi dongle  : TP-LINK TL-WN725N V2 (rtl8188 driver) : OK
- Cross compile the test program and read raw audio data on a raspberry : OK
- Stream Raw audio data through wifi
- Optimize latency
- Facilitate user configuration 
- Wifi Hotspot mode ?
- Android app ?

For further information please see : http://agrou.eu



To checkout everything :
git clone https://github.com/JGrossholtz/opensono
git submodule update --init

