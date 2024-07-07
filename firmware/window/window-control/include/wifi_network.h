#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "freertos/event_groups.h"

#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#pragma once

namespace Window{
    class WiFi {
    public:
    void connect();


    private:

    };
}
