#ifndef IDLE_STATE_H
#define IDLE_STATE_H

#include "thermostat_state.h"

class IdleState : public ThermostatState {
public:
  void enter(Thermostat& context) override;
  void update(Thermostat& context, float currentTemperatureC) override;
  const char* name() const override;
};

#endif // IDLE_STATE_H
