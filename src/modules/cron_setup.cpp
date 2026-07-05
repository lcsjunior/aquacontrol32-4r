#include "cron_setup.h"

#include <CronAlarms.h>

#include <arduino_clock.h>
#include <config.h>
#include <relay.h>

constexpr int LAMP_ON_CRON_IDX = 0;
constexpr int LAMP_OFF_CRON_IDX = 1;
constexpr int CO2_ON_CRON_IDX = 2;
constexpr int CO2_OFF_CRON_IDX = 3;

extern Relay lamp;
extern Relay co2;

static void registerCron(const char* label, const char* cronExpr,
                         OnTick_t onTick) {
  if (*cronExpr == '\0')
    return;
  log_i("%s: %s", label, cronExpr);
  Cron.create((char*)cronExpr, onTick, false);
}

void initCron() {
  registerCron("Lamp ON cron", AppConfig.cron(LAMP_ON_CRON_IDX), []() {
    log_i("Lamp ON cron triggered");
    safeCron(lamp, &Relay::turnOn);
  });
  registerCron("Lamp OFF cron", AppConfig.cron(LAMP_OFF_CRON_IDX), []() {
    log_i("Lamp OFF cron triggered");
    lamp.turnOff();
  });
  registerCron("CO2 ON cron", AppConfig.cron(CO2_ON_CRON_IDX), []() {
    log_i("CO2 ON cron triggered");
    safeCron(co2, &Relay::turnOn);
  });
  registerCron("CO2 OFF cron", AppConfig.cron(CO2_OFF_CRON_IDX), []() {
    log_i("CO2 OFF cron triggered");
    co2.turnOff();
  });
}
