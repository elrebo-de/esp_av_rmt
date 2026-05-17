/* Example program using
   * elrebo-de/esp_rmt_ir for rmt_ir functionality
   * elrebo-de/generic_button for onBoard button functionality
   * elrebo-de/generic_nvsflash for storing data in NvsFlash
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
bool yamahaReceiver = false;
bool panasonicTv = false;
bool pioneerDvd = false;
std::string activeScene = "OFF"; // {OFF, TV, AppleTV, Radio, DVD}

// read value of key from NvsFlash
uint8_t readNvsFlashU8(std::string tag, std::string space, std::string key, esp_err_t *ret)
{
    /* Open NvsFlash namespace for read operations */
    GenericNvsFlash nvsRmt(tag, space, NVS_READONLY);
    return nvsRmt.GetU8(key, ret);
}
std::string readNvsFlashString(std::string tag, std::string space, std::string key, esp_err_t *ret)
{
    /* Open NvsFlash namespace for read operations */
    GenericNvsFlash nvsRmt(tag, space, NVS_READONLY);
    return nvsRmt.GetStr(key, ret);
}

// read data from NvsFlash
void readDataFromNvsFlash()
{
    esp_err_t ret;
    state = (bool) readNvsFlashU8(std::string("nvsRmt"), std::string("rmt"), std::string("state"), &ret);
    ESP_LOGI(tag, "state = %d", state);
    yamahaReceiver = (bool) readNvsFlashU8(std::string("nvsRmt"), std::string("rmt"), std::string("yamahaReceiver"), &ret);
    ESP_LOGI(tag, "yamahaReceiver = %d", yamahaReceiver);
    panasonicTv = (bool) readNvsFlashU8(std::string("nvsRmt"), std::string("rmt"), std::string("panasonicTv"), &ret);
    ESP_LOGI(tag, "panasonicTv = %d", panasonicTv);
    pioneerDvd = (bool) readNvsFlashU8(std::string("nvsRmt"), std::string("rmt"), std::string("pioneerDvd"), &ret);
    ESP_LOGI(tag, "pioneerDvd = %d", pioneerDvd);
    std::string activeScene = readNvsFlashString(std::string("nvsRmt"), std::string("rmt"), std::string("activeScene"), &ret);
    ESP_LOGI(tag, "activeScene = %s", activeScene.c_str());
}

// update value of key in NvsFlash
esp_err_t updateNvsFlash(std::string tag, std::string space, std::string key, uint8_t value)
{
    GenericNvsFlash nvsRmt(tag, space, NVS_READWRITE);
    return nvsRmt.SetU8(key, value);
}

// update value of key in NvsFlash
esp_err_t updateNvsFlash(std::string tag, std::string space, std::string key, std::string value)
{
    GenericNvsFlash nvsRmt(tag, space, NVS_READWRITE);
    return nvsRmt.SetStr(key, value);
}

// update data in NvsFlash
void updateDataInNvsFlash() {
    updateNvsFlash(std::string("nvsRmt"), std::string("rmt"), std::string("state"), state);
    ESP_LOGI(tag, "state = %d", state);
    updateNvsFlash(std::string("nvsRmt"), std::string("rmt"), std::string("yamahaReceiver"), yamahaReceiver);
    ESP_LOGI(tag, "yamahaReceiver = %d", yamahaReceiver);
    updateNvsFlash(std::string("nvsRmt"), std::string("rmt"), std::string("panasonicTv"), panasonicTv);
    ESP_LOGI(tag, "panasonicTvr = %d", panasonicTv);
    updateNvsFlash(std::string("nvsRmt"), std::string("rmt"), std::string("pioneerDvd"), pioneerDvd);
    ESP_LOGI(tag, "pioneerDvd = %d", pioneerDvd);
    updateNvsFlash(std::string("nvsRmt"), std::string("rmt"), std::string("activeScene"), activeScene);
    ESP_LOGI(tag, "activeScene = %s", activeScene.c_str());
}

// Callback function for BUTTON_SINGLE_CLICK event from onBoardButton
extern "C" void callback_onBoardButton_BUTTON_SINGLE_CLICK(void *arg, void *data)
{
    ESP_LOGI("onBoardButton Callback", "for Event BUTTON_SINGLE_CLICK called!");

    iot_button_print_event((button_handle_t)arg);

    // BUTTON_SINGLE_CLICK: switch on Panasonic TV and YAMAHA receiver for watching TV
    state = true;
    RmtIr* rmtIr = &rmtIr->getInstance(); // get the Singleton instance
    if (pioneerDvd == true) {
        // Pioneer DVD Player
        rmtIr->transmitPioneerCommandFrame((uint8_t)0xa3, (uint8_t)0x99, (uint8_t)0xaf, (uint8_t)0xbb); // "Shift + OFF"
        pioneerDvd = false;
        vTaskDelay(pdMS_TO_TICKS(500)); // delay 0.5 seconds
    }
    if (panasonicTv == false) {
        // Panasonic TV
        rmtIr->transmitPanasonicCommandFrame(0x4004, 0x01, 0x00, 0x7c); // "Power On"
        panasonicTv = true;
        vTaskDelay(pdMS_TO_TICKS(2000)); // delay 2 seconds
    }
    // YAMAHA Receiver
    rmtIr->transmitNecCommandFrame((uint16_t)0x7a85, (uint16_t)0x037c); // "TV Scene"
    yamahaReceiver = true;
    vTaskDelay(pdMS_TO_TICKS(500)); // delay 0.5 seconds
    // Panasonic TV
    rmtIr->transmitPanasonicCommandFrame(0x4004, 0x01, 0x40, 0x0c); // "TV"
    activeScene = "TV";

    updateDataInNvsFlash();
}

