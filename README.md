# AvRmt - A Remote Controller for a YAMAHA Audio Receiver, a PANASONIC Television, a Pioneer DVD Player and an Apple TV

This repository contains an ESP-IDF component for a remote controller using different remote control protocols. It runs on a ESP32 processor connected to an IR transmitter and is built using the ESP-IDF build system in version 5.5+.

It uses component elrebo-de/rmt_ir for these audio/video devices:
* NEC protocol (used by YAMAHA audio receiver)
* Panasonic protocol (used by PANASONIC VIERA TV)
* Pioneer protocol (used by Pioneer DVD player)
* 
The component is implemented as C++ class `AvRmt`.

## Connecting the component and Usage

You need to include ```av_rmt.hpp```.

Then you have to initialize the `RmtIr` class which does the actual IR communication and then the `AvRmt` class which implements the specific scenario.
```C++
    ESP_LOGI(tag, "Initialize RmtIr class");
    RmtIr* rmtIr = &rmtIr->getInstance(); // get the Singleton instance
    rmtIr->setGpioPins(32,0); // set the GPIO pins for M5 ATOM LITE
    //rmtIr->setGpioPins(4,0); // set the GPIO pins for ESP32C3 Supermini
    rmtIr->initialize(); // initialize RMT IR

    ESP_LOGI(tag, "Initialize AvRmt class");
    AvRmt* avRmt = &avRmt->getInstance(); // get the Singleton instance

    avRmt->initialize(rmtIr);
```

Now you can control the audio/video devices with these methods:
```C++
        // Methods to control Audio/Video devices
        void switchAllOff();
        void switchOnTv();
        void switchOnAppleTv();
        void switchOnDvd();
        void switchOnRadio();
        void increaseVolume();
        void decreaseVolume();
        void goToNextChannel();
        void goToPreviousChannel();
```

## API
The API of the component is located in the include directory ```include/av_rmt.hpp``` and defines the
C++ class ```AvRmt```.

```AvRmt``` is a Singleton class.

The class definition is shown here:
```C++
/* class AvRmt
   Class to implement an IR Remote Controller for a
   * a YAMAHA Audio Receiver,
   * a Panasonic TV,
   * an AppleTV and
   * a Pioneer DVD Player

   Source of Singleton class structure - https://stackoverflow.com/a/1008289
   Posted by Loki Astari, modified by community. See post 'Timeline' for change history
   Retrieved 2026-02-01, License - CC BY-SA 4.0
*/

class AvRmt
{
    public:
        static AvRmt& getInstance();
        void initialize(RmtIr *rmtIr);

        // Methods to control Audio/Video devices
        void switchAllOff();
        void switchOnTv();
        void switchOnAppleTv();
        void switchOnDvd();
        void switchOnRadio();
        void incrementVolume();
        void decrementVolume();
        void goToNextChannel();
        void goToPreviousChannel();

    private:
        AvRmt() {}                 // Constructor

        // Methods to work with NvsFlash
        void readDataFromNvsFlash();
        void updateDataInNvsFlash();
        uint8_t readNvsFlashU8(std::string tag, std::string space, std::string key, esp_err_t *ret);
        std::string readNvsFlashString(std::string tag, std::string space, std::string key, esp_err_t *ret);
        esp_err_t updateNvsFlash(std::string tag, std::string space, std::string key, uint8_t value);
        esp_err_t updateNvsFlash(std::string tag, std::string space, std::string key, std::string value);

        std::string tag = "AvRmt";

        bool state = false;
        bool yamahaReceiver = false;
        bool panasonicTv = false;
        bool pioneerDvd = false;
        std::string activeScene = "OFF"; // {OFF, TV, AppleTV, Radio, DVD}

        RmtIr *rmtIr = NULL;

    public:
        AvRmt(AvRmt const&) = delete;
        void operator=(AvRmt const&) = delete;
};
```

# License
This component is provided under the Apache 2.0 license.
