#include "gtest/gtest.h"
#include "../../Shared.h"

using namespace ducklib;
using namespace std;

TEST(Address, CtorDefault_ShouldBeInvalid)
{
	const auto address = Address{};

	EXPECT_TRUE(address.is_valid());
}