// Callback function for BUTTON_DOUBLE_CLICK event from onBoardButton
extern "C" void callback_onBoardButton_BUTTON_DOUBLE_CLICK(void *arg, void *data)
{
    ESP_LOGI("onBoardButton Callback", "for Event BUTTON_DOUBLE_CLICK called!");

    iot_button_print_event((button_handle_t)arg);

    // BUTTON_DOUBLE_CLICK: switch on Panasonic TV and YAMAHA receiver for AppleTV
    state = true;
    ESP_LOGI(tag, "state = %d", state);
    RmtIr* rmtIr = &rmtIr->getInstance(); // get the Singleton instance

    if (pioneerDvd == true) {
        // Pioneer DVD Player
        rmtIr->transmitPioneerCommandFrame((uint8_t)0xa3, (uint8_t)0x99, (uint8_t)0xaf, (uint8_t)0xbb); // "Shift + OFF"
        pioneerDvd = false;
        vTaskDelay(pdMS_TO_TICKS(500)); // delay 0.5 seconds
    }
    if (panasonicTv == false) {
        // Panasonic TV
        rmtIr->transmitPanasonicCommandFrame(0x4004, 0x01, 0x00, 0x7c); // "Power On"
        panasonicTv = true;
        vTaskDelay(pdMS_TO_TICKS(2000)); // delay 2 seconds
    }
    // YAMAHA Receiver
    rmtIr->transmitNecCommandFrame((uint16_t)0x7a85, (uint16_t)0x0976); // "Radio Scene" (AppleTV)
    yamahaReceiver = true;
    vTaskDelay(pdMS_TO_TICKS(500)); // delay 0.5 seconds
    // Panasonic TV
    rmtIr->transmitPanasonicCommandFrame(0x4004, 0x01, 0x20, 0x0d); // "HDMI1" (direct)
    activeScene = "AppleTV";

    updateDataInNvsFlash();
}

// Callback function for BUTTON_MULTIPLE_CLICK_3 event from onBoardButton
extern "C" void callback_onBoardButton_BUTTON_MULTIPLE_CLICK_3(void *arg, void *data)
{
    ESP_LOGI("onBoardButton Callback", "for Event BUTTON_MULTIPLE_CLICK_3 called!");

    iot_button_print_event((button_handle_t)arg);

    // BUTTON_MULTIPLE_CLICK_3: switch on Pioneer DVD Player, Panasonic TV and YAMAHA Receiver for watching a DVD
    state = true;
    ESP_LOGI(tag, "state = %d", state);
    RmtIr* rmtIr = &rmtIr->getInstance(); // get the Singleton instance

    if (panasonicTv == false) {
        // Panasonic TV
        rmtIr->transmitPanasonicCommandFrame(0x4004, 0x01, 0x00, 0x7c); // "Power On"
        panasonicTv = true;
        vTaskDelay(pdMS_TO_TICKS(1000)); // delay 1 seconds
    }
    if(pioneerDvd == false) {
        // Pioneer DVD Player
        rmtIr->transmitPioneerCommandFrame((uint8_t)0xa3, (uint8_t)0x99, (uint8_t)0xaf, (uint8_t)0xba); // "ON"
        pioneerDvd = true;
        vTaskDelay(pdMS_TO_TICKS(1000)); // delay 1 seconds
    }
    // YAMAHA Receiver
    rmtIr->transmitNecCommandFrame((uint16_t)0x7a85, (uint16_t)0x007f); // "BD/DVD Scene"
    yamahaReceiver = true;
    vTaskDelay(pdMS_TO_TICKS(500)); // delay 0.5 seconds
    // Pioneer DVD Player
    rmtIr->transmitPioneerCommandFrame((uint8_t)0xa3, (uint8_t)0x99, (uint8_t)0xaf, (uint8_t)0xb6); // "OPEN/CLOSE"
    vTaskDelay(pdMS_TO_TICKS(500)); // delay 0.5 seconds
    // Panasonic TV
    rmtIr->transmitPanasonicCommandFrame(0x4004, 0x01, 0x00, 0x40); // "AV2" (direct)
    activeScene = "DVD";

    updateDataInNvsFlash();
}

