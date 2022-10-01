#pragma once

namespace DuckLib::Render
{
class IFence
{
public:
	virtual ~IFence() = 0;

	virtual void Wait() = 0;
};
}
