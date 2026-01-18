#include "LedController.h"
#include <iostream>
#include <pigpio.h>


LedController::LedController(int pin) : pin_(pin), blinking_(false) {
  gpioSetMode(pin_, PI_OUTPUT);
  off();
}

LedController::~LedController() {
  stopBlink();
  off();
}

void LedController::on() {
  stopBlink();
  gpioWrite(pin_, 1);
}

void LedController::off() {
  stopBlink();
  gpioWrite(pin_, 0);
}

void LedController::blink(int intervalMs) {
  if (blinking_) {
    stopBlink();
  }
  blinking_ = true;
  blinkThread_ = std::thread(&LedController::blinkLoop, this, intervalMs);
}

void LedController::stopBlink() {
  if (blinking_) {
    blinking_ = false;
    if (blinkThread_.joinable()) {
      blinkThread_.join();
    }
  }
}

bool LedController::isBlinking() const { return blinking_; }

void LedController::blinkLoop(int intervalMs) {
  while (blinking_) {
    gpioWrite(pin_, 1);
    std::this_thread::sleep_for(std::chrono::milliseconds(intervalMs));
    if (!blinking_)
      break;
    gpioWrite(pin_, 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(intervalMs));
  }
  gpioWrite(pin_, 0);
}
