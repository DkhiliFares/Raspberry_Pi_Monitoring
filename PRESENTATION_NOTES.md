# Presentation Notes & Architecture

## 1. System Architecture
**Overview**: A distributed IoT monitoring system where the Raspberry Pi acts as an edge node performing acquisition and control, while offloading data storage and supervision to central services (MQTT Broker, InfluxDB).

- **Hardware Layer**:
  - **Sensors**: DHT11 (Temp/Humidity) connected to GPIO 4 (Pin 7).
  - **Actuators**: LED on GPIO 17 (Pin 0).
  - **Input**: Push Button on GPIO 27 (Pin 2).

- **Software Layer (C++)**:
  - **Multi-threaded Design**: Ensures blocking tasks (sensor reading) don't stop control tasks (listening to MQTT).
  - **Modular Classes**: `MqttManager`, `InfluxDbManager`, `LedController`.

## 2. Technical Choices
- **C++ Native**: Chosen for performance, low-level hardware control, and strict satisfying of project requirements (no high-level frameworks).
- **Paho MQTT**: The industry-standard library for MQTT in C/C++; robust and async support.
- **LibCURL**: A mature, stable library for HTTP requests, used for interacting with InfluxDB's API efficiently.
- **Pigpio**: Chosen over WiringPi (deprecated) for better stability and precise timing control. It provides a C interface (compatible with C++) that allows accurate microsecond-level signal generation, which is critical for the DHT11's custom 1-wire protocol. Unlike high-level frameworks, this is a native system library acting directly on hardware registers.

## 3. Code Organization
- **Separation of Concerns**:
  - `src/` vs `include/`: Standard C++ project layout.
  - **Managers**: Handle external communications logic.
  - **Controllers/Sensors**: Encapsulate hardware specifics.
  - **Main**: Orchestrates the startup and acts as the "glue" via callbacks.
- **Concurrency**:
  - **Atomic Variables**: Used for shared configuration (`g_tempThreshold`) to avoid race conditions without heavy locks.
  - **Paho Async**: The MQTT client runs its own loop, freeing our main thread for logic or cleanup.

## 4. Real-time Functionality
- **Data Path**: Sensor -> Read (18ms) -> buffer -> publish MQTT & POST HTTP -> Dashboard.
- **Control Path**: MQTT Message -> Callback -> Immediate Action (LED Toggle).
- **Automation**: The acquisition loop checks `currentTemp > threshold` every cycle, providing autonomous safety logic even if network fails.
