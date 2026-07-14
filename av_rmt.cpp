/*
 * av_rmt.cpp
 *
 *      Author: christophoberle
 *
 * this work is licenced under the Apache 2.0 licence
 */

#include <string>
#include "esp_log.h"

#include "av_rmt.hpp"

#include "generic_nvsflash.hpp"

AvRmt& AvRmt::getInstance()
{
    static AvRmt instance; // Guaranteed to be destroyed. Instantiated on first use.
    return instance;
}

// Function to initialize AvRmt Singleton
void AvRmt::initialize(RmtIr *rmtIr)
{
    ESP_LOGI(tag.c_str(), "Initializing AvRmt");

    // set RmtIr
    this->rmtIr = rmtIr;

    // read data from nvs_flash
    this->readDataFromNvsFlash();
}

std::string AvRmt::getActiveScene() {
    return activeScene;
}

void AvRmt::switchAllOff() {
    ESP_LOGI(tag.c_str(), "switchAllOff");
    state = false;
    RmtIr* rmtIr = &rmtIr->getInstance(); // get the Singleton instance

    // YAMAHA Receiver
    rmtIr->transmitNecCommandFrame((uint8_t)0x7a, (uint8_t)0x1e); // "STANDBY"
    yamahaReceiver = false;
    vTaskDelay(pdMS_TO_TICKS(500)); // delay 0.5 seconds

    // Panasonic TV
    rmtIr->transmitPanasonicCommandFrame(0x4004, 0x01, 0x00, 0xfc); // "Power Off"
    panasonicTv = false;
    vTaskDelay(pdMS_TO_TICKS(500)); // delay 0.5 seconds

    // Pioneer DVD Player
    rmtIr->transmitPioneerCommandFrame((uint8_t)0xa3, (uint8_t)0x99, (uint8_t)0xaf, (uint8_t)0xbb); // "Shift + OFF"
    pioneerDvd = false;
    activeScene = "OFF";
    state = false;

    updateDataInNvsFlash();
}

void AvRmt::switchOnTv(){
    ESP_LOGI(tag.c_str(), "switchOnTv");
    state = true;
    RmtIr* rmtIr = &rmtIr->getInstance(); // get the Singleton instance

    // Panasonic TV
    rmtIr->transmitPanasonicCommandFrame(0x4004, 0x01, 0x00, 0x7c); // "Power On"
    panasonicTv = true;
    vTaskDelay(pdMS_TO_TICKS(2000)); // delay 2 seconds

    // YAMAHA Receiver
    rmtIr->transmitNecCommandFrame((uint16_t)0x7a85, (uint16_t)0x037c); // "TV Scene"
    yamahaReceiver = true;
    vTaskDelay(pdMS_TO_TICKS(500)); // delay 0.5 seconds

    // Panasonic TV
    rmtIr->transmitPanasonicCommandFrame(0x4004, 0x01, 0x40, 0x0c); // "TV"
    activeScene = "TV";
    vTaskDelay(pdMS_TO_TICKS(500)); // delay 0.5 seconds

    // Pioneer DVD Player
    rmtIr->transmitPioneerCommandFrame((uint8_t)0xa3, (uint8_t)0x99, (uint8_t)0xaf, (uint8_t)0xbb); // "Shift + OFF"
    pioneerDvd = false;

    updateDataInNvsFlash();
}

