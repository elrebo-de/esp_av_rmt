/*
 * av_rmt.hpp
 *
 *      Author: christophoberle
 *
 * this work is licenced under the Apache 2.0 licence
 */

#ifndef AV_RMT_HPP_
#define AV_RMT_HPP_

#include <string>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "rmt_ir.hpp"


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
        void switchOnRecordPlayer();

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

#endif /* AV_RMT_HPP_ */
