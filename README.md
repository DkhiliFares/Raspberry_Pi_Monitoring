# Raspberry Pi IoT Monitoring System (C++ Native)

A high-performance, native C++ application for the Raspberry Pi that performs environmental monitoring (Temperature/Humidity), remote control via MQTT, and time-series data storage in InfluxDB. 

**Constraint:** Developed strictly using native C/C++ libraries (no Node-RED, Python scripts, or high-level IoT frameworks).

## 🚀 Features

- **Real-Time Acquisition**: Reads DHT11 sensor data every 10s (configurable).
- **MQTT Communication**: Publishes data to a broker and subscribes to remote commands.
- **Data Persistence**: Logs all measurements to a local or remote InfluxDB instance via HTTP API.
- **Hardware Control**: 
  - **LED**: Remote ON/OFF/BLINK control + Automatic blinking if Temperature > Threshold.
  - **Button**: Physical push-button interface for local events.
- **Multi-threading**: Dedicated threads for Networking, Sensor Acquisition, and Actuator Control ensure non-blocking operation.

## 🛠 Hardware Setup

| Component | GPIO Pin (BCM) | Description |
|-----------|----------------|-------------|
| **DHT11** | GPIO 4         | Temperature & Humidity Sensor |
| **LED**   | GPIO 17        | Status / Warning Light (Active High) |
| **Button**| GPIO 27        | User Input (Active Low / Pull-Up) |

## 📦 Software Architecture

- **Language**: C++17
- **GPIO Library**: [Pigpio](https://abi-laboratory.pro/tracker/timeline/pigpio/) (Direct hardware register access for precise timing)
- **Messaging**: [Eclipse Paho MQTT C++](https://github.com/eclipse/paho.mqtt.cpp)
- **Database**: [LibCURL](https://curl.se/libcurl/) (HTTP Client for InfluxDB)
- **Build System**: CMake

## 🔨 Installation & Build

For detailed setup instructions (including installing libraries), see [DEPLOYMENT.md](DEPLOYMENT.md).

### Quick Build
```bash
mkdir build
cd build
cmake ..
make
```

### Run
```bash
sudo ./rpi_monitor
```
*(Root privileges required for Pigpio hardware access)*

## 📡 MQTT API

### Published Topics
| Topic | Payload | Description |
|-------|---------|-------------|
| `/rpi/temperature` | `24.5` | Current Temperature (°C) |
| `/rpi/humidity` | `60.0` | Current Humidity (%) |
| `/rpi/sensor` | `{ "temp": 24.5, "hum": 60, "timestamp": ... }` | JSON Object |
| `/rpi/status` | `online` | System LWT / Status |
| `/rpi/button` | `pressed` | Button event |

### Subscribed Topics (Commands)
| Topic | Payload | Description |
|-------|---------|-------------|
| `/rpi/cmd/led` | `ON`, `OFF`, `BLINK` | Control LED state |
| `/rpi/cmd/temp_period`| `Integer` (e.g. `5`) | Change acquisition interval (seconds) |
| `/rpi/cmd/threshold` | `Float` (e.g. `30.0`) | Set temp threshold for Auto-Blink |
| `/rpi/cmd/reboot` | `any` | Trigger simulated system reboot |

## 💾 Data Storage (InfluxDB)

Data is written to the `sensor_data` database.

- **Measurement**: `temperature`, `humidity`
- **Tags**: `source=rpi`
- **Fields**: `value=float`

## 👨‍💻 Project Structure
```
.
├── src/            # Source files (main.cpp, Managers, Controllers)
├── include/        # Header files
├── build/          # Compiled binaries
├── CMakeLists.txt  # Build configuration
├── DEPLOYMENT.md   # Setup guide
└── README.md       # This file
```
