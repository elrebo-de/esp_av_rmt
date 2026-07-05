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

// Callback function for BUTTON_SINGLE_CLICK event from adcButtons
extern "C" void callback_onBoardButton_BUTTON_SINGLE_CLICK(void *arg, void *data)
{
    button_adc_config_t *btn_adc_cfg = (button_adc_config_t *)data;
    uint8_t buttonIndex = btn_adc_cfg->button_index;

    ESP_LOGI("adcButtons Callback", "for Event BUTTON_SINGLE_CLICK called from button_index %u", buttonIndex);

    AvRmt *avRmt = &AvRmt::getInstance();

    switch(buttonIndex) {
        case 4:
            // buttonIndex == 4: switch on Panasonic TV and YAMAHA receiver for watching TV
            onBoardLed->setLedPixelColor(0, 16, 0, 0); // pixel 0, color red, intensity 16/256
            onBoardLed->setLedState(1);
            onBoardLed->show();

            avRmt->switchOnTv();
            break;
        case 3:
            // buttonIndex == 3: switch on Panasonic TV and YAMAHA receiver for AppleTV
            onBoardLed->setLedPixelColor(0, 16, 16, 0); // pixel 0, color yellow, intensity 16/256
            onBoardLed->setLedState(1);
            onBoardLed->show();

            avRmt->switchOnAppleTv();
            break;
        case 2:
            // buttonIndex == 2: switch on Pioneer DVD Player, Panasonic TV and YAMAHA Receiver for watching a DVD
            onBoardLed->setLedPixelColor(0, 0, 16, 0); // pixel 0, color green, intensity 16/256
            onBoardLed->setLedState(1);
            onBoardLed->show();

            avRmt->switchOnDvd();
            break;
        case 5:
            // buttonIndex == 5: switch on Tuner
            onBoardLed->setLedPixelColor(0, 0, 0, 16); // pixel 0, color blue, intensity 16/256
            onBoardLed->setLedState(1);
            onBoardLed->show();

            avRmt->switchOnRadio();
            break;
        case 1:
            // buttonIndex == 1: everything is switched off
            onBoardLed->setLedPixelColor(0, 16,16, 16); // pixel 0, color white, intensity 16/256
            onBoardLed->setLedState(1);
            onBoardLed->show();

            avRmt->switchAllOff();
            break;
        case 0:
            // buttonIndex == 0: switch on YAMAHA receiver for DUAL record player
            onBoardLed->setLedPixelColor(0, 0, 16, 16); // pixel 0, color pink, intensity 16/256
            onBoardLed->setLedState(1);
            onBoardLed->show();

            avRmt->switchOnRecordPlayer();
            break;
    }
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

    /* ESP32C3 Waveshare Zero */
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
    // set ADC button configuration
    adc_oneshot_unit_handle_t *adc_handle = NULL; // will be set in first ADC button constructor

    // adcButton0
    button_adc_config_t btn_adc_cfg_0 = {
        .adc_handle = adc_handle,           /**< handle of adc unit, if NULL will create new one internal, else will use the handle */
        .unit_id = ADC_UNIT_1,               /**< ADC unit */
        .adc_channel = ADC1_GPIO0_CHANNEL,   /**< Channel of ADC */
        .button_index = 0,                   /**< button index on the channel */
        .min = 0,                            /**< min voltage in mv corresponding to the button */
        .max = 154,                          /**< max voltage in mv corresponding to the button */
    };

    // adcButton1
    button_adc_config_t btn_adc_cfg_1 = {
        .adc_handle = adc_handle,           /**< handle of adc unit, if NULL will create new one internal, else will use the handle */
        .unit_id = ADC_UNIT_1,               /**< ADC unit */
        .adc_channel = ADC1_GPIO0_CHANNEL,   /**< Channel of ADC */
        .button_index = 1,                   /**< button index on the channel */
        .min = 155,                          /**< min voltage in mv corresponding to the button */
        .max = 477,                          /**< max voltage in mv corresponding to the button */
    };

    // adcButton2
    button_adc_config_t btn_adc_cfg_2 = {
        .adc_handle = adc_handle,           /**< handle of adc unit, if NULL will create new one internal, else will use the handle */
        .unit_id = ADC_UNIT_1,               /**< ADC unit */
        .adc_channel = ADC1_GPIO0_CHANNEL,   /**< Channel of ADC */
        .button_index = 2,                   /**< button index on the channel */
        .min = 478,                          /**< min voltage in mv corresponding to the button */
        .max = 852,                         /**< max voltage in mv corresponding to the button */
    };

    // adcButton3
    button_adc_config_t btn_adc_cfg_3 = {
        .adc_handle = adc_handle,           /**< handle of adc unit, if NULL will create new one internal, else will use the handle */
        .unit_id = ADC_UNIT_1,               /**< ADC unit */
        .adc_channel = ADC1_GPIO0_CHANNEL,   /**< Channel of ADC */
        .button_index = 3,                   /**< button index on the channel */
        .min = 853,                         /**< min voltage in mv corresponding to the button */
        .max = 1323,                         /**< max voltage in mv corresponding to the button */
    };

    // adcButton4
    button_adc_config_t btn_adc_cfg_4 = {
        .adc_handle = adc_handle,           /**< handle of adc unit, if NULL will create new one internal, else will use the handle */
        .unit_id = ADC_UNIT_1,               /**< ADC unit */
        .adc_channel = ADC1_GPIO0_CHANNEL,   /**< Channel of ADC */
        .button_index = 4,                   /**< button index on the channel */
        .min = 1324,                         /**< min voltage in mv corresponding to the button */
        .max = 1964,                         /**< max voltage in mv corresponding to the button */
    };

    // adcButton5
    button_adc_config_t btn_adc_cfg_5 = {
        .adc_handle = adc_handle,           /**< handle of adc unit, if NULL will create new one internal, else will use the handle */
        .unit_id = ADC_UNIT_1,               /**< ADC unit */
        .adc_channel = ADC1_GPIO0_CHANNEL,   /**< Channel of ADC */
        .button_index = 5,                   /**< button index on the channel */
        .min = 1965,                         /**< min voltage in mv corresponding to the button */
        .max = 2612,                         /**< max voltage in mv corresponding to the button */
    };

    GenericButton adcButton0(std::string("adcButton0"), &btn_adc_cfg_0);
    adcButton0.RegisterCallbackForEvent(BUTTON_SINGLE_CLICK, callback_onBoardButton_BUTTON_SINGLE_CLICK, (void *)&btn_adc_cfg_0);

    GenericButton adcButton1(std::string("adcButton1"), &btn_adc_cfg_1);
    adcButton1.RegisterCallbackForEvent(BUTTON_SINGLE_CLICK, callback_onBoardButton_BUTTON_SINGLE_CLICK, (void *)&btn_adc_cfg_1);

    GenericButton adcButton2(std::string("adcButton2"), &btn_adc_cfg_2);
    adcButton2.RegisterCallbackForEvent(BUTTON_SINGLE_CLICK, callback_onBoardButton_BUTTON_SINGLE_CLICK, (void *)&btn_adc_cfg_2);

    GenericButton adcButton3(std::string("adcButton3"), &btn_adc_cfg_3);
    adcButton3.RegisterCallbackForEvent(BUTTON_SINGLE_CLICK, callback_onBoardButton_BUTTON_SINGLE_CLICK, (void *)&btn_adc_cfg_3);

    GenericButton adcButton4(std::string("adcButton4"), &btn_adc_cfg_4);
    adcButton4.RegisterCallbackForEvent(BUTTON_SINGLE_CLICK, callback_onBoardButton_BUTTON_SINGLE_CLICK, (void *)&btn_adc_cfg_4);

    GenericButton adcButton5(std::string("adcButton5"), &btn_adc_cfg_5);
    adcButton5.RegisterCallbackForEvent(BUTTON_SINGLE_CLICK, callback_onBoardButton_BUTTON_SINGLE_CLICK, (void *)&btn_adc_cfg_5);

    ESP_LOGI(tag, "Initialize RmtIr class");
    RmtIr* rmtIr = &rmtIr->getInstance(); // get the Singleton instance
    //rmtIr->setGpioPins(32,0); // set the GPIO pins for M5 ATOM LITE
    //rmtIr->setGpioPins(10,0); // set the GPIO pins for ESP32C3 Supermini
    rmtIr->setGpioPins(3,0); // set the GPIO pins for ESP32C3 Waveshare Zero
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
    vTaskDelay(pdMS_TO_TICKS(30000)); // delay 30 seconds
    onBoardLed->setLedState(0);
    onBoardLed->show();


    bool rc = false;

    ESP_LOGI(tag, "EnableGpioWakeup");
    ESP_ERROR_CHECK(deepSleep.EnableAdcWakeup((gpio_num_t) 0, 0));  // enable wake up when ADC GPIO 0 is pulled down

    ESP_LOGI(tag, "GoToDeepSleep");
    rc = deepSleep.GoToDeepSleep(); // go to deep sleep

    // this statement will not be reached, if GoToDeepSleep is working
    ESP_LOGI(tag, "GoToDeepSleep rc=%u", rc);
}
