#ifndef IDLE_STATE_H
#define IDLE_STATE_H

#include "ThermostatState.h"

class IdleStateClass : public ThermostatState {
 public:
  void enter(Thermostat& context) override;
  void update(Thermostat& context, float currentTemperatureC) override;
  const char* name() const override;
};

extern IdleStateClass IdleState;

#endif  // IDLE_STATE_H
