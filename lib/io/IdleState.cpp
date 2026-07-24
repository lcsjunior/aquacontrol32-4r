#include "IdleState.h"

#include "HeatingState.h"
#include "Thermostat.h"

IdleStateClass IdleState;

void IdleStateClass::enter(Thermostat& context) {
  context.actuator()->turnOff();
}

void IdleStateClass::update(Thermostat& context, float currentTemperatureC) {
  if (currentTemperatureC < context.setpoint())
    context.transitionTo(&HeatingState);
}

const char* IdleStateClass::name() const {
  return "Idle";
}
