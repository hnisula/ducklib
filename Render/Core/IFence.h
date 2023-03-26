#pragma once

namespace DuckLib::Render
{
class IFence
{
public:
	virtual ~IFence() {}

	virtual void Wait() = 0;
};
}
