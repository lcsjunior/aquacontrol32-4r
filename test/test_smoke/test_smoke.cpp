#include <unity.h>

void setUp() {}
void tearDown() {}

void test_toolchain_is_working() {
    TEST_ASSERT_TRUE(true);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_toolchain_is_working);
    return UNITY_END();
}
