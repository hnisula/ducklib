#include <gtest/gtest.h>
#include "net/shared.h"

using namespace ducklib;

TEST(SomeTest, FirstTest)
{
    ASSERT_TRUE(true);
}

TEST(address_tests, address_constructor)
{
    Address addr{"127.0.0.1"};
    ASSERT_TRUE(addr.get_port() == 0);
}
