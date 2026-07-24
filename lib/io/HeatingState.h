#ifndef HEATING_STATE_H
#define HEATING_STATE_H

#include "ThermostatState.h"

class HeatingStateClass : public ThermostatState {
 public:
  void enter(Thermostat& context) override;
  void update(Thermostat& context, float currentTemperatureC) override;
  const char* name() const override;
};

extern HeatingStateClass HeatingState;

#endif  // HEATING_STATE_H
