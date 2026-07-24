#ifndef THERMOSTAT_H
#define THERMOSTAT_H

#include "Actuator.h"
#include "Clock.h"
#include "ThermostatState.h"

class Thermostat {
 public:
  explicit Thermostat(Actuator* actuator, Clock* clock);

  void begin(const float setpoint, const float hysteresis,
             const float lowerLimit, const float upperLimit);
  void update(float currentTemperatureC);

  void transitionTo(ThermostatState* nextState);

  Actuator* actuator() const;
  float setpoint() const;
  float hysteresis() const;
  const char* stateName() const;

 private:
  Actuator* actuator_;
  Clock* clock_;
  ThermostatState* currentState_;
  float setpoint_;
  float hysteresis_;
  float lowerLimit_;
  float upperLimit_;
  unsigned long lastUpdateMs_ = 0;
};

bool validateThermostatConfig(const float setpoint, const float hysteresis,
                              const float lowerLimit, const float upperLimit);

#endif  // THERMOSTAT_H
