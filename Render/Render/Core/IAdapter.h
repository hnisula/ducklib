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

	virtual void* GetApiHandle() const = 0;
	bool IsHardware() const;

protected:

	char* description;
	bool isHardware;
};
}
}
