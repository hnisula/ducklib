#include "Core/Memory/IAlloc.h"
#include <cstring>
#include "IAdapter.h"

namespace DuckLib::Render
{
IAdapter::IAdapter(const char* description, bool isHardware)
{
	this->isHardware = isHardware;

	if (description)
	{
		uint32_t descriptionLength = (uint32_t)strlen(description) + 1;
		strcpy_s(this->description, descriptionLength, description);
	}
}

IAdapter::~IAdapter() { }

bool IAdapter::IsHardware() const
{
	return isHardware;
}
}
