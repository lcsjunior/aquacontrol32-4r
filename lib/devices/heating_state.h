#ifndef HEATING_STATE_H
#define HEATING_STATE_H

#include "thermostat_state.h"

class HeatingStateImpl : public ThermostatState {
 public:
  void enter(Thermostat& context) override;
  void update(Thermostat& context, float currentTemperatureC) override;
  const char* name() const override;
};

extern HeatingStateImpl HeatingState;

#endif  // HEATING_STATE_H
