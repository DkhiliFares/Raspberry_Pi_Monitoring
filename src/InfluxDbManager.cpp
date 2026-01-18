#include "InfluxDbManager.h"
#include <curl/curl.h>
#include <iostream>


InfluxDbManager::InfluxDbManager(const std::string &serverUrl,
                                 const std::string &dbName)
    : serverUrl_(serverUrl), dbName_(dbName) {
  curl_global_init(CURL_GLOBAL_ALL);
}

InfluxDbManager::~InfluxDbManager() { curl_global_cleanup(); }

bool InfluxDbManager::write(const std::string &measurement, float value,
                            const std::string &tags) {
  // Format: measurement,tag1=val1 field=value timestamp(optional)
  // If tags is empty, just measurement field=value
  std::string line = measurement;
  if (!tags.empty()) {
    line += "," + tags;
  }
  line += " value=" + std::to_string(value);

  return write(line);
}

bool InfluxDbManager::write(const std::string &lineProtocolData) {
  CURL *curl = curl_easy_init();
  if (!curl)
    return false;

  // Build URL: http://localhost:8086/write?db=mydb
  std::string url = serverUrl_ + "/write?db=" + dbName_;

  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_POST, 1L);
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, lineProtocolData.c_str());

  // InfluxDB 1.x usually doesn't need auth if not configured, or basic auth.
  // Assuming simple setup for now.

  CURLcode res = curl_easy_perform(curl);

  if (res != CURLE_OK) {
    std::cerr << "InfluxDB Write Error: " << curl_easy_strerror(res)
              << std::endl;
    curl_easy_cleanup(curl);
    return false;
  }

  long response_code;
  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);

  curl_easy_cleanup(curl);

  if (response_code >= 200 && response_code < 300) {
    return true;
  } else {
    std::cerr << "InfluxDB returned error code: " << response_code << std::endl;
    return false;
  }
}