void AvRmt::switchOnAppleTv() {
    ESP_LOGI(tag.c_str(), "switchOnAppleTv");
    state = true;
    RmtIr* rmtIr = &rmtIr->getInstance(); // get the Singleton instance

    // Panasonic TV
    rmtIr->transmitPanasonicCommandFrame(0x4004, 0x01, 0x00, 0x7c); // "Power On"
    panasonicTv = true;
    vTaskDelay(pdMS_TO_TICKS(2000)); // delay 2 seconds

    // Pioneer DVD Player
    rmtIr->transmitPioneerCommandFrame((uint8_t)0xa3, (uint8_t)0x99, (uint8_t)0xaf, (uint8_t)0xbb); // "Shift + OFF"
    pioneerDvd = false;
    vTaskDelay(pdMS_TO_TICKS(500)); // delay 0.5 seconds

    // YAMAHA Receiver
    rmtIr->transmitNecCommandFrame((uint16_t)0x7a85, (uint16_t)0x0976); // "Radio Scene" (AppleTV)
    yamahaReceiver = true;
    vTaskDelay(pdMS_TO_TICKS(500)); // delay 0.5 seconds

    // Panasonic TV
    rmtIr->transmitPanasonicCommandFrame(0x4004, 0x01, 0x20, 0x0d); // "HDMI1" (direct)
    activeScene = "AppleTV";

    updateDataInNvsFlash();
}

void AvRmt::switchOnDvd() {
    ESP_LOGI(tag.c_str(), "switchOnDvd");
    state = true;
    ESP_LOGI(tag.c_str(), "state = %d", state);
    RmtIr* rmtIr = &rmtIr->getInstance(); // get the Singleton instance

    // Panasonic TV
    rmtIr->transmitPanasonicCommandFrame(0x4004, 0x01, 0x00, 0x7c); // "Power On"
    panasonicTv = true;
    vTaskDelay(pdMS_TO_TICKS(500)); // delay 1 seconds

    // Pioneer DVD Player
    rmtIr->transmitPioneerCommandFrame((uint8_t)0xa3, (uint8_t)0x99, (uint8_t)0xaf, (uint8_t)0xba); // "ON"
    pioneerDvd = true;
    vTaskDelay(pdMS_TO_TICKS(500)); // delay 1 seconds

    // YAMAHA Receiver
    rmtIr->transmitNecCommandFrame((uint16_t)0x7a85, (uint16_t)0x007f); // "BD/DVD Scene"
    yamahaReceiver = true;
    vTaskDelay(pdMS_TO_TICKS(1500)); // delay 0.5 seconds

    // Pioneer DVD Player
    rmtIr->transmitPioneerCommandFrame((uint8_t)0xa3, (uint8_t)0x99, (uint8_t)0xaf, (uint8_t)0xb6); // "OPEN/CLOSE"
    vTaskDelay(pdMS_TO_TICKS(500)); // delay 0.5 seconds

    // Panasonic TV
    rmtIr->transmitPanasonicCommandFrame(0x4004, 0x01, 0x00, 0x40); // "AV2" (direct)
    activeScene = "DVD";

    updateDataInNvsFlash();
}

void AvRmt::switchOnRecordPlayer(){
    ESP_LOGI(tag.c_str(), "switchOnRecordPlayer");
    state = true;
    RmtIr* rmtIr = &rmtIr->getInstance(); // get the Singleton instance

    // Panasonic TV
    rmtIr->transmitPanasonicCommandFrame(0x4004, 0x01, 0x00, 0xfc); // "Power Off"
    panasonicTv = false;
    vTaskDelay(pdMS_TO_TICKS(500)); // delay 0.5 seconds

    // YAMAHA Receiver
    rmtIr->transmitNecCommandFrame((uint16_t)0x7a85, (uint16_t)0x0679); // "CD Scene"
    yamahaReceiver = true;
    vTaskDelay(pdMS_TO_TICKS(500)); // delay 0.5 seconds

    // Pioneer DVD Player
    rmtIr->transmitPioneerCommandFrame((uint8_t)0xa3, (uint8_t)0x99, (uint8_t)0xaf, (uint8_t)0xbb); // "Shift + OFF"
    pioneerDvd = false;

    activeScene = "RecordPlayer";

    updateDataInNvsFlash();
}

