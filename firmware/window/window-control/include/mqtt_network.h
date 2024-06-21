//mqtt_network.h
//want to make esp32 communciate with raspberry pi
//wef

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "mqtt_client.h"
#include "esp_wifi.h"

#include "esp_log.h"
#include "esp_system.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_netif.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#pragma once

namespace MQTT {
    class Client {
    public:
        Client(); 
        ~Client();

        void mqtt_connect(const char* broker, int port); //connect
        void publish(const char* topic, const char* message); //publish
        void subscribe(const char* topic); //subscribe
        void disconnect(); //disconnect
        void onMessageReceived(void(*callback)(char*, uint8_t*, unsigned int)); //handle messages
        
    private:
        int myData; // Member variable
    };
}