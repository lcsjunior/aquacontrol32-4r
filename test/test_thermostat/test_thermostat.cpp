#include <unity.h>
#include "Thermostat.h"
#include "HeatingState.h"
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
}

void tearDown() {
  delete thermostat;
  thermostat = nullptr;
}

void test_begin_leaves_in_idle_actuator_off() {
  TEST_ASSERT_FALSE(actuator.isOn());
  TEST_ASSERT_EQUAL_STRING("Idle", thermostat->stateName());
}

void test_first_transition_after_begin_is_not_blocked() {
  thermostat->update(23.0f);
  TEST_ASSERT_TRUE(actuator.isOn());
  TEST_ASSERT_EQUAL_STRING("Heating", thermostat->stateName());
}

void test_transition_idle_to_heating_when_below_threshold() {
  float belowThreshold = 24.0f - 0.5f - 0.1f;
  thermostat->update(belowThreshold);
  TEST_ASSERT_TRUE(actuator.isOn());
  TEST_ASSERT_EQUAL_STRING("Heating", thermostat->stateName());
}

void test_second_transition_within_debounce_is_suppressed() {
  thermostat->update(23.0f);
  TEST_ASSERT_TRUE(actuator.isOn());
  TEST_ASSERT_EQUAL_STRING("Heating", thermostat->stateName());

  fakeClock.advance(30000);
  thermostat->update(25.0f);
  TEST_ASSERT_TRUE(actuator.isOn());
  TEST_ASSERT_EQUAL_STRING("Heating", thermostat->stateName());
}

void test_transition_allowed_after_exactly_60000ms() {
  thermostat->update(23.0f);
  TEST_ASSERT_TRUE(actuator.isOn());
  TEST_ASSERT_EQUAL_STRING("Heating", thermostat->stateName());

  fakeClock.advance(60000);
  thermostat->update(25.0f);
  TEST_ASSERT_FALSE(actuator.isOn());
  TEST_ASSERT_EQUAL_STRING("Idle", thermostat->stateName());
}

void test_invalid_temperature_nan_forces_idle() {
  thermostat->update(23.0f);
  TEST_ASSERT_TRUE(actuator.isOn());

  thermostat->update(NAN);
  TEST_ASSERT_FALSE(actuator.isOn());
  TEST_ASSERT_EQUAL_STRING("Idle", thermostat->stateName());
}

void test_invalid_temperature_below_lower_limit_forces_idle() {
  thermostat->update(23.0f);
  TEST_ASSERT_TRUE(actuator.isOn());

  thermostat->update(-1.0f);
  TEST_ASSERT_FALSE(actuator.isOn());
  TEST_ASSERT_EQUAL_STRING("Idle", thermostat->stateName());
}

void test_invalid_temperature_above_upper_limit_forces_idle() {
  thermostat->update(23.0f);
  TEST_ASSERT_TRUE(actuator.isOn());

  thermostat->update(41.0f);
  TEST_ASSERT_FALSE(actuator.isOn());
  TEST_ASSERT_EQUAL_STRING("Idle", thermostat->stateName());
}

void test_safety_force_bypasses_update_interval() {
  thermostat->update(23.0f);
  TEST_ASSERT_TRUE(actuator.isOn());

  fakeClock.advance(1000);
  thermostat->update(-1.0f);
  TEST_ASSERT_FALSE(actuator.isOn());
  TEST_ASSERT_EQUAL_STRING("Idle", thermostat->stateName());
}

void test_force_transition_to_idle_is_noop_when_already_idle() {
  int turnOffCountBefore = actuator.turnOffCount_;
  thermostat->update(NAN);
  TEST_ASSERT_EQUAL(turnOffCountBefore, actuator.turnOffCount_);
  TEST_ASSERT_EQUAL_STRING("Idle", thermostat->stateName());
}

void test_force_transition_to_heating_transitions_from_idle() {
  thermostat->transitionTo(&HeatingState);
  TEST_ASSERT_TRUE(actuator.isOn());
  TEST_ASSERT_EQUAL_STRING("Heating", thermostat->stateName());
}

void test_force_transition_to_heating_is_noop_when_already_heating() {
  thermostat->transitionTo(&HeatingState);
  int turnOnCountBefore = actuator.turnOnCount_;
  thermostat->transitionTo(&HeatingState);
  TEST_ASSERT_EQUAL(turnOnCountBefore, actuator.turnOnCount_);
  TEST_ASSERT_EQUAL_STRING("Heating", thermostat->stateName());
}

void test_transition_to_same_state_is_noop() {
  int turnOffCountBefore = actuator.turnOffCount_;
  thermostat->update(25.0f);
  TEST_ASSERT_EQUAL(turnOffCountBefore, actuator.turnOffCount_);
  TEST_ASSERT_EQUAL_STRING("Idle", thermostat->stateName());
}

