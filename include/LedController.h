#ifndef LEDCONTROLLER_H
#define LEDCONTROLLER_H

#include <atomic>
#include <thread>

class LedController {
public:
  LedController(int pin);
  ~LedController();

  void on();
  void off();
  void blink(int intervalMs = 500);
  void stopBlink();

  // Check if currently blinking
  bool isBlinking() const;

private:
  int pin_;
  std::atomic<bool> blinking_;
  std::thread blinkThread_;

  void blinkLoop(int intervalMs);
};

#endif // LEDCONTROLLER_H
