#pragma once
#include <cstdint>
#include "Format.h"

namespace DuckLib::Render
{
struct InputComponent
{
	char* name;
	uint32_t index;
	Format format;
	uint32_t inputSlot;
	uint32_t alignOffset;
	uint32_t instanceStepRate;
	bool isInstanceData;
};

struct InputDescription
{
	InputComponent* inputComponents;
	uint32_t numInputComponents;
};
}
