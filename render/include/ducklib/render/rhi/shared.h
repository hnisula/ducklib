#ifndef DUCKLIB_SHARED_H
#define DUCKLIB_SHARED_H

namespace ducklib::render {
enum class AdapterType {
    DISCRETE_GPU,
    INTEGRATED_GPU,
    CPU,
    OTHER
};

struct AdapterInfo {
    uint64_t device_luid;
    uint32_t device_id;
    uint32_t vendor_id;
    char device_name[64];
    AdapterType type;
};

enum class QueueType {
    GRAPHICS,
    COPY,
    COMPUTE
};

enum class HeapType {
    DEFAULT,
    UPLOAD
};

enum class DescriptorHeapType {
    CBV_SRV_UAV,
    SAMPLER,
    RT,
    DS
};

enum class DescriptorSetRangeType {
    SRV,
    UAV,
    CBV,
    SAMPLER
};

#undef DOMAIN
enum class BindingStage {
    VERTEX = 1,
    PIXEL = 2,
    GEOMETRY = 4,
    HULL = 8,
    DOMAIN = 16,
    COMPUTE = 32,
    ALL = 255
};

enum class ShaderType {
    VERTEX,
    PIXEL,
    GEOMETRY,
    HULL,
    DOMAIN,
    COMPUTE
};

enum class ShaderVisibility {
    ALL,
    VERTEX,
    HULl,
    DOMAIN,
    GEOMETRY,
    PIXEL
};

enum class BindingType {
    CONSTANT,
    BUFFER_DESCRIPTOR, // "BUFFER_DESCRIPTOR"?
    SRV_DESCRIPTOR, // TODO: Rename? "READ_DESCRIPTOR"?
    UAV_DESCRIPTOR, // "WRITE_DESCRIPTOR"?
    DESCRIPTOR_SET
};

enum class ResourceType {
    BUFFER,
    TEXTURE_1D,
    TEXTURE_2D,
    TEXTURE_3D,
    TEXTURE_CUBE
};

enum class FillMode {
    SOLID,
    WIREFRAME
};

enum class CullMode {
    BACK,
    FRONT,
    NONE
};

enum class FrontFace {
    CLOCKWISE,
    COUNTER_CLOCKWISE
};

enum class DepthComparison {
    LTEQ,
    LT,
    GTEQ,
    GT,
    EQ,
    NEQ,
    ALWAYS,
    NEVER
};

enum class InputSlotType {
    PER_VERTEX_DATA,
    PER_INSTANCE_DATA
};

enum class PrimitiveTopology {
    UNDEFINED,
    POINT,
    LINE,
    TRIANGLE,
    PATCH
};
}

#endif //DUCKLIB_SHARED_H