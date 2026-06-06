#include <unity.h>
#include "fake_clock.h"

void setUp() {}
void tearDown() {}

void test_fake_clock_starts_at_zero() {
  FakeClock clock;
  TEST_ASSERT_EQUAL(0, clock.millis());
}

void test_fake_clock_advance_increments_millis() {
  FakeClock clock;
  clock.advance(1000);
  TEST_ASSERT_EQUAL(1000, clock.millis());
}

void test_fake_clock_multiple_advances_accumulate() {
  FakeClock clock;
  clock.advance(500);
  clock.advance(500);
  TEST_ASSERT_EQUAL(1000, clock.millis());
}

void test_fake_clock_advance_large_value() {
  FakeClock clock;
  clock.advance(60000);
  TEST_ASSERT_EQUAL(60000, clock.millis());
}

int main() {
  UNITY_BEGIN();
  RUN_TEST(test_fake_clock_starts_at_zero);
  RUN_TEST(test_fake_clock_advance_increments_millis);
  RUN_TEST(test_fake_clock_multiple_advances_accumulate);
  RUN_TEST(test_fake_clock_advance_large_value);
  return UNITY_END();
}