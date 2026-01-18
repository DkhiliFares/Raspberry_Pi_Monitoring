#ifndef DHT11SENSOR_H
#define DHT11SENSOR_H

class Dht11Sensor {
public:
  Dht11Sensor(int pin);
  ~Dht11Sensor();

  bool read(float &temperature, float &humidity);

private:
  int pin_;
};

#endif // DHT11SENSOR_H
