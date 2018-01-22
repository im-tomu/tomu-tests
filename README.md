Exclave Tests for Tomu
================

This is a configuration directory for Tomu tests, written for use with Exclave.

Currently the test sequence is:

1. Load firmware
1. Blink LEDs 3x

Future versions of the test should perform a more rigorous inspection of the connectivity, but for now we assume that if the firmware load is successful, and if both LEDs can blink, then everything else was assembled correctly.

Hardware
--------

This hardware was designed to run on a Raspberry Pi.  With the current configuration, the following connections must be made from the Pi header:

````
+--------------------+
|55G---G-D-C---G-G---|
|----GRYg----G-----SG|
+--------------------+

G = GND
R = Red LED
Y = Yellow LED
g = Green LED
S = Switch
D = SWD
C = SWC
5 = 5V
````

| Pin | Target
| --- | ------
| 2   | Tomu +5V
| 6   | Tomu GND
| 9   | LEDs GND
| 11  | Red LED
| 13  | Yellow LED
| 15  | Green LED
| 18  | Tomu SWD
| 22  | Tomu SWC
| 37  | START button
| 39  | START button (GND)

Usage
------

1. Build [Exclave](https://exclave.io/)
1. Run exclave -c [path-to-this-directory]