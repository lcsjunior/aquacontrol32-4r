#ifndef THERMOSTAT_STATE_H
#define THERMOSTAT_STATE_H

class Thermostat;

class ThermostatState {
public:
  virtual ~ThermostatState() = default;
  virtual void enter(Thermostat& context) = 0;
  virtual void update(Thermostat& context, float currentTemperatureC) = 0;
  virtual const char* name() const = 0;
};

#endif // THERMOSTAT_STATE_H
