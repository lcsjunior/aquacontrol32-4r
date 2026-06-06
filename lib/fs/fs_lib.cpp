#include <Arduino.h>
#include <LittleFS.h>
#include <fs_lib.h>

bool mountLittleFS() {
  if (!LittleFS.begin(true)) {
    log_w("[fs_lib] Failed to mount LittleFS");
    return false;
  }
  return true;
}
