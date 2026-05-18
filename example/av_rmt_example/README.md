## ESP  AV RMT - A Remote Controller for a YAMAHA Audio Receiver, a PANASONIC Television, a Pioneer DVD Player and an Apple TV

We create a remote controller to power on 
* the Panasonic TV,
* the YAMAHA Audio Receiver and
* the Pioneer DVD Player

with a button click
and choose the right settings for
* watching TV (SINGLE CLICK) color: green,
* watching Apple TV (DOUBLE CLICK) color: blue,
* watching DVDs or listening to CDs (TRIPLE CLICK) color: yellow,
* listening to the Radio (LONG PRESS UP (1 second press time)) color: pink or
* switching everything off (LONG PRESS START (3 seconds press time)) color: red

## Hardware
to be controlled:

* YAMAHA Audio Receiver
* Panasonic TV
* Pioneer DVD Player
* (Apple TV)  

for the Remote Controller:

* ESP-C3 Supermini SoC or M5 Atom Lite ESP32 SoC and optional M5 Tailbat Battery
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
The onboard button on the ESP32C3 Supermini or the M5 Atom Lite is used to control the devices.

### Actions
A single click powers on the TV and the Receiver for watching TV - the onboard led lights green.

A double click powers on the TV and the Receiver for watching Apple TV - the onboard led lights blue.

A triple click powers on the DVD Player, the TV and the Receiver for watching DVDs or listening to CDs the onboard led lights yellow.

A long press of 1 second press time powers on the Receiver to listen to the Radio - the onboard led lights pink.

A longer press of 3 seconds press time powers off everything - the onboard led lights red.

### DeepSleep
The system goes to deep sleep after a command is executed - the onboard led goes off.

To wake up from deep sleep the onboard button must be clicked - the onboard led lights white.  