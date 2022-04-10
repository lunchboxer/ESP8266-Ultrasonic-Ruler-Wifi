# ESP8266-Ultrasonic-Ruler
Ultrasonic ruler Arduino project for Wemos D1 mini

This project uses platformio and is set up for emacs, but should work in any editor.

## Purpose

Primary school students can apply the concept of measuring distances using a known fixed speed and measuring the time, with a ultrasonic ruler. Distance equals speed times time and the speed of sound is about 340 m/s depending on temperature and humidity.

## Components

The project uses:
- Wemos D1 Mini esp8266 microcontroller
- HC SR04 ultrasonic ranging module.

Instead of an OLED display this version is designed to output to a WiFi enabled device with a screen such as an iPad or phone.

## Usage

Turn it on and look for "Ultrasonic Ruler" wifi network. The device will then probably default to 192.168.4.1. Open that in a web browser once connected to the device and you can read the distance in cm
