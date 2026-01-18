#include "ButtonHandler.h"
#include "Dht11Sensor.h"
#include "InfluxDbManager.h"
#include "LedController.h"
#include "MqttManager.h"
#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <iostream>
#include <mutex>
#include <pigpio.h>
#include <sstream>
#include <thread>
#include <vector>

// Configuration Constants
const std::string MQTT_BROKER = "tcp://localhost:1883";
const std::string CLIENT_ID = "RPi_Monitor_Client";
const std::string INFLUX_URL = "http://localhost:8086";
const std::string INFLUX_DB = "sensor_data";

// GPIO Pins (Pigpio uses BCM numbering)
const int DHT_PIN = 4;  // Was WiringPi 7
const int LED_PIN = 17; // Was WiringPi 0
const int BTN_PIN = 27; // Was WiringPi 2

// Global State (Thread-safe)
std::atomic<float> g_tempThreshold(30.0f);
std::atomic<int> g_acquisitionPeriod(10); // Seconds
std::atomic<bool> g_running(true);

// Components
LedController led(LED_PIN);
Dht11Sensor dht(DHT_PIN);
ButtonHandler btn(BTN_PIN);
MqttManager mqttClient(MQTT_BROKER, CLIENT_ID);
InfluxDbManager influxDb(INFLUX_URL, INFLUX_DB);

class MyMqttCallback : public virtual mqtt::callback {
public:
  void connection_lost(const std::string &cause) override {
    std::cout << "\nConnection lost: " << cause << std::endl;
    // Reconnection logic could go here
  }

  void message_arrived(mqtt::const_message_ptr msg) override {
    std::string topic = msg->get_topic();
    std::string payload = msg->to_string();
    std::cout << "Message arrived [" << topic << "]: " << payload << std::endl;

    if (topic == "/rpi/cmd/led") {
      if (payload == "ON")
        led.on();
      else if (payload == "OFF")
        led.off();
      else if (payload == "BLINK")
        led.blink();
    } else if (topic == "/rpi/cmd/threshold") {
      try {
        float val = std::stof(payload);
        g_tempThreshold = val;
        std::cout << "Threshold updated to: " << val << std::endl;
      } catch (...) {
        std::cerr << "Invalid threshold value" << std::endl;
      }
    } else if (topic == "/rpi/cmd/temp_period") {
      try {
        int val = std::stoi(payload);
        if (val > 0)
          g_acquisitionPeriod = val;
        std::cout << "Period updated to: " << val << std::endl;
      } catch (...) {
        std::cerr << "Invalid period value" << std::endl;
      }
    } else if (topic == "/rpi/cmd/reboot") {
      std::cout << "Reboot command received (Simulation)" << std::endl;
      // system("sudo reboot");
    }
  }

  void delivery_complete(mqtt::delivery_token_ptr token) override {}
};

void sensorLoop() {
  while (g_running) {
    float temp = 0.0f, hum = 0.0f;
    if (dht.read(temp, hum)) {
      std::cout << "Read DHT11: " << temp << "C, " << hum << "%" << std::endl;

      // InfluxDB
      influxDb.write("temperature", temp, "source=rpi");
      influxDb.write("humidity", hum, "source=rpi");

      // MQTT
      mqttClient.publish("/rpi/temperature", std::to_string(temp));
      mqttClient.publish("/rpi/humidity", std::to_string(hum));

      // JSON Sensor Payload
      std::stringstream ss;
      ss << "{\"temp\": " << temp << ", \"hum\": " << hum
         << ", \"timestamp\": " << std::time(nullptr) << "}";
      mqttClient.publish("/rpi/sensor", ss.str());

      // Threshold Check
      if (temp > g_tempThreshold) {
        if (!led.isBlinking()) {
          std::cout << "Temp > Threshold! Auto-Blink ON" << std::endl;
          led.blink();
        }
      }
    } else {
      std::cerr << "Failed to read DHT11" << std::endl;
    }

    std::this_thread::sleep_for(std::chrono::seconds(g_acquisitionPeriod));
  }
}

int main() {
  std::cout << "Initializing Raspberry Pi Monitoring System..." << std::endl;

  if (gpioInitialise() < 0) {
    std::cerr << "pigpio initialisation failed" << std::endl;
    return 1;
  }

  // Button Callback
  btn.setCallback([]() {
    mqttClient.publish("/rpi/button", "pressed");
    // Optional: Toggle LED for local testing
    // led.on();
  });
  // Subscribe
  mqttClient.subscribe("/rpi/cmd/#");
  mqttClient.publish("/rpi/status", "online");

  // Start Input Monitoring (after MQTT is ready)
  btn.start();

  // Start Sensor Thread
  std::thread sensorThread(sensorLoop);

  // Main loop wait
  std::cout << "System Running. Press Enter to exit." << std::endl;
  std::cin.get();

  g_running = false;
  sensorThread.join();
  btn.stop();
  mqttClient.disconnect();

  return 0;
}
