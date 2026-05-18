/* Example program using
   * elrebo-de/av_rmt to remote control
     * a YAMAHA Receiver,
     * a Panasonic TV and
     * a Pioneer DVD Player
 */

#include <string>
#include "esp_log.h"

#include "av_rmt.hpp"

const char *tag = "ESP AV RMT";

// Callback function for BUTTON_SINGLE_CLICK event from onBoardButton
extern "C" void callback_onBoardButton_BUTTON_SINGLE_CLICK(void *arg, void *data)
{
    ESP_LOGI("onBoardButton Callback", "for Event BUTTON_SINGLE_CLICK called!");

    iot_button_print_event((button_handle_t)arg);

    // BUTTON_SINGLE_CLICK: switch on Panasonic TV and YAMAHA receiver for watching TV
    AvRmt *avRmt = &AvRmt::getInstance();
    avRmt->switchOnTv();
}

// Callback function for BUTTON_DOUBLE_CLICK event from onBoardButton
extern "C" void callback_onBoardButton_BUTTON_DOUBLE_CLICK(void *arg, void *data)
{
    ESP_LOGI("onBoardButton Callback", "for Event BUTTON_DOUBLE_CLICK called!");

    iot_button_print_event((button_handle_t)arg);

    // BUTTON_DOUBLE_CLICK: switch on Panasonic TV and YAMAHA receiver for AppleTV
    AvRmt *avRmt = &AvRmt::getInstance();
    avRmt->switchOnAppleTv();
}

// Callback function for BUTTON_MULTIPLE_CLICK_3 event from onBoardButton
extern "C" void callback_onBoardButton_BUTTON_MULTIPLE_CLICK_3(void *arg, void *data)
{
    ESP_LOGI("onBoardButton Callback", "for Event BUTTON_MULTIPLE_CLICK_3 called!");

    iot_button_print_event((button_handle_t)arg);

    // BUTTON_MULTIPLE_CLICK_3: switch on Pioneer DVD Player, Panasonic TV and YAMAHA Receiver for watching a DVD
    AvRmt *avRmt = &AvRmt::getInstance();
    avRmt->switchOnDvd();
}

// Callback function for BUTTON_LONG_PRESS_UP events from onBoardButton
extern "C" void callback_onBoardButton_BUTTON_LONG_PRESS_UP(void *arg, void *data)
{
    ESP_LOGI("onBoardButton Callback", "for Event BUTTON_LONG_PRESS_UP called!");

    iot_button_print_event((button_handle_t)arg);
    uint32_t pressedTime = iot_button_get_pressed_time((button_handle_t)arg);
    if (pressedTime >= 2500) return;

    // BUTTON_LONG_PRESS_UP_1000 -> select Tuner

    // BUTTON_LONG_PRESS_UP with pressedTime 1000 ms: switch on Tuner
    AvRmt *avRmt = &AvRmt::getInstance();
    avRmt->switchOnRadio();
}

// Callback function for BUTTON_LONG_PRESS_UP event from onBoardButton
extern "C" void callback_onBoardButton_BUTTON_LONG_PRESS_START_3000(void *arg, void *data)
{
    ESP_LOGI("onBoardButton Callback", "for Event BUTTON_LONG_PRESS_START_3000 called!");

    iot_button_print_event((button_handle_t)arg);

    // BUTTON_LONG_PRESS_START_3000: everything is switched off
    AvRmt *avRmt = &AvRmt::getInstance();
    avRmt->switchAllOff();
}

extern "C" void app_main(void)
{
    // short delay to reconnect logging
    vTaskDelay(pdMS_TO_TICKS(500)); // delay 0.5 seconds

    ESP_LOGI(tag, "Program Start");

    ESP_LOGI(tag, "Initialize GenericButton class");
    GenericButton* onBoardButton = new GenericButton(
	    std::string("onBoardButton"),
	    (gpio_num_t) 39, // GPIO for M5 Atom Lite
	    //(gpio_num_t) 9, // GPIO for ESP32C3 Supermini
	    0, // active = DOWN
	    true, // pull disabled - M5 Atom does not support internal PU/PD on this gpio
	    std::string("GPIO")
	);

    // register Button Callbacks
    onBoardButton->RegisterCallbackForEvent(BUTTON_SINGLE_CLICK, callback_onBoardButton_BUTTON_SINGLE_CLICK);
    onBoardButton->RegisterCallbackForEvent(BUTTON_DOUBLE_CLICK, callback_onBoardButton_BUTTON_DOUBLE_CLICK);
    button_event_args_t press_time_1000 = {
       { // long_press
           1000, // press_time
       }
    };
    onBoardButton->RegisterCallbackForEvent(BUTTON_LONG_PRESS_UP, &press_time_1000, callback_onBoardButton_BUTTON_LONG_PRESS_UP);
    button_event_args_t press_time_3000 = {
       { // long_press
           3000, // press_time
       }
    };
    onBoardButton->RegisterCallbackForEvent(BUTTON_LONG_PRESS_START, &press_time_3000, callback_onBoardButton_BUTTON_LONG_PRESS_START_3000);
    button_event_args_t clicks = {
       { // multiple_clicks
           3, // clicks
       }
    };
    onBoardButton->RegisterCallbackForEvent(BUTTON_MULTIPLE_CLICK, &clicks, callback_onBoardButton_BUTTON_MULTIPLE_CLICK_3);

    ESP_LOGI(tag, "Initialize RmtIr class");
    RmtIr* rmtIr = &rmtIr->getInstance(); // get the Singleton instance
    //rmtIr->setGpioPins(4,0); // set the GPIO pins for ESP32C3 Supermini
    rmtIr->setGpioPins(32,0); // set the GPIO pins for M5 ATOM LITE
    rmtIr->initialize(); // initialize RMT IR

    ESP_LOGI(tag, "Initialize AvRmt class");
    AvRmt* avRmt = &avRmt->getInstance(); // get the Singleton instance

    avRmt->initialize(rmtIr);

    ESP_LOGI(tag, "Endless Loop");
    while(1) {
        vTaskDelay(pdMS_TO_TICKS(30000)); // delay 30 seconds
    }

}
