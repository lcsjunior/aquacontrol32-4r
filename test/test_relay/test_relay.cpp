#include <unity.h>
#include "relay.h"

static Relay relay;

void setUp() {
  digitalWriteCallCount_ = 0;
  relay = Relay{};
  relay.begin(5);
}

void tearDown() {}

void test_begin_configures_pin() {
  TEST_ASSERT_FALSE(relay.isOn());
}

void test_turn_on_sets_is_on() {
  relay.turnOn();
  TEST_ASSERT_TRUE(relay.isOn());
}

void test_turn_off_clears_is_on() {
  relay.turnOn();
  relay.turnOff();
  TEST_ASSERT_FALSE(relay.isOn());
}

void test_toggle_turns_on_when_off() {
  relay.toggle();
  TEST_ASSERT_TRUE(relay.isOn());
}

void test_toggle_turns_off_when_on() {
  relay.turnOn();
  relay.toggle();
  TEST_ASSERT_FALSE(relay.isOn());
}

void test_turn_on_idempotent() {
  relay.turnOn();
  int callsAfterFirstTurnOn = digitalWriteCallCount_;
  relay.turnOn();
  TEST_ASSERT_EQUAL(callsAfterFirstTurnOn, digitalWriteCallCount_);
}

void test_turn_off_idempotent() {
  int callsWhileOff = digitalWriteCallCount_;
  relay.turnOff();
  TEST_ASSERT_EQUAL(callsWhileOff, digitalWriteCallCount_);
}

void test_relay_is_actuator() {
  Actuator* a = &relay;
  a->turnOn();
  TEST_ASSERT_TRUE(relay.isOn());
  a->turnOff();
  TEST_ASSERT_FALSE(relay.isOn());
}

int main() {
  UNITY_BEGIN();
  RUN_TEST(test_begin_configures_pin);
  RUN_TEST(test_turn_on_sets_is_on);
  RUN_TEST(test_turn_off_clears_is_on);
  RUN_TEST(test_toggle_turns_on_when_off);
  RUN_TEST(test_toggle_turns_off_when_on);
  RUN_TEST(test_turn_on_idempotent);
  RUN_TEST(test_turn_off_idempotent);
  RUN_TEST(test_relay_is_actuator);
  return UNITY_END();
}
