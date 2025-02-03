#ifndef RHI_H
#define RHI_H
#include "Core/Memory/Containers/TArray.h"
#include "IAdapter.h"

namespace ducklib::Render
{
class IRHI
{
public:
	virtual ~IRHI() = default;
	virtual const std::vector<IAdapter*>& GetAdapters() const = 0;
};
}
#endif