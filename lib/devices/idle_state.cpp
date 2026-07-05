#include "idle_state.h"
#include "heating_state.h"
#include "thermostat.h"

IdleStateImpl IdleState;

void IdleStateImpl::enter(Thermostat& context) {
  context.actuator()->turnOff();
}

void IdleStateImpl::update(Thermostat& context, float currentTemperatureC) {
  if (currentTemperatureC < context.setpoint())
    context.transitionTo(&HeatingState);
}

const char* IdleStateImpl::name() const {
  return "Idle";
}
