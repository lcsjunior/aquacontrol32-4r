#include "thermostat.h"
#include "idle_state.h"

#define THERMOSTAT_DEBOUNCE_MS 60000UL

Thermostat::Thermostat(Actuator* actuator, Clock* clock)
    : actuator_(actuator), clock_(clock), currentState_(&idleStateSingleton) {
}

void Thermostat::begin(const float setpoint, const float hysteresis,
                       const float lowerLimit, const float upperLimit) {
  setpoint_ = setpoint;
  hysteresis_ = hysteresis;
  lowerLimit_ = lowerLimit;
  upperLimit_ = upperLimit;
  lastTransitionMs_ = clock_->millis() - THERMOSTAT_DEBOUNCE_MS;
  currentState_ = &idleStateSingleton;
  currentState_->enter(*this);
}

void Thermostat::update(float currentTemperatureC) {
  if (isnan(currentTemperatureC) || currentTemperatureC < lowerLimit_ ||
      currentTemperatureC > upperLimit_) {
    forceTransitionToIdle();
    return;
  }
  currentState_->update(*this, currentTemperatureC);
}

void Thermostat::transitionTo(ThermostatState* nextState) {
  if (nextState == currentState_)
    return;
  if (clock_->millis() - lastTransitionMs_ < THERMOSTAT_DEBOUNCE_MS)
    return;
  ThermostatState* prev = currentState_;
  currentState_ = nextState;
  lastTransitionMs_ = clock_->millis();
  currentState_->enter(*this);
  log_i("%s -> %s", prev->name(), currentState_->name());
}

void Thermostat::forceTransitionToIdle() {
  if (currentState_ == &idleStateSingleton)
    return;
  log_w("Forcing transition to idle");
  currentState_ = &idleStateSingleton;
  lastTransitionMs_ = clock_->millis();
  currentState_->enter(*this);
}

Actuator* Thermostat::actuator() const {
  return actuator_;
}

float Thermostat::setpoint() const {
  return setpoint_;
}

float Thermostat::hysteresis() const {
  return hysteresis_;
}

const char* Thermostat::stateName() const {
  return currentState_->name();
}
