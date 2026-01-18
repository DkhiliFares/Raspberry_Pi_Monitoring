#include "ButtonHandler.h"
#include <chrono>
#include <iostream>
#include <pigpio.h>


ButtonHandler::ButtonHandler(int pin) : pin_(pin), running_(false) {
  gpioSetMode(pin_, PI_INPUT);
  gpioSetPullUpDown(pin_, PI_PUD_UP); // Active low
}

ButtonHandler::~ButtonHandler() { stop(); }

void ButtonHandler::setCallback(std::function<void()> callback) {
  callback_ = callback;
}

void ButtonHandler::start() {
  if (running_)
    return;
  running_ = true;
  monitorThread_ = std::thread(&ButtonHandler::monitorLoop, this);
}

void ButtonHandler::stop() {
  if (running_) {
    running_ = false;
    if (monitorThread_.joinable()) {
      monitorThread_.join();
    }
  }
}

void ButtonHandler::monitorLoop() {
  int lastState = 1; // High

  while (running_) {
    int currentState = gpioRead(pin_);

    if (lastState == 1 && currentState == 0) {
      std::cout << "Button Pressed!" << std::endl;
      if (callback_) {
        callback_();
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(200)); // Debounce
    }

    lastState = currentState;
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }
}