void AvRmt::switchOnRadio() {
    ESP_LOGI(tag.c_str(), "switchOnRadio");
    state = true;
    ESP_LOGI(tag.c_str(), "state = %d", state);
    RmtIr* rmtIr = &rmtIr->getInstance(); // get the Singleton instance

    // YAMAHA Receiver
    rmtIr->transmitNecCommandFrame((uint8_t)0x7a, (uint8_t)0x1d); // "POWER ON"
    yamahaReceiver = true;
    vTaskDelay(pdMS_TO_TICKS(500)); // delay 0.5 seconds

    // Pioneer DVD Player
    rmtIr->transmitPioneerCommandFrame((uint8_t)0xa3, (uint8_t)0x99, (uint8_t)0xaf, (uint8_t)0xbb); // "Shift + OFF"
    pioneerDvd = false;
    vTaskDelay(pdMS_TO_TICKS(500)); // delay 0.5 seconds

    // Panasonic TV
    rmtIr->transmitPanasonicCommandFrame(0x4004, 0x01, 0x00, 0xfc); // "Power Off"
    panasonicTv = false;
    vTaskDelay(pdMS_TO_TICKS(500)); // delay 0.5 seconds

    // YAMAHA Receiver
    rmtIr->transmitNecCommandFrame((uint8_t)0x7a, (uint8_t)0x16); // "Tuner"
    activeScene = "Radio";

    updateDataInNvsFlash();
}

void AvRmt::incrementVolume() {
     ESP_LOGI(tag.c_str(), "incrementVolume");
     RmtIr* rmtIr = &rmtIr->getInstance(); // get the Singleton instance

     // YAMAHA Receiver
     rmtIr->transmitNecCommandFrame((uint8_t)0x7a, (uint8_t)0x1a); // "Volume Up"
}

void AvRmt::decrementVolume() {
     ESP_LOGI(tag.c_str(), "decrementVolume");
     RmtIr* rmtIr = &rmtIr->getInstance(); // get the Singleton instance

     // YAMAHA Receiver
     rmtIr->transmitNecCommandFrame((uint8_t)0x7a, (uint8_t)0x1b); // "Volume Down"
}

void AvRmt::goToNextChannel() {
    ESP_LOGI(tag.c_str(), "goToNextChannel");
    RmtIr* rmtIr = &rmtIr->getInstance(); // get the Singleton instance

    if (activeScene == "OFF") {
        // nothing to do
    }
    else if (activeScene == "TV") {
        // go to next TV channel
        rmtIr->transmitPanasonicCommandFrame(0x4004, 0x01, 0x00, 0x2c); // "Channel Up"
    }
    else if (activeScene == "AppleTV") {
        // nothing to do (not implemented)
    }
    else if (activeScene == "DVD") {
        // go to next title on CD/DVD
        rmtIr->transmitPioneerCommandFrame((uint8_t)0xa3, (uint8_t)0x9c); // "Next"
    }
    else if (activeScene == "Radio") {
        // go to next preset station
        // YAMAHA Receiver
        rmtIr->transmitNecCommandFrame((uint8_t)0x7a, (uint8_t)0x10); // "Next Preset Station"
    }
    else if (activeScene == "RecordPlayer") {
        // nothing to do;
    }

}

void AvRmt::goToPreviousChannel() {
    ESP_LOGI(tag.c_str(), "goToPreviousChannel");
    RmtIr* rmtIr = &rmtIr->getInstance(); // get the Singleton instance

    if (activeScene == "OFF") {
        // nothing to do
    }
    else if (activeScene == "TV") {
        // go to previous TV channel
        rmtIr->transmitPanasonicCommandFrame(0x4004, 0x01, 0x00, 0xac); // "Channel Down"
    }
    else if (activeScene == "AppleTV") {
        // nothing to do (not implemented)
    }
    else if (activeScene == "DVD") {
        // go to previous title on CD/DVD
        // first "Previous" command goes to beginning of current title
        rmtIr->transmitPioneerCommandFrame((uint8_t)0xa3, (uint8_t)0x9d); // "Previous"
        // wait a moment
        vTaskDelay(pdMS_TO_TICKS(500)); // delay 0.5 seconds
        // second "Previous" command goes to beginning of previous title
        rmtIr->transmitNecCommandFrame((uint8_t)0xa3, (uint8_t)0x9d); // "Previous"

    }
    else if (activeScene == "Radio") {
        // go to previous preset station
        // YAMAHA Receiver
        rmtIr->transmitNecCommandFrame((uint8_t)0x7a, (uint8_t)0x11); // "Previous Preset Station"
    }
    else if (activeScene == "RecordPlayer") {
        // nothing to do;
    }

}

