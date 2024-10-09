#include "ntp_handler.h"

NtpHandler::NtpHandler(IPAddress serverIp, u16_t updateInterval, u32_t timezoneOffset) : 
    ntpClient(ntpConnection, serverIp, 0, updateInterval),
    timezoneOffset(timezoneOffset)
{
}
bool NtpHandler::begin(u32_t timeoutMillis) {
    Stopwatch32MS watch;
    ntpClient.begin();
    ESP_LOGI(NTP_LOG_TAG, "Getting time ...");
    watch.restart();
    for(;;) {
        if(ntpClient.forceUpdate()) {
            break;
        }
        if(watch.getTimeSinceStart() >= timeoutMillis) {
            ESP_LOGW(NTP_LOG_TAG, "Getting time failed, due to timeout!");
            return false;
        }
    }
    ESP_LOGI(NTP_LOG_TAG, "Got time from server: %s", ntpClient.getFormattedTime());
    return true;
}
void NtpHandler::update() {
    ntpClient.update();
}
u32_t NtpHandler::getEpochTime() const {
    return ntpClient.getEpochTime();
}
u64_t NtpHandler::getEpochTimeMillis() const {
    return ntpClient.getEpochTimeMillis();
}
String NtpHandler::getFormattedTime() const {
    unsigned long rawTime = getEpochTime() + timezoneOffset;
    unsigned long milliseconds = millis() % 1000;

    // Convert epoch time to components
    struct tm *ptm = gmtime((time_t *)&rawTime);
    char timeString[25];
    sprintf(timeString, "%04d-%02d-%02dT%02d:%02d:%02d.%03luZ",
            ptm->tm_year + 1900,
            ptm->tm_mon + 1,
            ptm->tm_mday,
            ptm->tm_hour,
            ptm->tm_min,
            ptm->tm_sec,
            milliseconds);

    return String(timeString);
}

