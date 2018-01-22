Exclave Tests for Tomu
================

This is a configuration directory for Tomu tests, written for use with Exclave.

Currently the test sequence is:

1. Load firmware
1. Blink LEDs 3x

Future versions of the test should perform a more rigorous inspection of the connectivity, but for now we assume that if the firmware load is successful, and if both LEDs can blink, then everything else was assembled correctly.

Usage
------

1. Build [Exclave](https://exclave.io/)
1. Run exclave -c [path-to-this-directory]