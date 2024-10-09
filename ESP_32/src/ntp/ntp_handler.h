#pragma once
#include <Arduino.h>
#include <WiFiUdp.h>
#include <arduino_stopwatch.h>
#include "NTPClient.h"
#include "esp_log.h"

using ArduinoStopwatch::Stopwatch32MS;

class NtpHandler {
public:
    static const u32_t DEFAULT_UPDATE_INTERVAL_MILLIS = 60000;
public:
    NtpHandler(IPAddress serverIp, u16_t updateInterval=DEFAULT_UPDATE_INTERVAL_MILLIS, u32_t timezoneOffset=0);
    bool begin(u32_t timeoutMillis=2000);
    void update();
    u32_t getEpochTime() const;
    u64_t getEpochTimeMillis() const;
    String getFormattedTime() const;
private:
    WiFiUDP ntpConnection;
    NTPClient ntpClient;
    u32_t timezoneOffset;
    const char* NTP_LOG_TAG = "NTP-Handler";
};