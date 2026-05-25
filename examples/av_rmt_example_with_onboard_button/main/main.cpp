/* Example program using
   * elrebo-de/av_rmt to remote control
     * a YAMAHA Receiver,
     * a Panasonic TV and
     * a Pioneer DVD Player
 */

#include <string>
#include "esp_log.h"

#include "av_rmt.hpp"
#include "onboard_led.hpp"
#include "generic_button.hpp"
#include "deep_sleep.hpp"
RTC_DATA_ATTR int bootCount = 0;

const char *tag = "ESP AV RMT";

// flag to signal to go to DeepSleep
bool finished = false;

// OnBoardLed
OnBoardLed* onBoardLed = NULL;

// Callback function for BUTTON_SINGLE_CLICK event from onBoardButton
extern "C" void callback_onBoardButton_BUTTON_SINGLE_CLICK(void *arg, void *data)
{
    ESP_LOGI("onBoardButton Callback", "for Event BUTTON_SINGLE_CLICK called!");

    iot_button_print_event((button_handle_t)arg);

    // BUTTON_SINGLE_CLICK: switch on Panasonic TV and YAMAHA receiver for watching TV
    onBoardLed->setLedPixelColor(0, 0, 16, 0); // pixel 0, color green, intensity 16/256
    onBoardLed->setLedState(1);
    onBoardLed->show();

    AvRmt *avRmt = &AvRmt::getInstance();
    avRmt->switchOnTv();

    finished = true;
}

// Callback function for BUTTON_DOUBLE_CLICK event from onBoardButton
extern "C" void callback_onBoardButton_BUTTON_DOUBLE_CLICK(void *arg, void *data)
{
    ESP_LOGI("onBoardButton Callback", "for Event BUTTON_DOUBLE_CLICK called!");

    iot_button_print_event((button_handle_t)arg);

    // BUTTON_DOUBLE_CLICK: switch on Panasonic TV and YAMAHA receiver for AppleTV
    onBoardLed->setLedPixelColor(0, 0, 0, 16); // pixel 0, color blue, intensity 16/256
    onBoardLed->setLedState(1);
    onBoardLed->show();

    AvRmt *avRmt = &AvRmt::getInstance();
    avRmt->switchOnAppleTv();

    finished = true;
}

// Callback function for BUTTON_MULTIPLE_CLICK_3 event from onBoardButton
extern "C" void callback_onBoardButton_BUTTON_MULTIPLE_CLICK_3(void *arg, void *data)
{
    ESP_LOGI("onBoardButton Callback", "for Event BUTTON_MULTIPLE_CLICK_3 called!");

    iot_button_print_event((button_handle_t)arg);

    // BUTTON_MULTIPLE_CLICK_3: switch on Pioneer DVD Player, Panasonic TV and YAMAHA Receiver for watching a DVD
    onBoardLed->setLedPixelColor(0, 16, 16, 0); // pixel 0, color yellow, intensity 16/256
    onBoardLed->setLedState(1);
    onBoardLed->show();

    AvRmt *avRmt = &AvRmt::getInstance();
    avRmt->switchOnDvd();

    finished = true;
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
    onBoardLed->setLedPixelColor(0, 16, 0, 16); // pixel 0, color pink, intensity 16/256
    onBoardLed->setLedState(1);
    onBoardLed->show();

    AvRmt *avRmt = &AvRmt::getInstance();
    avRmt->switchOnRadio();

    finished = true;
}

// Callback function for BUTTON_LONG_PRESS_UP event from onBoardButton
extern "C" void callback_onBoardButton_BUTTON_LONG_PRESS_START_3000(void *arg, void *data)
{
    ESP_LOGI("onBoardButton Callback", "for Event BUTTON_LONG_PRESS_START_3000 called!");

    iot_button_print_event((button_handle_t)arg);

    // BUTTON_LONG_PRESS_START_3000: everything is switched off
    onBoardLed->setLedPixelColor(0, 16, 0, 0); // pixel 0, color red, intensity 16/256
    onBoardLed->setLedState(1);
    onBoardLed->show();

    AvRmt *avRmt = &AvRmt::getInstance();
    avRmt->switchAllOff();

    finished = true;
}

