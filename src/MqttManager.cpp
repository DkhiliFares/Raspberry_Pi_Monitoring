#include "MqttManager.h"
#include <iostream>

MqttManager::MqttManager(const std::string &address,
                         const std::string &clientId)
    : client_(address, clientId) {}

MqttManager::~MqttManager() {
  try {
    if (client_.is_connected()) {
      client_.disconnect()->wait();
    }
  } catch (const mqtt::exception &exc) {
    std::cerr << "Error disconnecting: " << exc.what() << std::endl;
  }
}

void MqttManager::connect() {
  try {
    mqtt::connect_options connOpts;
    connOpts.set_keep_alive_interval(20);
    connOpts.set_clean_session(true);

    std::cout << "Connecting to the MQTT server..." << std::endl;
    client_.connect(connOpts)->wait();
    std::cout << "Connected to MQTT server" << std::endl;
  } catch (const mqtt::exception &exc) {
    std::cerr << "Error connecting to MQTT: " << exc.what() << std::endl;
  }
}

void MqttManager::disconnect() {
  try {
    std::cout << "Disconnecting from MQTT..." << std::endl;
    client_.disconnect()->wait();
    std::cout << "Disconnected" << std::endl;
  } catch (const mqtt::exception &exc) {
    std::cerr << "Error disconnecting: " << exc.what() << std::endl;
  }
}

void MqttManager::publish(const std::string &topic,
                          const std::string &payload) {
  try {
    client_.publish(topic, payload.c_str(), payload.length(), 1, false);
  } catch (const mqtt::exception &exc) {
    std::cerr << "Error publishing to " << topic << ": " << exc.what()
              << std::endl;
  }
}

void MqttManager::subscribe(const std::string &topic) {
  try {
    client_.subscribe(topic, 1);
    std::cout << "Subscribed to " << topic << std::endl;
  } catch (const mqtt::exception &exc) {
    std::cerr << "Error subscribing to " << topic << ": " << exc.what()
              << std::endl;
  }
}

void MqttManager::setCallback(mqtt::callback &cb) { client_.set_callback(cb); }
