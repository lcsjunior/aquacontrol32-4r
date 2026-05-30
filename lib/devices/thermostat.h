#ifndef THERMOSTAT_H
#define THERMOSTAT_H

#include <Arduino.h>
#include "relay.h"

#define MILLIS_PER_SECOND 1000UL
#define IDLE_TIMEOUT (MILLIS_PER_SECOND * 60)

enum ThermostatState { IDLE, COOLING, HEATING };

class Thermostat {
private:
  ThermostatState _state = IDLE;
  Relay *_k;
  float _setpoint;
  float _hysteresis;
  float _lowerLimit;
  float _upperLimit;
  unsigned long _stateExitTime = 0;

public:
  Thermostat(Relay *k) : _k(k){};
  ThermostatState getState() const;
  void setState(ThermostatState newState);
  char *getStatus() const;
  void begin(const float setpoint, const float hysteresis,
             const float lowerLimit, const float upperLimit);
  void handleCooler(float cTemp);
  void handleHeater(float cTemp);
};

#endif // THERMOSTAT_H