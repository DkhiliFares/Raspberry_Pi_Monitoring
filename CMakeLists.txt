cmake_minimum_required(VERSION 3.10)
project(rpi_monitor)

set(CMAKE_CXX_STANDARD 17)

# Include directories
include_directories(include)

# Find Paho MQTT
find_library(PAHO_MQTT_C paho-mqtt3as)
find_library(PAHO_MQTT_CPP paho-mqttpp3)

# Find CURL
find_package(CURL REQUIRED)

# Find Pigpio
find_library(PIGPIO_LIB pigpio)

# Source files
file(GLOB SOURCES "src/*.cpp")

# Executable
add_executable(rpi_monitor ${SOURCES})

# Link libraries
target_link_libraries(rpi_monitor 
    ${PAHO_MQTT_CPP} 
    ${PAHO_MQTT_C} 
    ${CURL_LIBRARIES} 
    ${PIGPIO_LIB}
    pthread
    rt
)
