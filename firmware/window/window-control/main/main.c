#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "mqtt_client.h"

#define CONFIG_BLINK_PERIOD 1000

//ESP32 controls linear actuator to open and close the window

static const char* TAG = "main";
static gpio_num_t LED_PIN = 2;
static uint32_t LED_state = 0;

void configure_led(void) {
    ESP_LOGI(TAG, "configuring LED...");
    gpio_reset_pin(LED_PIN);
    gpio_mode_t mode = GPIO_MODE_OUTPUT;
    gpio_set_direction(LED_PIN, mode);
}

static void blink_led(void) {
    gpio_set_level(LED_PIN, LED_state);
}

void app_main(void)
{
    configure_led();

    while(1) {
        ESP_LOGI(TAG, "Turning the LED %s", LED_state == true ? "ON": "OFF");
        blink_led();
        LED_state = !LED_state;
        vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
    }
}