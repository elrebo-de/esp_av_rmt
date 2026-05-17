/*
 * Example program to use rmt_ir functionality with elrebo-de/esp_rmt_ir
 */

#include <string>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "rmt_ir.hpp"
#include "generic_button.hpp"
#include "generic_nvsflash.hpp"
#include <iot_button.h>

static const char *tag = "ESP AV RMT Controller";

bool state = false;
std::string activeScene = "OFF"; // {OFF, TV, AppleTV, Radio}

// read value of key from NvsFlash
uint8_t readNvsFlashU8(std::string tag, std::string space, std::string key, esp_err_t *ret)
{
    /* Open NVS flash Namespace for read operations */
    GenericNvsFlash nvsRmt(tag, space, NVS_READONLY);
    return nvsRmt.GetU8(key, ret);
}
std::string readNvsFlashString(std::string tag, std::string space, std::string key, esp_err_t *ret)
{
    /* Open NVS flash Namespace for read operations */
    GenericNvsFlash nvsRmt(tag, space, NVS_READONLY);
    return nvsRmt.GetStr(key, ret);
}

// update value of key in NvsFlash
esp_err_t updateNvsFlash(std::string tag, std::string space, std::string key, uint8_t value)
{
    GenericNvsFlash nvsRmt(tag, space, NVS_READWRITE);
    return nvsRmt.SetU8(key, value);
}

esp_err_t updateNvsFlash(std::string tag, std::string space, std::string key, std::string value)
{
    GenericNvsFlash nvsRmt(tag, space, NVS_READWRITE);
    return nvsRmt.SetStr(key, value);
}

void switchAllOff(RmtIr* rmtIr) {
        // YAMAHA Receiver
        rmtIr->transmitNecCommandFrame((uint8_t)0x7a, (uint8_t)0x1e); // "STANDBY"
        vTaskDelay(pdMS_TO_TICKS(500)); // delay 0.5 seconds
        // Panasonic TV
        rmtIr->transmitPanasonicCommandFrame(0x4004, 0x01, 0x00, 0xfc); // "Power Off"
        vTaskDelay(pdMS_TO_TICKS(500)); // delay 0.5 seconds
        // Pioneer DVD Player
        rmtIr->transmitPioneerCommandFrame((uint8_t)0xa3, (uint8_t)0x99, (uint8_t)0xaf, (uint8_t)0xbb); // "Shift"
}

// Callback function for BUTTON_SINGLE_CLICK event from onBoardButton
extern "C" void callback_onBoardButton_BUTTON_SINGLE_CLICK(void *arg, void *data)
{
    ESP_LOGI("onBoardButton Callback", "for Event BUTTON_SINGLE_CLICK called!");

    iot_button_print_event((button_handle_t)arg);

    // bei jedem BUTTON_SINGLE_CLICK wird der Fernseher ein-/ausgeschaltet
    state = !state;
    RmtIr* rmtIr = &rmtIr->getInstance(); // get the Singleton instance
    if (!state) {
        switchAllOff(rmtIr);
        activeScene = "OFF";
    }
    else {
        // Panasonic TV
        rmtIr->transmitPanasonicCommandFrame(0x4004, 0x01, 0x00, 0x7c); // "Power On"
        vTaskDelay(pdMS_TO_TICKS(500)); // delay 0.5 seconds
        // YAMAHA receiver
        rmtIr->transmitNecCommandFrame((uint16_t)0x7a85, (uint16_t)0x037c); // "TV Scene"
        // Panasonic TV
        vTaskDelay(pdMS_TO_TICKS(2000)); // delay 2 seconds
        rmtIr->transmitPanasonicCommandFrame(0x4004, 0x01, 0x40, 0x0c); // "TV"
        activeScene = "TV";
    }

    updateNvsFlash(std::string("nvsRmt"), std::string("rmt"), std::string("state"), state);
    ESP_LOGI(tag, "state = %d", state);
    updateNvsFlash(std::string("nvsRmt"), std::string("rmt"), std::string("activeScene"), activeScene);
    ESP_LOGI(tag, "activeScene = %s", activeScene.c_str());
}

