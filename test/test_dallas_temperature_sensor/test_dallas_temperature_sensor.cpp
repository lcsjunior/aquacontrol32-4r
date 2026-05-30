#include <unity.h>
#include "dallas_temperature_sensor.h"

static DallasTemperatureSensor sensor;

void setUp() {
  stubMillis_ = 0;
  logECallCount_ = 0;
  DallasTemperature::setStubTempC(25.0f);
  sensor = DallasTemperatureSensor{};
  sensor.begin(22);
}

void tearDown() {}

void test_request_temperatures_called_on_first_request() {
  sensor.requestTemperatures();
  TEST_ASSERT_EQUAL_FLOAT(25.0f, sensor.getTemperatureC());
}

void test_polling_respects_1000ms_interval() {
  sensor.requestTemperatures();
  unsigned long firstCallMs = stubMillis_;

  stubMillis_ = 999;
  DallasTemperature::setStubTempC(26.0f);
  sensor.requestTemperatures();

  stubMillis_ = firstCallMs;
  DallasTemperature::setStubTempC(25.0f);
  float temp = sensor.getTemperatureC();
  TEST_ASSERT_EQUAL_FLOAT(25.0f, temp);
}

void test_polling_allows_after_1000ms() {
  sensor.requestTemperatures();
  stubMillis_ = 1000;
  DallasTemperature::setStubTempC(26.0f);
  sensor.requestTemperatures();
  TEST_ASSERT_EQUAL_FLOAT(26.0f, sensor.getTemperatureC());
}

void test_get_temperature_propagates_stub_value() {
  DallasTemperature::setStubTempC(28.5f);
  TEST_ASSERT_EQUAL_FLOAT(28.5f, sensor.getTemperatureC());
}

void test_disconnected_sensor_returns_sentinel() {
  DallasTemperature::setStubTempC(DEVICE_DISCONNECTED_C);
  float result = sensor.getTemperatureC();
  TEST_ASSERT_EQUAL_FLOAT(DEVICE_DISCONNECTED_C, result);
}

void test_disconnected_sensor_logs_error() {
  DallasTemperature::setStubTempC(DEVICE_DISCONNECTED_C);
  sensor.getTemperatureC();
  TEST_ASSERT_GREATER_THAN(0, logECallCount_);
}

int main() {
  UNITY_BEGIN();
  RUN_TEST(test_request_temperatures_called_on_first_request);
  RUN_TEST(test_polling_respects_1000ms_interval);
  RUN_TEST(test_polling_allows_after_1000ms);
  RUN_TEST(test_get_temperature_propagates_stub_value);
  RUN_TEST(test_disconnected_sensor_returns_sentinel);
  RUN_TEST(test_disconnected_sensor_logs_error);
  return UNITY_END();
}
