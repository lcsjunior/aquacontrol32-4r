#include <unity.h>
#include "DallasTempSensor.h"

static DallasTempSensor sensor;

void setUp() {
  stubMillis_ = 0;
  logECallCount_ = 0;
  DallasTemperature::stubTemperatureC(25.0f);
  sensor = DallasTempSensor{};
  sensor.begin(22);
}

void tearDown() {}

void test_request_temperatures_called_on_first_request() {
  sensor.requestTemperatures();
  TEST_ASSERT_EQUAL_FLOAT(25.0f, sensor.temperatureC());
}

void test_polling_respects_1000ms_interval() {
  sensor.requestTemperatures();
  unsigned long firstCallMs = stubMillis_;

  stubMillis_ = 999;
  DallasTemperature::stubTemperatureC(26.0f);
  sensor.requestTemperatures();

  stubMillis_ = firstCallMs;
  DallasTemperature::stubTemperatureC(25.0f);
  float temp = sensor.temperatureC();
  TEST_ASSERT_EQUAL_FLOAT(25.0f, temp);
}

void test_polling_allows_after_1000ms() {
  sensor.requestTemperatures();
  stubMillis_ = 1000;
  DallasTemperature::stubTemperatureC(26.0f);
  sensor.requestTemperatures();
  TEST_ASSERT_EQUAL_FLOAT(26.0f, sensor.temperatureC());
}

void test_get_temperature_propagates_stub_value() {
  DallasTemperature::stubTemperatureC(28.5f);
  sensor.requestTemperatures();
  TEST_ASSERT_EQUAL_FLOAT(28.5f, sensor.temperatureC());
}

void test_disconnected_sensor_returns_sentinel() {
  DallasTemperature::stubTemperatureC(DEVICE_DISCONNECTED_C);
  sensor.requestTemperatures();
  float result = sensor.temperatureC();
  TEST_ASSERT_EQUAL_FLOAT(DEVICE_DISCONNECTED_C, result);
}

void test_disconnected_sensor_logs_error() {
  DallasTemperature::stubTemperatureC(DEVICE_DISCONNECTED_C);
  sensor.requestTemperatures();
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
