Badger
=======

This repository contains the firmware for the X-CTF 2016 badge.

Check under **Releases** to find the actual binary blob that was flashed 
into the participants' badges.

The hardware portion for this badge is also open-source:
<https://github.com/jellyjellyrobot/neander>

If you are interested in the design process, 
I wrote up about the hardware and software design aspects separately in these blog posts:

- <https://irq5.io/2016/06/22/designing-the-x-ctf-2016-badge/>
- <https://irq5.io/2016/07/19/x-ctf-2016-badge-firmware/>

Feel free to use this code in your own projects 
(see the *License* section below for details).

Special thanks to Jeremias, Yu Siang and the rest of the gang in the 
NUS Greyhats team for helping to design and assemble the badges.


Compiling
==========

1. Install the Arduino IDE and ESP8266 addon by [following the Adafruit guide](https://learn.adafruit.com/adafruit-feather-huzzah-esp8266/using-arduino-ide)
2. Clone this repo into your Arduino sketches folder.
3. Open the badger project in the Arduino IDE and click the Upload button


Uploading without Compilation
===============================

Use the [esptool.py](https://github.com/themadinventor/esptool) and specify the binary file `badger.ino.bin`.
You can find this file under the **Releases** section in the GitHub repo.
The command-line should look like the following:

    esptool.py -b 921600 -p /dev/ttyUSB1 write_flash -ff 80m -fs 32m 0x0000 badger.ino.bin

If you are interested in mass-uploading the firmware to ESP8266 devices, 
check out the method I described in my blog post.


License
========

**badger is licensed under the 3-clause ("modified") BSD License.**

Copyright (C) 2016 Darell Tan & Jacob Soo

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. The name of the author may not be used to endorse or promote products
   derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

