#include "idle_state.h"
#include "thermostat.h"
#include "heating_state.h"

IdleState idleStateSingleton;

void IdleState::enter(Thermostat& context) {
  context.actuator()->turnOff();
}

void IdleState::update(Thermostat& context, float currentTemperatureC) {
  if (currentTemperatureC < context.setpoint()) {
    context.transitionTo(&heatingStateSingleton);
  }
}

const char* IdleState::name() const { return "Idle"; }
