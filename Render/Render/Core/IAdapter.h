#pragma once

namespace DuckLib
{
namespace Render
{
class IAdapter
{
public:

	IAdapter(const char* description, bool isHardware);

	virtual void* GetApiHandle() const = 0;
	bool IsHardware() const;

protected:

	virtual ~IAdapter();

	char* description;
	bool isHardware;
};
}
}
