#include <unity.h>
#include <time.h>
#include "arduino_clock.h"

void setUp() {
}
void tearDown() {
}

static time_t makeUtcEpoch(int year, int month, int day, int hour, int minute,
                           int second) {
  struct tm timeInfo = {};
  timeInfo.tm_year = year - 1900;
  timeInfo.tm_mon = month - 1;
  timeInfo.tm_mday = day;
  timeInfo.tm_hour = hour;
  timeInfo.tm_min = minute;
  timeInfo.tm_sec = second;
  return timegm(&timeInfo);
}

void test_epoch_zero_1970_is_not_synced() {
  TEST_ASSERT_FALSE(isClockSynced(0));
}

void test_epoch_just_before_min_year_is_not_synced() {
  time_t beforeMinYear = makeUtcEpoch(2023, 12, 31, 23, 59, 59);
  TEST_ASSERT_FALSE(isClockSynced(beforeMinYear));
}

void test_epoch_at_min_year_boundary_is_synced() {
  time_t atMinYear = makeUtcEpoch(2024, 1, 1, 0, 0, 0);
  TEST_ASSERT_TRUE(isClockSynced(atMinYear));
}

void test_epoch_current_future_year_is_synced() {
  time_t futureYear = makeUtcEpoch(2026, 7, 1, 12, 0, 0);
  TEST_ASSERT_TRUE(isClockSynced(futureYear));
}

int main() {
  UNITY_BEGIN();
  RUN_TEST(test_epoch_zero_1970_is_not_synced);
  RUN_TEST(test_epoch_just_before_min_year_is_not_synced);
  RUN_TEST(test_epoch_at_min_year_boundary_is_synced);
  RUN_TEST(test_epoch_current_future_year_is_synced);
  return UNITY_END();
}
