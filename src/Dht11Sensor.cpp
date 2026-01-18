#include "Dht11Sensor.h"
#include <chrono>
#include <iostream>
#include <pigpio.h>
#include <thread>


Dht11Sensor::Dht11Sensor(int pin) : pin_(pin) {
  // Initialise is handled in main, but we can verify
  // or set mode here.
  gpioSetMode(pin_, PI_INPUT);
}

Dht11Sensor::~Dht11Sensor() {}

bool Dht11Sensor::read(float &temperature, float &humidity) {
  uint8_t dht11_dat[5] = {0, 0, 0, 0, 0};
  uint8_t laststate = 1; // High
  uint8_t counter = 0;
  uint8_t j = 0, i;

  dht11_dat[0] = dht11_dat[1] = dht11_dat[2] = dht11_dat[3] = dht11_dat[4] = 0;

  // Pull pin down for 18ms
  gpioSetMode(pin_, PI_OUTPUT);
  gpioWrite(pin_, 0);
  std::this_thread::sleep_for(std::chrono::milliseconds(18));

  // Pull pin up for 40us
  gpioWrite(pin_, 1);
  gpioDelay(40);

  // Prepare to read
  gpioSetMode(pin_, PI_INPUT);

  // Detect change and read data
  // Pigpio is fast, but we are still bit-banging in user space.
  // Ideally use gpioSetAlertFunc for edge detection logic, but keeping loop for
  // similarity.
  for (i = 0; i < 85; i++) {
    counter = 0;
    while (gpioRead(pin_) == laststate) {
      counter++;
      gpioDelay(1);
      if (counter == 255) {
        break;
      }
    }
    laststate = gpioRead(pin_);

    if (counter == 255)
      break;

    // Ignore first 3 transitions
    if ((i >= 4) && (i % 2 == 0)) {
      dht11_dat[j / 8] <<= 1;
      if (counter > 16)
        dht11_dat[j / 8] |= 1;
      j++;
    }
  }

  if ((j >= 40) &&
      (dht11_dat[4] ==
       ((dht11_dat[0] + dht11_dat[1] + dht11_dat[2] + dht11_dat[3]) & 0xFF))) {

    humidity = (float)dht11_dat[0] + (float)dht11_dat[1] / 10.0f;
    temperature = (float)dht11_dat[2] + (float)dht11_dat[3] / 10.0f;
    return true;
  } else {
    return false;
  }
}
