# Deployment & Build Guide

# Hardware Wiring Guide

| Component | Pin Function | BCM GPIO (Pigpio) | Connection Details |
|-----------|--------------|-------------------|--------------------|
| **DHT11** | Data         | 4                 | Connect to Pin 7 (GPIO 4). VCC to 3.3V, GND to GND. |
| **LED**   | Signal (+)   | 17                | Anode to Pin 11 (GPIO 17), Cathode to GND (via 220Ω resistor) |
| **Button**| Signal       | 27                | One leg to Pin 13 (GPIO 27), other leg to GND (Internal Pull-Up enabled) |

## Prerequisites on Raspberry Pi
Ensure you have the following installed on your Raspberry Pi OS:

```bash
sudo apt-get update
sudo apt-get install -y cmake g++ git
sudo apt-get install -y libssl-dev build-essential
sudo apt-get install -y libcurl4-openssl-dev
sudo apt-get install -y mosquitto mosquitto-clients
# pigpio (Build from source if not in repo)
wget https://github.com/joan2937/pigpio/archive/master.zip
unzip master.zip
cd pigpio-master
make
sudo make install
cd ..
rm -rf pigpio-master master.zip
```

### Install Paho MQTT C & C++ Libraries
```bash
# Paho MQTT C
git clone https://github.com/eclipse/paho.mqtt.c.git
cd paho.mqtt.c
cmake -Bbuild -H. -DPAHO_ENABLE_TESTING=OFF -DPAHO_BUILD_STATIC=ON \
    -DPAHO_WITH_SSL=ON -DPAHO_HIGH_PERFORMANCE=ON
sudo cmake --build build/ --target install
sudo ldconfig

# Paho MQTT C++
cd ..
git clone https://github.com/eclipse/paho.mqtt.cpp.git
cd paho.mqtt.cpp
cmake -Bbuild -H. -DPAHO_BUILD_STATIC=ON -DPAHO_BUILD_DOCUMENTATION=FALSE -DPAHO_BUILD_SAMPLES=TRUE
sudo cmake --build build/ --target install
sudo ldconfig
```

### Install InfluxDB (if local)
```bash
sudo apt install -y influxdb influxdb-client
sudo systemctl unmask influxdb
sudo systemctl enable influxdb
sudo systemctl start influxdb

# Create the database using CLI
influx -execute 'CREATE DATABASE sensor_data'

# OR create using CURL if CLI is missing
curl -i -XPOST http://localhost:8086/query --data-urlencode "q=CREATE DATABASE sensor_data"
```

## Compilation
Navigate to the project directory on the Pi:
```bash
mkdir build
cd build
cmake ..
make
```

## Running the Application
1. Start user services (InfluxDB, Mosquitto if local):
   ```bash
   sudo service influxdb start
   # If running local broker
   # sudo service mosquitto start
   ```
2. Run the program (needs root for WiringPi/DOM):
   ```bash
   sudo ./rpi_monitor
   ```

## Testing via MQTT
Open two terminal windows (or tabs) to test sending and receiving.

### 1. Monitor All Outputs
Subscribe to all `/rpi/` topics to see temperature, humidity, sensor JSON, status, and button events.
```bash
mosquitto_sub -t "/rpi/#" -v
```
*You should see data arriving every 10 seconds (default).*

### 2. Send Control Commands
In the second terminal, test the following commands:

**LED Control:**
```bash
# Turn LED ON
mosquitto_pub -t "/rpi/cmd/led" -m "ON"

# Turn LED OFF
mosquitto_pub -t "/rpi/cmd/led" -m "OFF"

# Start Blinking
mosquitto_pub -t "/rpi/cmd/led" -m "BLINK"
```

**Configuration:**
```bash
# Change sensor reading period to 5 seconds
mosquitto_pub -t "/rpi/cmd/temp_period" -m "5"

# Set temperature alert threshold to 25.5°C
# (If current temp > 25.5, LED will auto-blink)
mosquitto_pub -t "/rpi/cmd/threshold" -m "25.5"
```

**System:**
```bash
# Simulate Remote Reboot (Prints log message only)
mosquitto_pub -t "/rpi/cmd/reboot" -m "now"
```
