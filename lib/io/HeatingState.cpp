#include "HeatingState.h"

#include "IdleState.h"
#include "Thermostat.h"

HeatingStateClass HeatingState;

void HeatingStateClass::enter(Thermostat& context) {
  context.actuator()->turnOn();
}

void HeatingStateClass::update(Thermostat& context, float currentTemperatureC) {
  if (currentTemperatureC > context.setpoint() + context.hysteresis())
    context.transitionTo(&IdleState);
}

const char* HeatingStateClass::name() const {
  return "Heating";
}
