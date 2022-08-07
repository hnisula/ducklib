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
		uint32 descriptionLength = (uint32)strlen(description) + 1;
		strcpy_s(this->description, descriptionLength, description);
	}
}

IAdapter::~IAdapter() { }

bool IAdapter::IsHardware() const
{
	return isHardware;
}
}
