#include "wifi_handler.h"
#include <arduino_stopwatch.h>

using ArduinoStopwatch::Stopwatch32MS;

WiFiHandler::WiFiHandler(const char* hostname) :
    hostname(hostname)
{
}
bool WiFiHandler::connect(const char* ssid, const char* password, u32_t timeoutMillis) {
    Stopwatch32MS watch;

    s16_t count = WiFi.scanNetworks();
    ESP_LOGI(WIFI_LOG_TAG, "Scanning Networks...");
    for (s16_t i=0; i < count; i++) {
        wifi_ap_record_t* record = (wifi_ap_record_t*)WiFi.getScanInfoByIndex(i);
        ESP_LOGI(WIFI_LOG_TAG, "%i: %s", i, (unsigned char*)(record->ssid));
    }

    WiFi.setHostname(hostname);
    WiFi.begin(ssid, password);
    ESP_LOGI(WIFI_LOG_TAG, "Connecting to %s as %s", ssid, hostname);
    watch.restart();
    for(;;) {
        wl_status_t status = WiFi.status();
        if(status == wl_status_t::WL_CONNECTED) {
            break;
        } else if(status == wl_status_t::WL_CONNECT_FAILED) {
            WiFi.reconnect();
        }
        if(watch.getTimeSinceStart() >= timeoutMillis) {
            ESP_LOGE(WIFI_LOG_TAG, "Connecting failed, due to timeout!");
            return false;
        }
    }
    ESP_LOGI(WIFI_LOG_TAG, "Connected to %s with the IPAddress %s", ssid, WiFi.localIP().toString().c_str());
    return true;
}

bool WiFiHandler::connect(const char* ssid, const char* password, NetworkConfig config, u32_t timeoutMillis) {
    WiFi.config(config.localIp, config.gateway, config.subnet, config.dns1, config.dns2);
    return connect(ssid, password, timeoutMillis);
}