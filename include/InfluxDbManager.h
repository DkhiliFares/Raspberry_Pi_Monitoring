#ifndef INFLUXDBMANAGER_H
#define INFLUXDBMANAGER_H

#include <string>

class InfluxDbManager {
public:
  InfluxDbManager(const std::string &serverUrl, const std::string &dbName);
  ~InfluxDbManager();

  bool write(const std::string &measurement, float value,
             const std::string &tags = "");
  bool write(const std::string &lineProtocolData);

private:
  std::string serverUrl_;
  std::string dbName_;
};

#endif // INFLUXDBMANAGER_H
