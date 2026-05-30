#ifndef HEATING_STATE_H
#define HEATING_STATE_H

#include "thermostat_state.h"

class HeatingState : public ThermostatState {
public:
  void enter(Thermostat& context) override;
  void update(Thermostat& context, float currentTemperatureC) override;
  const char* name() const override;
};

#endif // HEATING_STATE_H
