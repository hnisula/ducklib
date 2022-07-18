#pragma once

#include <cstdint>

namespace DuckLib
{
namespace Render
{
class IAdapter
{
public:
	IAdapter(const char* description, bool isHardware);
	virtual ~IAdapter();

	// TODO: Perhaps object to change
	virtual void* GetApiHandle() const = 0;

	uint32_t GetDeviceId() const;
	const char* GetDescription() const;
	bool IsHardware() const;

protected:
	char* description;
	uint32_t deviceId;
	bool isHardware;
};
}
}
