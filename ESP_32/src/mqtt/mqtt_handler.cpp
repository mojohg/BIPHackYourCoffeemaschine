#include "mqtt_handler.h"

MqttHandler::MqttHandler(const char* clientName) : 
    clientName(clientName),
    mqttClient(wifiClient)
{
}
bool MqttHandler::connect(IPAddress mqttBroker, u16_t port,  MQTT_CALLBACK_SIGNATURE, u32_t timeoutMillis) {
    Stopwatch32MS watch;
    mqttClient.setServer(mqttBroker, port);
    mqttClient.setCallback(callback);
    ESP_LOGI(MQTT_LOG_TAG, "Connecting to %s:%i as %s", mqttBroker, port, clientName);
    watch.restart();
    for(;;) {
        if(mqttClient.connect(clientName)) {
            break;
        }
        if(watch.getTimeSinceStart() >= timeoutMillis) {
            ESP_LOGW(MQTT_LOG_TAG, "Connecting failed, due to timeout!");
            return false;
        }
    }
    ESP_LOGI(MQTT_LOG_TAG, "Connected to %s:%i", mqttBroker, port);
    mqttClient.publish("esp_state", "i am alive");
    return true;
}
void MqttHandler::update(){
    if(!mqttClient.connected() && lastConnectionWatch.getTimeSinceStart() >= DEFAULT_RETRY_WAIT_MILLIS){
        ESP_LOGW(MQTT_LOG_TAG, "lost connection to mqtt broker!");
        // Attempt to connect
        if (!mqttClient.connect(clientName)) {
            lastConnectionWatch.restart();
            return;   
        }
        ESP_LOGI(MQTT_LOG_TAG, "reconnected to mqtt broker!");
        mqttClient.publish("esp_state", "i am alive");
        if(sub_topic != "") {
            mqttClient.subscribe(sub_topic.c_str());
        }
    }
    mqttClient.loop();
}
bool MqttHandler::publish(const char* topic, JsonDocument jsonMsg){
    String jsonString;
    serializeJson(jsonMsg, jsonString);
    return mqttClient.publish(topic, jsonString.c_str());
}
bool MqttHandler::subscribe(const char* topic) {
    sub_topic = String(topic);
    return mqttClient.subscribe(topic);
}

