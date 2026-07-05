## ESP  AV RMT - A Remote Controller for a YAMAHA Audio Receiver, a PANASONIC Television, a Pioneer DVD Player and an Apple TV

We create a remote controller to power on 
* the Panasonic TV,
* the YAMAHA Audio Receiver and
* the Pioneer DVD Player

with a button click
and choose the right settings for
* watching TV (red button) color: red,
* watching Apple TV (yellow button) color: yellow,
* watching DVDs or listening to CDs (green button) color: green,
* listening to the Radio (left blue button) color: blue or
* listening to the Record Player (right blue button) color: blue or
* switching everything off (black button) color: white

and 
* increase volume on the receiver with button Volume+
* decrease volume on the receiver with button Volume-
* go to next channel (on TV, DVD and Radio) with button Channel+
* go to previous channel (on TV, DVD and Radio) with button Channel-
## Hardware
to be controlled:

* YAMAHA Audio Receiver
* Panasonic TV
* Pioneer DVD Player
* (Apple TV)

for the Remote Controller:

* Waveshare ESP32C3 Zero SoC 
* seeed Grove IR sender
* 3.7 V LiPo battery
* Charger board for 3.7V Li batteries, USB-C
* Aluminum package with 10 buttons 

## Build and Flash

Build the project and flash it to the board, then run the monitor tool to view the serial output:

* Run `source <directory with ESP-IDF version>/export.sh` to set IDF environment
* Run `idf.py set-target esp32c3` to set target chip
* Run `idf.py build` to build the project
* Run `idf.py flash monitor` to flash and monitor the project

(To exit the serial monitor, type `Ctrl-]` (or on a Mac `Ctrl-Option-6`).

See the Getting Started Guide for all the steps to configure and use the ESP-IDF to build projects.

## Function
The 10 buttons in the package are connected to a resistor ladder which is used as input to ADC unit 1, ADC channel 0 on GPIO 0 of the Waveshare ESP32C3 Zero board. 

For every button a GenericButton instance is created and for the BUTTON_SINGLE_CLICK event a callback function is defined.

For visual feedback the onboard LED is used. If the onboard LED is of type LED strip the color is changed according to the button pressed. This is unfortunately not true for the ESPC3 Supermini SoC.

### Actions
A single click on the red button powers on the TV and the Receiver for watching TV - the onboard led lights red.

A single click on the yellow button powers on the TV and the Receiver for watching Apple TV - the onboard led lights yellow.

A single click on the green button powers on the DVD Player, the TV and the Receiver for watching DVDs or listening to CDs the onboard led lights green.

A single click on the left blue button powers on the Receiver to listen to the Radio - the onboard led lights blue.

A single click on the right blue button powers on the Receiver to listen to the RecordPlayer - the onboard led lights blue.

A single click on the black button powers off everything - the onboard led lights white.

### DeepSleep
(( not yet activated))

The system goes to deep sleep after a command is executed - the onboard led goes off.

To wake up from deep sleep the onboard button must be clicked - the onboard led lights white.  