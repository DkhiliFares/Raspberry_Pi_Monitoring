#ifndef BUTTONHANDLER_H
#define BUTTONHANDLER_H

#include <atomic>
#include <functional>
#include <thread>


class ButtonHandler {
public:
  ButtonHandler(int pin);
  ~ButtonHandler();

  void setCallback(std::function<void()> callback);
  void start();
  void stop();

private:
  int pin_;
  std::function<void()> callback_;
  std::atomic<bool> running_;
  std::thread monitorThread_;

  void monitorLoop();
};

#endif // BUTTONHANDLER_H