void test_heating_turns_on_actuator() {
  thermostat->update(23.0f);
  TEST_ASSERT_EQUAL(1, actuator.turnOnCount_);
  TEST_ASSERT_EQUAL_STRING("Heating", thermostat->stateName());
}

void test_idle_turns_off_actuator() {
  thermostat->update(23.0f);
  TEST_ASSERT_TRUE(actuator.isOn());
  TEST_ASSERT_EQUAL_STRING("Heating", thermostat->stateName());

  fakeClock.advance(60000);
  thermostat->update(25.0f);
  TEST_ASSERT_FALSE(actuator.isOn());
  TEST_ASSERT_GREATER_THAN(0, actuator.turnOffCount_);
  TEST_ASSERT_EQUAL_STRING("Idle", thermostat->stateName());
}

void test_full_cycle_idle_heating_idle() {
  TEST_ASSERT_FALSE(actuator.isOn());
  TEST_ASSERT_EQUAL_STRING("Idle", thermostat->stateName());

  thermostat->update(23.0f);
  TEST_ASSERT_TRUE(actuator.isOn());
  TEST_ASSERT_EQUAL_STRING("Heating", thermostat->stateName());

  fakeClock.advance(60000);
  thermostat->update(25.0f);
  TEST_ASSERT_FALSE(actuator.isOn());
  TEST_ASSERT_EQUAL_STRING("Idle", thermostat->stateName());

  fakeClock.advance(60000);
  thermostat->update(23.0f);
  TEST_ASSERT_TRUE(actuator.isOn());
  TEST_ASSERT_EQUAL_STRING("Heating", thermostat->stateName());
}

void test_validate_defaults_are_valid() {
  TEST_ASSERT_TRUE(validateThermostatConfig(23.5f, 0.5f, 0.0f, 28.0f));
}

void test_validate_rejects_lower_limit_not_below_upper() {
  TEST_ASSERT_FALSE(validateThermostatConfig(23.5f, 0.5f, 30.0f, 30.0f));
  TEST_ASSERT_FALSE(validateThermostatConfig(23.5f, 0.5f, 31.0f, 30.0f));
}

void test_validate_rejects_setpoint_above_upper_limit() {
  TEST_ASSERT_FALSE(validateThermostatConfig(31.0f, 0.5f, 0.0f, 30.0f));
}

void test_validate_rejects_setpoint_below_lower_limit() {
  TEST_ASSERT_FALSE(validateThermostatConfig(-1.0f, 0.5f, 0.0f, 30.0f));
}

void test_validate_rejects_non_positive_hysteresis() {
  TEST_ASSERT_FALSE(validateThermostatConfig(23.5f, 0.0f, 0.0f, 30.0f));
  TEST_ASSERT_FALSE(validateThermostatConfig(23.5f, -0.5f, 0.0f, 30.0f));
}

void test_validate_rejects_actuation_band_below_lower_limit() {
  TEST_ASSERT_FALSE(validateThermostatConfig(0.2f, 0.5f, 0.0f, 30.0f));
}

void test_validate_rejects_actuation_band_above_upper_limit() {
  TEST_ASSERT_FALSE(validateThermostatConfig(29.8f, 0.5f, 0.0f, 30.0f));
}

int main() {
  UNITY_BEGIN();
  RUN_TEST(test_begin_leaves_in_idle_actuator_off);
  RUN_TEST(test_first_transition_after_begin_is_not_blocked);
  RUN_TEST(test_transition_idle_to_heating_when_below_threshold);
  RUN_TEST(test_second_transition_within_debounce_is_suppressed);
  RUN_TEST(test_transition_allowed_after_exactly_60000ms);
  RUN_TEST(test_invalid_temperature_nan_forces_idle);
  RUN_TEST(test_invalid_temperature_below_lower_limit_forces_idle);
  RUN_TEST(test_invalid_temperature_above_upper_limit_forces_idle);
  RUN_TEST(test_safety_force_bypasses_update_interval);
  RUN_TEST(test_force_transition_to_idle_is_noop_when_already_idle);
  RUN_TEST(test_force_transition_to_heating_transitions_from_idle);
  RUN_TEST(test_force_transition_to_heating_is_noop_when_already_heating);
  RUN_TEST(test_transition_to_same_state_is_noop);
  RUN_TEST(test_heating_turns_on_actuator);
  RUN_TEST(test_idle_turns_off_actuator);
  RUN_TEST(test_full_cycle_idle_heating_idle);
  RUN_TEST(test_validate_defaults_are_valid);
  RUN_TEST(test_validate_rejects_lower_limit_not_below_upper);
  RUN_TEST(test_validate_rejects_setpoint_above_upper_limit);
  RUN_TEST(test_validate_rejects_setpoint_below_lower_limit);
  RUN_TEST(test_validate_rejects_non_positive_hysteresis);
  RUN_TEST(test_validate_rejects_actuation_band_below_lower_limit);
  RUN_TEST(test_validate_rejects_actuation_band_above_upper_limit);
  return UNITY_END();
}
