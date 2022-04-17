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
	virtual const std::vector<IAdapter*>& GetAdapters() const = 0;
};
}
}
