#include <unity.h>
#include "idle_state.h"
#include "thermostat.h"
#include "fake_actuator.h"
#include "fake_clock.h"

static FakeActuator actuator;
static FakeClock clock;
static Thermostat* thermostat;

void setUp() {
  actuator = FakeActuator{};
  clock = FakeClock{};
  clock.advance(60000);
  thermostat = new Thermostat(&actuator, &clock);
  thermostat->begin(24.0f, 0.5f, 0.0f, 40.0f);
}

void tearDown() {
  delete thermostat;
  thermostat = nullptr;
}

void test_idle_enter_turns_off_actuator() {
  actuator.turnOn();
  IdleState idle;
  idle.enter(*thermostat);
  TEST_ASSERT_FALSE(actuator.isOn());
}

void test_idle_name_returns_idle() {
  IdleState idle;
  TEST_ASSERT_EQUAL_STRING("Idle", idle.name());
}

void test_idle_update_no_transition_when_above_threshold() {
  TEST_ASSERT_FALSE(actuator.isOn());
  thermostat->update(24.0f);
  TEST_ASSERT_FALSE(actuator.isOn());
}

void test_idle_update_transitions_to_heating_when_below_threshold() {
  float belowThreshold = 24.0f - 0.5f - 0.1f;
  thermostat->update(belowThreshold);
  TEST_ASSERT_TRUE(actuator.isOn());
}

void test_idle_update_at_exactly_threshold_no_transition() {
  float atThreshold = 24.0f;
  thermostat->update(atThreshold);
  TEST_ASSERT_FALSE(actuator.isOn());
}

int main() {
  UNITY_BEGIN();
  RUN_TEST(test_idle_enter_turns_off_actuator);
  RUN_TEST(test_idle_name_returns_idle);
  RUN_TEST(test_idle_update_no_transition_when_above_threshold);
  RUN_TEST(test_idle_update_transitions_to_heating_when_below_threshold);
  RUN_TEST(test_idle_update_at_exactly_threshold_no_transition);
  return UNITY_END();
}
