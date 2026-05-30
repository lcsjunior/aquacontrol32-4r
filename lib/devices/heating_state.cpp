#include "heating_state.h"
#include "thermostat.h"
#include "idle_state.h"

HeatingState heatingStateSingleton;
extern IdleState idleStateSingleton;

void HeatingState::enter(Thermostat& context) {
  context.actuator()->turnOn();
}

void HeatingState::update(Thermostat& context, float currentTemperatureC) {
  if (currentTemperatureC > context.setpoint()) {
    context.transitionTo(&idleStateSingleton);
  }
}

const char* HeatingState::name() const { return "Heating"; }
