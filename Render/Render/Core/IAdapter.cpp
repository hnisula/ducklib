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
		uint32_t descriptionLength = strlen(description);
		this->description = DL_NEW_ARRAY(DefAlloc(), char, descriptionLength + 1);
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
