#ifndef MQTTMANAGER_H
#define MQTTMANAGER_H

#include <string>
#include <mqtt/async_client.h>

class MqttManager {
public:
    MqttManager(const std::string& address, const std::string& clientId);
    virtual ~MqttManager();

    void connect();
    void disconnect();
    void publish(const std::string& topic, const std::string& payload);
    void subscribe(const std::string& topic);
    void setCallback(mqtt::callback& cb);

private:
    mqtt::async_client client_;
};

#endif // MQTTMANAGER_H
