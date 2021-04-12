#include <Core/Memory/IAlloc.h>
#include <cstring>
#include "IAdapter.h"

namespace DuckLib
{
namespace Render
{
IAdapter::IAdapter(const char* description, bool isHardware)
{
	this->isHardware = isHardware;

	if (description)
	{
		uint32_t descriptionLength = (uint32_t)strlen(description) + 1;
		this->description = DL_NEW_ARRAY(DefAlloc(), char, descriptionLength);
		strcpy_s(this->description, descriptionLength, description);
	}
}

IAdapter::~IAdapter()
{
	if (description)
		DL_DELETE_ARRAY(DefAlloc(), description);
}

bool IAdapter::IsHardware() const
{
	return isHardware;
}
}
}
