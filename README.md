Wireless Sensor Node
====================

This repo contains the source code and assets for my wireless sensor node 
project.

<img src="https://github.com/fasaxc/WirelessSensorNode/raw/master/assets/box.jpg" width="333" height="500" alt="Completed sensor module">

<img src="https://github.com/fasaxc/WirelessSensorNode/raw/master/assets/node.jpg" width="500" height="333" alt="Completed sensor module">

Code
----

The root directory of the repository is an Eclipse CDT/AVR project targetting
the ATTiny13, which is a miniscule microcontroller with only 1K of ROM and 
64 bytes of RAM.

The code in main.c implements a main loop that:

* reads a TMP36 temperature sensor on the defined pin
* writes a Manchester-encoded signal on the defined transmit pin (which should be connected to <a href="http://www.sparkfun.com/products/10534">something like this</a>).
* goes into deep sleep for about 4 minutes to preserve battery life

To reduce battery usage further, the TMP36 is driven directly via a 
microcontroller pin and only turned on when needed.  Likewise, the transmitter 
is only turned on when needed; since it uses more power, I used a transistor 
attached to the "RADIO_POWER_PIN".

The code is released under the BSD license.

Assets
------

The diagram for laser-cut box is in the assets directory.  It works well when 
cut in 3mm MDF or plywood.  I've tried birch and walnut ply from laserbits.com.

<a rel="license" href="http://creativecommons.org/licenses/by-nc-sa/3.0/"><img alt="Creative Commons License" style="border-width:0" src="http://i.creativecommons.org/l/by-nc-sa/3.0/88x31.png" /></a><br /><span xmlns:dct="http://purl.org/dc/terms/" href="http://purl.org/dc/dcmitype/StillImage" property="dct:title" rel="dct:type">Laser cut box</span> by <span xmlns:cc="http://creativecommons.org/ns#" property="cc:attributionName">Shaun Crampton</span> is licensed under a <a rel="license" href="http://creativecommons.org/licenses/by-nc-sa/3.0/">Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License</a>.

The box design is written up in <a href="http://www.instructables.com/id/Laser-cut-enclosure-with-living-hinge-lid/">an instructable</a>.
