#pragma once
#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <arduino_stopwatch.h>
#include <ArduinoJson.h>
#include "esp_log.h"

using ArduinoStopwatch::Stopwatch32MS;

class MqttHandler {
public:
    static const u32_t DEFAULT_RETRY_WAIT_MILLIS = 2000;
public:
    /// @brief initializes the MQTT client
    /// @param clientName the name of the MQTT client
    MqttHandler(const char* clientName="MyEspClient");

    /// @brief Configures and connects to the MQTT broker
    /// @param mqttBroker The IP address of the MQTT broker
    /// @param port The port of the MQTT server
    /// @param timeoutMillis Time after the connection attempt fails
    /// @return if the connection was successfully established
    bool connect(IPAddress mqttBroker, u16_t port, MQTT_CALLBACK_SIGNATURE, u32_t timeoutMillis=2000);
    
    /// @brief handles the mqtt connection
    void update();

    /// @brief Publishes a custom JSON object to a specific MQTT topic
    /// @param topic the MQTT topic to publish 
    /// @param jsonMsg The JSON object to be published
    /// @return if the message was successful published
    bool publish(const char* topic, JsonDocument jsonMsg);

    bool subscribe(const char* topic);
private:
    const char* clientName;
    WiFiClient wifiClient;
    PubSubClient mqttClient;
    Stopwatch32MS lastConnectionWatch;
    String sub_topic;
    const char* MQTT_LOG_TAG = "MQTT-Handler";
};