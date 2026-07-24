#include <unity.h>
#include "HeatingState.h"
#include "Thermostat.h"
#include "FakeActuator.h"
#include "FakeClock.h"

static FakeActuator actuator;
static FakeClock fakeClock;
static Thermostat* thermostat;

void setUp() {
  actuator = FakeActuator{};
  fakeClock = FakeClock{};
  fakeClock.advance(60000);
  thermostat = new Thermostat(&actuator, &fakeClock);
  thermostat->begin(24.0f, 0.5f, 0.0f, 40.0f);
  thermostat->update(23.0f);
}

void tearDown() {
  delete thermostat;
  thermostat = nullptr;
}

void test_heating_enter_turns_on_actuator() {
  TEST_ASSERT_TRUE(actuator.isOn());
  TEST_ASSERT_EQUAL_STRING("Heating", thermostat->stateName());
}

void test_heating_name_returns_heating() {
  HeatingStateClass heating;
  TEST_ASSERT_EQUAL_STRING("Heating", heating.name());
}

void test_heating_update_no_transition_when_below_setpoint() {
  fakeClock.advance(60000);
  thermostat->update(23.5f);
  TEST_ASSERT_TRUE(actuator.isOn());
  TEST_ASSERT_EQUAL_STRING("Heating", thermostat->stateName());
}

void test_heating_update_transitions_to_idle_when_above_setpoint() {
  fakeClock.advance(60000);
  thermostat->update(24.6f);
  TEST_ASSERT_FALSE(actuator.isOn());
  TEST_ASSERT_EQUAL_STRING("Idle", thermostat->stateName());
}

void test_heating_update_at_exactly_setpoint_no_transition() {
  fakeClock.advance(60000);
  thermostat->update(24.0f);
  TEST_ASSERT_TRUE(actuator.isOn());
  TEST_ASSERT_EQUAL_STRING("Heating", thermostat->stateName());
}

int main() {
  UNITY_BEGIN();
  RUN_TEST(test_heating_enter_turns_on_actuator);
  RUN_TEST(test_heating_name_returns_heating);
  RUN_TEST(test_heating_update_no_transition_when_below_setpoint);
  RUN_TEST(test_heating_update_transitions_to_idle_when_above_setpoint);
  RUN_TEST(test_heating_update_at_exactly_setpoint_no_transition);
  return UNITY_END();
}