// read value of key from NvsFlash
uint8_t AvRmt::readNvsFlashU8(std::string tag, std::string space, std::string key, esp_err_t *ret)
{
    /* Open NvsFlash namespace for read operations */
    GenericNvsFlash nvsRmt(tag, space, NVS_READONLY);
    return nvsRmt.GetU8(key, ret);
}
std::string AvRmt::readNvsFlashString(std::string tag, std::string space, std::string key, esp_err_t *ret)
{
    /* Open NvsFlash namespace for read operations */
    GenericNvsFlash nvsRmt(tag, space, NVS_READONLY);
    return nvsRmt.GetStr(key, ret);
}

// read data from NvsFlash
void AvRmt::readDataFromNvsFlash()
{
    esp_err_t ret;
    state = (bool) readNvsFlashU8(std::string("nvsRmt"), std::string("rmt"), std::string("state"), &ret);
    ESP_LOGI(tag.c_str(), "state = %d", state);
    yamahaReceiver = (bool) readNvsFlashU8(std::string("nvsRmt"), std::string("rmt"), std::string("yamahaReceiver"), &ret);
    ESP_LOGI(tag.c_str(), "yamahaReceiver = %d", yamahaReceiver);
    panasonicTv = (bool) readNvsFlashU8(std::string("nvsRmt"), std::string("rmt"), std::string("panasonicTv"), &ret);
    ESP_LOGI(tag.c_str(), "panasonicTv = %d", panasonicTv);
    pioneerDvd = (bool) readNvsFlashU8(std::string("nvsRmt"), std::string("rmt"), std::string("pioneerDvd"), &ret);
    ESP_LOGI(tag.c_str(), "pioneerDvd = %d", pioneerDvd);
    activeScene = readNvsFlashString(std::string("nvsRmt"), std::string("rmt"), std::string("activeScene"), &ret);
    ESP_LOGI(tag.c_str(), "activeScene = %s", activeScene.c_str());
}

// update value of key in NvsFlash
esp_err_t AvRmt::updateNvsFlash(std::string tag, std::string space, std::string key, uint8_t value)
{
    GenericNvsFlash nvsRmt(tag, space, NVS_READWRITE);
    return nvsRmt.SetU8(key, value);
}

// update value of key in NvsFlash
esp_err_t AvRmt::updateNvsFlash(std::string tag, std::string space, std::string key, std::string value)
{
    GenericNvsFlash nvsRmt(tag, space, NVS_READWRITE);
    return nvsRmt.SetStr(key, value);
}

// update data in NvsFlash
void AvRmt::updateDataInNvsFlash() {
    updateNvsFlash(std::string("nvsRmt"), std::string("rmt"), std::string("state"), state);
    ESP_LOGI(tag.c_str(), "state = %d", state);
    updateNvsFlash(std::string("nvsRmt"), std::string("rmt"), std::string("yamahaReceiver"), yamahaReceiver);
    ESP_LOGI(tag.c_str(), "yamahaReceiver = %d", yamahaReceiver);
    updateNvsFlash(std::string("nvsRmt"), std::string("rmt"), std::string("panasonicTv"), panasonicTv);
    ESP_LOGI(tag.c_str(), "panasonicTvr = %d", panasonicTv);
    updateNvsFlash(std::string("nvsRmt"), std::string("rmt"), std::string("pioneerDvd"), pioneerDvd);
    ESP_LOGI(tag.c_str(), "pioneerDvd = %d", pioneerDvd);
    updateNvsFlash(std::string("nvsRmt"), std::string("rmt"), std::string("activeScene"), activeScene);
    ESP_LOGI(tag.c_str(), "activeScene = %s", activeScene.c_str());
}

