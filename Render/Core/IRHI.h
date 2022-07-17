#pragma once
#include <vector>
#include "IAdapter.h"

namespace DuckLib
{
namespace Render
{
class IRHI
{
public:
	virtual const IAdapter* Adapters() const = 0;
	// TODO: Remove virtual?
	virtual uint32_t AdapterCount() const = 0;
};
}
}