// Callback function for BUTTON_DOUBLE_CLICK event from onBoardButton
extern "C" void callback_onBoardButton_BUTTON_DOUBLE_CLICK(void *arg, void *data)
{
    ESP_LOGI("onBoardButton Callback", "for Event BUTTON_DOUBLE_CLICK called!");

    iot_button_print_event((button_handle_t)arg);

    // bei jedem BUTTON_DOUBLE_CLICK wird der AppleTV ein-/ausgeschaltet
    state = !state;
    ESP_LOGI(tag, "state = %d", state);
    RmtIr* rmtIr = &rmtIr->getInstance(); // get the Singleton instance
    if (!state) {
        switchAllOff(rmtIr);
        activeScene = "OFF";
    }
    else {
        // Panasonic TV
        rmtIr->transmitPanasonicCommandFrame(0x4004, 0x01, 0x00, 0x7c); // "Power On"
        // YAMAHA receiver
        vTaskDelay(pdMS_TO_TICKS(500)); // delay 0.5 seconds
        rmtIr->transmitNecCommandFrame((uint16_t)0x7a85, (uint16_t)0x0976); // "TV Scene"
        // Panasonic TV
        vTaskDelay(pdMS_TO_TICKS(2000)); // delay 2 seconds
        rmtIr->transmitPanasonicCommandFrame(0x4004, 0x01, 0x20, 0x0d); // "HDMI1" (direct)
        activeScene = "AppleTV";
    }

    updateNvsFlash(std::string("nvsRmt"), std::string("rmt"), std::string("state"), state);
    ESP_LOGI(tag, "state = %d", state);
    updateNvsFlash(std::string("nvsRmt"), std::string("rmt"), std::string("activeScene"), activeScene);
    ESP_LOGI(tag, "activeScene = %s", activeScene.c_str());
}

// Callback function for BUTTON_MULTIPLE_CLICK_3 event from onBoardButton
extern "C" void callback_onBoardButton_BUTTON_MULTIPLE_CLICK_3(void *arg, void *data)
{
    ESP_LOGI("onBoardButton Callback", "for Event BUTTON_MULTIPLE_CLICK_3 called!");

    iot_button_print_event((button_handle_t)arg);

    // bei jedem BUTTON_MULTIPLE_CLICK_3 wird der DVD-Spieler ein-/ausgeschaltet
    state = !state;
    ESP_LOGI(tag, "state = %d", state);
    RmtIr* rmtIr = &rmtIr->getInstance(); // get the Singleton instance
    if (!state) {
        switchAllOff(rmtIr);
        activeScene = "OFF";
   }
    else {
        // Panasonic TV
        rmtIr->transmitPanasonicCommandFrame(0x4004, 0x01, 0x00, 0x7c); // "Power On"
        // Pioneer DVD Player
        vTaskDelay(pdMS_TO_TICKS(500)); // delay 0.5 seconds
        rmtIr->transmitPioneerCommandFrame((uint8_t)0xa3, (uint8_t)0x99, (uint8_t)0xaf, (uint8_t)0xba); // "ON"
        // YAMAHA Receiver
        vTaskDelay(pdMS_TO_TICKS(500)); // delay 0.5 seconds
        rmtIr->transmitNecCommandFrame((uint16_t)0x7a85, (uint16_t)0x007f); // "BD/DVD Scene"
        // Pioneer DVD Player
        vTaskDelay(pdMS_TO_TICKS(500)); // delay 0.5 seconds
        rmtIr->transmitPioneerCommandFrame((uint8_t)0xa3, (uint8_t)0x99, (uint8_t)0xaf, (uint8_t)0xb6); // "OPEN/CLOSE"
        // Panasonic TV
        vTaskDelay(pdMS_TO_TICKS(500)); // delay 0.5 seconds
        rmtIr->transmitPanasonicCommandFrame(0x4004, 0x01, 0x00, 0x40); // "AV2" (direct)
        activeScene = "DVD";
    }

    updateNvsFlash(std::string("nvsRmt"), std::string("rmt"), std::string("state"), state);
    ESP_LOGI(tag, "state = %d", state);
    updateNvsFlash(std::string("nvsRmt"), std::string("rmt"), std::string("activeScene"), activeScene);
    ESP_LOGI(tag, "activeScene = %s", activeScene.c_str());
}

