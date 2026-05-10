## ESP AV RMT - A Remote Controller for a YAMAHA Audio Receiver, a PANASONIC Television and an Apple TV

We create a remote controller to power on/off 
* the PANASONIC TV and 
* the YAMAHA Audio Receiver 

with a single button click
and choose the right settings for
* watching TV,
* watching Apple TV or
* listening to the Radio.

The YAMAHA Audio Receiver and the PANASONIC TV are switched on/off with a Shelly Plug.

## Hardware
to be controlled:

* YAMAHA Audio Receiver
* PANASONIC TV
* Apple TV
* ShellyPlug

for the Remote Controller:

* ESP-C3 Supermini
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

At power-on TV and Receiver are switched on with the Shelly Plug and then the right configuration is set with the IR Remote Controller..

At power-off the TV is switched to stand-by with the IR Remote Controller and then TV and Receiver are switched off with the Shelly Plug.
