#include "heating_state.h"
#include "thermostat.h"
#include "idle_state.h"

HeatingState heatingStateSingleton;

void HeatingState::enter(Thermostat& context) {
  context.actuator()->turnOn();
  log_i("[HeatingState] Actuator on");
}

void HeatingState::update(Thermostat& context, float currentTemperatureC) {
  if (currentTemperatureC > context.setpoint() + context.hysteresis()) {
    context.transitionTo(&idleStateSingleton);
  }
}

const char* HeatingState::name() const { return "Heating"; }
