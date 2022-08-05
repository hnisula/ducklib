#pragma once

namespace DuckLib::Render
{
#define DL_VK_CHECK(statement, errorText) \
	if ((statement) != VK_SUCCESS) \
		throw std::runtime_error((errorText))
}
