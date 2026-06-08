#include "heating_state.h"
#include "idle_state.h"
#include "thermostat.h"

HeatingStateImpl HeatingState;

void HeatingStateImpl::enter(Thermostat& context) {
  context.actuator()->turnOn();
}

void HeatingStateImpl::update(Thermostat& context, float currentTemperatureC) {
  if (currentTemperatureC > context.setpoint() + context.hysteresis()) {
    context.transitionTo(&IdleState);
  }
}

const char* HeatingStateImpl::name() const {
  return "Heating";
}
