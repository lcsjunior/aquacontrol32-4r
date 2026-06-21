#include "thermostat.h"
#include "idle_state.h"

#define THERMOSTAT_UPDATE_INTERVAL_MS 60000UL

Thermostat::Thermostat(Actuator* actuator, Clock* clock)
    : actuator_(actuator), clock_(clock), currentState_(&IdleState) {
}

void Thermostat::begin(const float setpoint, const float hysteresis,
                       const float lowerLimit, const float upperLimit) {
  setpoint_ = setpoint;
  hysteresis_ = hysteresis;
  lowerLimit_ = lowerLimit;
  upperLimit_ = upperLimit;
  lastUpdateMs_ = clock_->millis() - THERMOSTAT_UPDATE_INTERVAL_MS;
  currentState_ = &IdleState;
  currentState_->enter(*this);
}

void Thermostat::update(float currentTemperatureC) {
  if (isnan(currentTemperatureC) || currentTemperatureC < lowerLimit_ ||
      currentTemperatureC > upperLimit_) {
    forceTransitionToIdle();
    return;
  }
  if (clock_->millis() - lastUpdateMs_ < THERMOSTAT_UPDATE_INTERVAL_MS)
    return;
  lastUpdateMs_ = clock_->millis();
  log_i("temp=%.2f state=%s", currentTemperatureC, currentState_->name());
  currentState_->update(*this, currentTemperatureC);
}

void Thermostat::transitionTo(ThermostatState* nextState) {
  if (nextState == currentState_)
    return;
  ThermostatState* prev = currentState_;
  currentState_ = nextState;
  currentState_->enter(*this);
  log_i("%s -> %s", prev->name(), currentState_->name());
}

void Thermostat::forceTransitionToIdle() {
  if (currentState_ == &IdleState)
    return;
  log_w("Forcing transition to idle");
  currentState_ = &IdleState;
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

bool validateThermostatConfig(const float setpoint, const float hysteresis,
                              const float lowerLimit, const float upperLimit) {
  return lowerLimit < upperLimit && setpoint > lowerLimit &&
         setpoint < upperLimit && hysteresis > 0 &&
         setpoint - hysteresis >= lowerLimit &&
         setpoint + hysteresis <= upperLimit;
}
