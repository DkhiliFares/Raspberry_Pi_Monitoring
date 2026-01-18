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
sudo apt install influxdb
sudo systemctl unmask influxdb
sudo systemctl enable influxdb
sudo systemctl start influxdb
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
From another terminal or machine:
- **Monitor Data**: `mosquitto_sub -t /rpi/# -v`
- **Command LED**: `mosquitto_pub -t /rpi/cmd/led -m "BLINK"`
- **Change Threshold**: `mosquitto_pub -t /rpi/cmd/threshold -m "25.0"`
- **Change Period**: `mosquitto_pub -t /rpi/cmd/temp_period -m "5"`
