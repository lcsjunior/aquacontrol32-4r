#ifndef ACTUATOR_H
#define ACTUATOR_H

class Actuator {
public:
  virtual ~Actuator() = default;
  virtual void turnOn() = 0;
  virtual void turnOff() = 0;
  virtual bool isOn() const = 0;
};

#endif // ACTUATOR_H
