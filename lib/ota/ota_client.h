#ifndef OTA_CLIENT_H
#define OTA_CLIENT_H

class OTAClient {
 public:
  void begin(const char* hostname, const char* password);
  void handle();
};

extern OTAClient OTA;

#endif  // OTA_CLIENT_H
