gpioThru.c : I reccomend running the setup up program before running this, as although all the pins are controlled in the main program behavior can be a bit odd. It currently outputs onto two of the USR leds (3, 1) and out onto gpio pin 3. These are controlled by two buttons on gpio pins 30 and 60. When hooked up to a oscilloscope, with a function generater outputting a 3V square wave at 2 Hz, delays ranged from about 1uS to about 4mS, with rare spikes up to 8. When a file was being copied over spikes up to about 20uS were observed.

 Setup: run make followed by ./gpioThru
