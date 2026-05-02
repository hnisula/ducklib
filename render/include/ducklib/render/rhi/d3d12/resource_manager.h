#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H
#ifdef DL_D3D12
#include "rhi/command_list.h"
#include "device.h"
#include "types.h"

namespace ducklib::render {
extern CommandList resource_list;
extern Buffer upload_buffer;

void setup_management_resources(Device* device);
void upload_buffer_data(const Buffer& dest_resource, uint64_t offset, const void* data, uint64_t size);
}

#endif
#endif