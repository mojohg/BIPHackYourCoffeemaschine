#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include "esp_log.h"

class NetworkConfig {
public:
    NetworkConfig(IPAddress localIp, IPAddress gateway, IPAddress subnet, IPAddress dns1 = u32_t(0),  IPAddress dns2 = u32_t(0)) :
        localIp(localIp),
        gateway(gateway),
        subnet(subnet),
        dns1(dns1),
        dns2(dns2)
    {
    }
    IPAddress localIp;
    IPAddress gateway;
    IPAddress subnet;
    IPAddress dns1;
    IPAddress dns2;
};

class WiFiHandler {
public:
    WiFiHandler(const char* hostname);
    bool connect(const char* ssid, const char* password, u32_t timeoutMillis=2000);
    bool connect(const char* ssid, const char* password, NetworkConfig config, u32_t timeoutMillis=2000);
private:
    const char* hostname;
    const char* WIFI_LOG_TAG = "WIFI-Handler";
};