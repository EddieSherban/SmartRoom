/******************************************************************************

    Project: Smart Room
    Description: This project contains functions for Smart Room tasks
                 such as window control.

    Author: Eddie Sherban
    Date: 2024-06-14

    Files:
    - main.cpp
    - motor_control.c
    - mqtt_network.c

******************************************************************************/

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "esp_attr.h"
#include "motor_control.h"
#include "mqtt_network.h"

#define CONFIG_BLINK_PERIOD 1000

static const char* TAG = "main";
static gpio_num_t LED_PIN = static_cast<gpio_num_t>(2);
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

void vBlinkTask(void *pvParameters) {
    while(1) {
        ESP_LOGI(TAG, "Turning the LED %s", LED_state == true ? "ON": "OFF");
        blink_led();
        LED_state = !LED_state;
        //vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
        vTaskDelay(pdMS_TO_TICKS(CONFIG_BLINK_PERIOD));
    }
}

void vSendMessage(void* pvParameters) {
    while(1) {
        ESP_LOGI(TAG, "Sending MQTT message...");
        MQTT::Client client;
        client.mqtt_connect("core-mosquitto", 1883);
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

extern "C" void app_main(void)
{
    configure_led();

    // LED blinking task
    xTaskCreate(vBlinkTask, "Blink Task", 4096, NULL, tskIDLE_PRIORITY, NULL);
    xTaskCreate(vSendMessage, "MQTT Task", 4096, NULL, tskIDLE_PRIORITY, NULL);
}