// Callback function for BUTTON_LONG_PRESS_UP events from onBoardButton
extern "C" void callback_onBoardButton_BUTTON_LONG_PRESS_UP(void *arg, void *data)
{
    ESP_LOGI("onBoardButton Callback", "for Event BUTTON_LONG_PRESS_UP called!");

    iot_button_print_event((button_handle_t)arg);
    uint32_t pressedTime = iot_button_get_pressed_time((button_handle_t)arg);
    if (pressedTime > 2000) return;

    // BUTTON_LONG_PRESS_UP_1000 -> select Tuner

    // BUTTON_LONG_PRESS_UP with pressedTime 1000 ms: switch on Tuner
    state = true;
    ESP_LOGI(tag, "state = %d", state);
    RmtIr* rmtIr = &rmtIr->getInstance(); // get the Singleton instance

    if (yamahaReceiver == false) {
        // YAMAHA Receiver
        rmtIr->transmitNecCommandFrame((uint8_t)0x7a, (uint8_t)0x1d); // "POWER ON"
        yamahaReceiver = true;
        vTaskDelay(pdMS_TO_TICKS(500)); // delay 0.5 seconds
    }
    if (pioneerDvd == true) {
        // Pioneer DVD Player
        rmtIr->transmitPioneerCommandFrame((uint8_t)0xa3, (uint8_t)0x99, (uint8_t)0xaf, (uint8_t)0xbb); // "Shift + OFF"
        pioneerDvd = false;
        vTaskDelay(pdMS_TO_TICKS(500)); // delay 0.5 seconds
    }
    if (panasonicTv == true) {
        // Panasonic TV
        rmtIr->transmitPanasonicCommandFrame(0x4004, 0x01, 0x00, 0xfc); // "Power Off"
        panasonicTv = false;
        vTaskDelay(pdMS_TO_TICKS(500)); // delay 0.5 seconds
    }
    // YAMAHA Receiver
    rmtIr->transmitNecCommandFrame((uint8_t)0x7a, (uint8_t)0x16); // "Tuner"
    activeScene = "Radio";

    updateDataInNvsFlash();
}

// Callback function for BUTTON_LONG_PRESS_UP event from onBoardButton
extern "C" void callback_onBoardButton_BUTTON_LONG_PRESS_START_3000(void *arg, void *data)
{
    ESP_LOGI("onBoardButton Callback", "for Event BUTTON_LONG_PRESS_START_3000 called!");

    iot_button_print_event((button_handle_t)arg);

    // BUTTON_LONG_PRESS_START_3000: everything is switched off
    state = false;
    ESP_LOGI(tag, "state = %d", state);
    RmtIr* rmtIr = &rmtIr->getInstance(); // get the Singleton instance

    if (yamahaReceiver == true) {
        // YAMAHA Receiver
        rmtIr->transmitNecCommandFrame((uint8_t)0x7a, (uint8_t)0x1e); // "STANDBY"
        yamahaReceiver = false;
        vTaskDelay(pdMS_TO_TICKS(500)); // delay 0.5 seconds
    }
    if (panasonicTv == true) {
        // Panasonic TV
        rmtIr->transmitPanasonicCommandFrame(0x4004, 0x01, 0x00, 0xfc); // "Power Off"
        panasonicTv = false;
        vTaskDelay(pdMS_TO_TICKS(500)); // delay 0.5 seconds
    }
    if (pioneerDvd == true) {
        // Pioneer DVD Player
        rmtIr->transmitPioneerCommandFrame((uint8_t)0xa3, (uint8_t)0x99, (uint8_t)0xaf, (uint8_t)0xbb); // "Shift + OFF"
        pioneerDvd = false;
    }
    activeScene = "OFF";
    state = false;

    updateDataInNvsFlash();
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
    button_event_args_t press_time_1000 = {
       { // long_press
           1000, // press_time
       }
    };
    onBoardButton.RegisterCallbackForEvent(BUTTON_LONG_PRESS_UP, &press_time_1000, callback_onBoardButton_BUTTON_LONG_PRESS_UP);
    button_event_args_t press_time_3000 = {
       { // long_press
           3000, // press_time
       }
    };
    onBoardButton.RegisterCallbackForEvent(BUTTON_LONG_PRESS_START, &press_time_3000, callback_onBoardButton_BUTTON_LONG_PRESS_START_3000);
    button_event_args_t clicks = {
       { // multiple_clicks
           3, // clicks
       }
    };
    onBoardButton.RegisterCallbackForEvent(BUTTON_MULTIPLE_CLICK, &clicks, callback_onBoardButton_BUTTON_MULTIPLE_CLICK_3);

    // read data from nvs_flash
    readDataFromNvsFlash();

    // receiver test
    while(1) {
        //ESP_LOGI(tag, "receiveNecOrPanasonicFrame");
        //rmtIr->receiveNecOrPanasonicFrame();
        vTaskDelay(pdMS_TO_TICKS(30000)); // delay 30 seconds
    }

}
