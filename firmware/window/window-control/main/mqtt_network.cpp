/******************************************************************************

    Description: This file contains code related to MQTT; the protocol ESP32
    will use to communicate with the Raspberry Pi Home Assistant OS.

    Author: Eddie Sherban
    Date: 2024-06-14

******************************************************************************/

#include "mqtt_network.h"
#include "wifi_network.h"
#include "secrets.h"

static const char* TAG = "mqtt_functionality";

static void log_error_if_nonzero(const char *message, int error_code)
{
    if (error_code != 0) {
        ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
    }
}

static void mqtt_event_handler(void* handler_args, esp_event_base_t base, int32_t event_id, void* event_data) {
    
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%" PRIi32 "", base, event_id);
    esp_mqtt_event_handle_t event = static_cast<esp_mqtt_event_handle_t>(event_data);
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        msg_id = esp_mqtt_client_publish(client, "/topic/qos1", "data_3", 0, 1, 0);
        ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);

        msg_id = esp_mqtt_client_subscribe(client, "/topic/qos0", 0);
        ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

        msg_id = esp_mqtt_client_subscribe(client, "/topic/qos1", 1);
        ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

        msg_id = esp_mqtt_client_unsubscribe(client, "/topic/qos1");
        ESP_LOGI(TAG, "sent unsubscribe successful, msg_id=%d", msg_id);
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;

    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        msg_id = esp_mqtt_client_publish(client, "/topic/qos0", "data", 0, 0, 0);
        ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        printf("DATA=%.*s\r\n", event->data_len, event->data);
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
            log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
            log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
            log_error_if_nonzero("captured as transport's socket errno",  event->error_handle->esp_transport_sock_errno);
            ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));

        }
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}

namespace Window{
namespace MQTT {
    Client::Client() {
        ESP_LOGI(TAG, "[APP] Startup..");
        ESP_LOGI(TAG, "[APP] Free memory: %" PRIu32 " bytes", esp_get_free_heap_size());
        ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());

        esp_log_level_set("*", ESP_LOG_INFO);
        esp_log_level_set("mqtt_client", ESP_LOG_VERBOSE);
        esp_log_level_set("mqtt_example", ESP_LOG_VERBOSE);
        esp_log_level_set("transport_base", ESP_LOG_VERBOSE);
        esp_log_level_set("esp-tls", ESP_LOG_VERBOSE);
        esp_log_level_set("transport", ESP_LOG_VERBOSE);
        esp_log_level_set("outbox", ESP_LOG_VERBOSE);

        ESP_ERROR_CHECK(nvs_flash_init());
        ESP_ERROR_CHECK(esp_netif_init());
        ESP_ERROR_CHECK(esp_event_loop_create_default());
        ESP_LOGI(TAG, "esp event loop created successfully...");

        Window::WiFi wifi;
        wifi.connect();
    }

    Client::~Client() {

    }

    void Client::mqtt_connect() {
        ESP_LOGI(TAG, "mqtt_connect()...");
        esp_mqtt_client_config_t mqtt_cfg;
        memset(&mqtt_cfg, 0, sizeof(esp_mqtt_client_config_t));

        //The folowing information is available in a "secrets.h" file that is not included in the repository, but locally
        mqtt_cfg.broker.address.uri = BROKER_URI;
        mqtt_cfg.broker.address.hostname = HOSTNAME;
        mqtt_cfg.broker.address.port = SERVER_PORT;
        mqtt_cfg.credentials.username = USERNAME;
        mqtt_cfg.credentials.client_id = CLIENT_ID;
        mqtt_cfg.credentials.authentication.password = PASSWORD;

        esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
        esp_mqtt_client_register_event(client, static_cast<esp_mqtt_event_id_t>(ESP_EVENT_ANY_ID), mqtt_event_handler, NULL);
        esp_mqtt_client_start(client);
    }
}    
}
