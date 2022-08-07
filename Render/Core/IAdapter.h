#pragma once

#include "../../Core/Types.h"

namespace DuckLib::Render
{
class IAdapter
{
public:

	IAdapter(const char* description, bool isHardware);
	virtual ~IAdapter();

	virtual void* GetApiHandle() const = 0;
	bool IsHardware() const;

protected:

	static constexpr uint32 MAX_DESCRIPTION_SIZE = 128;

	char description[MAX_DESCRIPTION_SIZE];
	bool isHardware;
};
}