// Callback function for BUTTON_LONG_PRESS_START event from onBoardButton
extern "C" void callback_onBoardButton_BUTTON_LONG_PRESS_START_1000(void *arg, void *data)
{
    ESP_LOGI("onBoardButton Callback", "for Event BUTTON_LONG_PRESS_START_1000 called!");

    iot_button_print_event((button_handle_t)arg);

    // bei jedem BUTTON_LONG_PRESS_START wird der state umgeschaltet
    state = !state;
    ESP_LOGI(tag, "state = %d", state);
    RmtIr* rmtIr = &rmtIr->getInstance(); // get the Singleton instance

    if (!state) {
        switchAllOff(rmtIr);
        activeScene = "OFF";
    }
    else {
        rmtIr->transmitNecCommandFrame((uint8_t)0x7a, (uint8_t)0x1d); // "POWER ON"
        vTaskDelay(pdMS_TO_TICKS(500)); // delay 0.5 seconds
        rmtIr->transmitNecCommandFrame((uint8_t)0x7a, (uint8_t)0x16); // "Tuner"
        activeScene = "Radio";
    }

    updateNvsFlash(std::string("nvsRmt"), std::string("rmt"), std::string("state"), state);
    ESP_LOGI(tag, "state = %d", state);
    updateNvsFlash(std::string("nvsRmt"), std::string("rmt"), std::string("activeScene"), activeScene);
    ESP_LOGI(tag, "activeScene = %s", activeScene.c_str());
}

extern "C" void app_main(void)
{
    // short delay to reconnect logging

    vTaskDelay(pdMS_TO_TICKS(500)); // delay 0.5 seconds

    ESP_LOGI(tag, "Example Program");

    /* Initialize RmtIr class */
    ESP_LOGI(tag, "RmtIr");
    RmtIr* rmtIr = &rmtIr->getInstance(); // get the Singleton instance
    //rmtIr->setGpioPins(4,0); // set the GPIO pins for ESP32C3 Supermini
    rmtIr->setGpioPins(32,0); // set the GPIO pins for M5 ATOM LITE
    rmtIr->initialize(); // initialize RMT IR

    GenericButton onBoardButton(
	    std::string("onBoardButton"),
	    (gpio_num_t) 39, // GPIO for M5 Atom Lite
	    //(gpio_num_t) 9, // GPIO for ESP32C3 Supermini
	    0, // active = DOWN
	    true, // pull disabled - M5 Atom does not support internal PU/PD on this gpio
	    std::string("GPIO")
	);

    onBoardButton.RegisterCallbackForEvent(BUTTON_SINGLE_CLICK, callback_onBoardButton_BUTTON_SINGLE_CLICK);
    onBoardButton.RegisterCallbackForEvent(BUTTON_DOUBLE_CLICK, callback_onBoardButton_BUTTON_DOUBLE_CLICK);
    button_event_args_t press_time = {
       { // long_press
           1000, // press_time
       }
    };
    onBoardButton.RegisterCallbackForEvent(BUTTON_LONG_PRESS_START, &press_time, callback_onBoardButton_BUTTON_LONG_PRESS_START_1000);
    button_event_args_t clicks = {
       { // multiple_clicks
           3, // clicks
       }
    };
    onBoardButton.RegisterCallbackForEvent(BUTTON_MULTIPLE_CLICK, &clicks, callback_onBoardButton_BUTTON_MULTIPLE_CLICK_3);

    // read state and activeScene from nvs_flash
    {
        esp_err_t ret = ESP_OK;
        uint8_t nvsState = readNvsFlashU8(std::string("nvsRmt"), std::string("rmt"), std::string("state"), &ret);
        if (ret == ESP_OK) {
            state = (bool) nvsState;
        }
        ret = ESP_OK;
        std::string nvsActiveScene = readNvsFlashString(std::string("nvsRmt"), std::string("rmt"), std::string("activeScene"), &ret);
        if (ret == ESP_OK) {
            activeScene = nvsActiveScene;
        }
        ESP_LOGI(tag, "state = %d", state);
        ESP_LOGI(tag, "activeScene = %s", activeScene.c_str());
    }

    // receiver test
    while(1) {
        //ESP_LOGI(tag, "receiveNecOrPanasonicFrame");
        //rmtIr->receiveNecOrPanasonicFrame();
        vTaskDelay(pdMS_TO_TICKS(30000)); // delay 30 seconds
    }

}
