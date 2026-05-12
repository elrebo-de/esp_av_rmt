## ESP  AV RMT - A Remote Controller for a YAMAHA Audio Receiver, a PANASONIC Television and an Apple TV

We create a remote controller to power on/off 
* the PANASONIC TV and 
* the YAMAHA Audio Receiver 

with a button click
and choose the right settings for
* watching TV (SINGLE CLICK),
* watching Apple TV (DOUBLE CLICK) or
* listening to the Radio (LONG CLICK).

## Hardware
to be controlled:

* YAMAHA Audio Receiver
* PANASONIC TV
* Apple TV

for the Remote Controller:

* ESP-C3 Supermini or M5 ATOM LITE ESP32 and optional M5 Tailbat Battery
* seeed Grove IR sender

## Build and Flash

Build the project and flash it to the board, then run the monitor tool to view the serial output:

* Run `source <directory with ESP-IDF version>/export.sh` to set IDF environment
* Run `idf.py set-target esp32c3` to set target chip
* Run `idf.py build` to build the project
* Run `idf.py flash monitor` to flash and monitor the project

(To exit the serial monitor, type `Ctrl-]` (or on a Mac `Ctrl-Option-6`).

See the Getting Started Guide for all the steps to configure and use the ESP-IDF to build projects.

## Function
The onboard button on the ESP32C3 Supermini is used to control the devices.

A single click powers on/off the TV and the Receiver for watching TV.

A double click powers on/off the TV and the Receiver for watching Apple TV.

A long click power on/off the Receiver to listen to the Radio.

At power-on TV and Receiver are switched on with the IR Remote Controller and then the right configuration is set.

At power-off the TV and Receiver are switched to stand-by with the IR Remote Controller.
