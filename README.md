# CarClock

## Overview
My 2017 Toyota Camry is a great car, however, for the last few years, it gets into a situation where the entertainment system (or whatever you call it) won't fully initialize -- it just sits at the splash screen for a while, then reboots and goes back to the splash screen, then reboots, and so on...  From what I've read, this is likely due to a bad connector between the head unit and the external Bluetooth unit.  This situation happens a few times a year, and I can live with it, but alas, the one and only clock in the car is shown on the entertainment system.  Thus, because the system won't complete initialization, it never gets to the main screen where the clock is displayed, and I'm left without a clock.

So, I decided to create a real clock that I could use in my car.  The first clock I made was based off of the ESP8266 (and used the [Wemos D1 Mini](https://www.wemos.cc/en/latest/d1/d1_mini.html) form factor).  It's working fine for now, but eventually I will move to the ESP32 so I can add more features, but for now the ESP8266 based version is good enough.

## CarClock-ESP8266

Features :

2026/02/01 : Ver 1.0 :
- [Wemos D1 Mini](https://www.wemos.cc/en/latest/d1/d1_mini.html) form factor which I got from [Amazon](https://www.amazon.com/Organizer-ESP8266-Internet-Development-Compatible/dp/B081PX9YFV) a few years ago.
- [2004 LCD 20x4 LCD character display with an I2C "backpack"](https://www.amazon.com/HiLetgo-Backlight-Display-Arduino-MEGA2560/dp/B01DKETWO2).  (FYI, since I already some non-I2C LCD2004's laying around, I bought some I2C/LCD backup from AliExpress.  I could have bought them from [Amazon](https://www.amazon.com/HiLetgo-Interface-LCD1602-Address-Changeable/dp/B00VC2NEU8) instead.)
- 3D printed case.  (I'll eventually put the STL files in this repository once I'm done refining the case.)

2026/02/22 : Ver 1.1 & 1.2 :
- Preserve time using a [DS3231 I2C RTC (Real Time Clock)](https://www.amazon.com/dp/B07Q7NZTQS).
- An ASCII "screen" (which mimics the LCD2004) for debugging without requiring a physical LCD2004 to be present.

Notes :

- For more information on the features and history, please refer to the comments at the top of src/main.cpp file (I'm old-school and like having comments in the code instead of putting the comments in a separate file such as this README.md file, however I will try to keep the README.md up to date, but no guarantees :-)

## CarClock-ESP32

Features :
> I'll fill this section in when I migrate the project from an ESP8266 to an ESP32.

