#pragma once

namespace DuckLib
{
namespace Render
{
class IAdapter
{
public:
	IAdapter(const char* description, bool isHardware);
	virtual ~IAdapter() = 0;

	// TODO: Premature and probably not the final thing needed
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