extern "C" void app_main(void)
{
    // short delay to reconnect logging
    //vTaskDelay(pdMS_TO_TICKS(500)); // delay 0.5 seconds

    ESP_LOGI(tag, "Program Start");

    ESP_LOGI(tag, "Initialize OnBoardLed class");

    /* M5 Atom Lite
    onBoardLed = new OnBoardLed(
		std::string("onBoardLed"),
		(gpio_num_t) 27,
		std::string("GRB"),
		std::string("RMT"),
		LED_MODEL_WS2812,
		500);
	// */

     /* ESP32-C3 Zero */
     onBoardLed = new OnBoardLed(
 		std::string("onBoardLed"),
 		(gpio_num_t) 10,
 		std::string("RGB"),
 		std::string("RMT"),
 		LED_MODEL_WS2812,
 		500);
 	// */

     /* ESP32C3 Supermini
    onBoardLed = new OnBoardLed(
		std::string("onBoardLed"),
	    (gpio_num_t) 8,
        (uint8_t) 0, // activeLevel
		500);
    // */

    onBoardLed->setLedPixelColor(0, 16, 16, 16); // pixel 0, color white, intensity 16/256
    onBoardLed->setLedState(1);
    onBoardLed->show();

    ESP_LOGI(tag, "Initialize GenericButton class");
    // set GPIO onBoard button configuration
    #if defined(CONFIG_IDF_TARGET_ESP32C6)
    /* ESP32-C6-DevKitM-1 V1.0 */
    button_gpio_config_t btn_gpio_cfg = {
        .gpio_num = 9,
        .active_level = 0,
        .enable_power_save = false,
        .disable_pull = false,
    };
    #elif defined(CONFIG_IDF_TARGET_ESP32C3)
    /* Waveshare ESP32-C3-Zero */
    button_gpio_config_t btn_gpio_cfg = {
        .gpio_num = 9,
        .active_level = 0,
        .enable_power_save = false,
        .disable_pull = false,
    };
    #elif defined(CONFIG_IDF_TARGET_ESP32)
    /* M5 Atom Lite */
    button_gpio_config_t btn_gpio_cfg = {
        .gpio_num = 39,
        .active_level = 0,
        .enable_power_save = false,
        .disable_pull = true,
    };
    #endif
    GenericButton* onBoardButton = new GenericButton(
	   std::string("onBoardButton"),
	   &btn_gpio_cfg
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
    //rmtIr->setGpioPins(32,0); // set the GPIO pins for M5 ATOM LITE
    rmtIr->setGpioPins(3,0); // set the GPIO pins for ESP32C3 Supermini
    rmtIr->initialize(); // initialize RMT IR

    ESP_LOGI(tag, "Initialize AvRmt class");
    AvRmt* avRmt = &avRmt->getInstance(); // get the Singleton instance

    avRmt->initialize(rmtIr);

    /* Initialize DeepSleep class */
    ESP_LOGI(tag, "Initialize DeepSleep class");
    DeepSleep deepSleep(
		std::string("DeepSleep"), // tag
		&bootCount // Address of int bootCount in RTC_DATA
    );

    ESP_LOGI(tag, "Loop until finished");
    while(!finished) {
        vTaskDelay(pdMS_TO_TICKS(1000)); // delay 1 seconds
    }

    // go to DeepSleep
    vTaskDelay(pdMS_TO_TICKS(1000)); // delay 1 seconds
    onBoardLed->setLedState(0);
    onBoardLed->show();


    bool rc = false;

    ESP_LOGI(tag, "EnableGpioWakeup");
    //ESP_ERROR_CHECK(deepSleep.EnableGpioWakeup((gpio_num_t) 39, 0));  // enable wake up when GPIO 39 is pulled down
    ESP_ERROR_CHECK(deepSleep.EnableGpioWakeup((gpio_num_t) 9, 0));  // enable wake up when GPIO 9 is pulled down

    ESP_LOGI(tag, "GoToDeepSleep");
    rc = deepSleep.GoToDeepSleep(); // go to deep sleep

    // this statement will not be reached, if GoToDeepSleep is working
    ESP_LOGI(tag, "GoToDeepSleep rc=%u", rc);
}
