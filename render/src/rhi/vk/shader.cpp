#include <dxc/dxcapi.h>
#include "ducklib/render/rhi/vk/vk.h"
#include "render/src/render_util.h"

namespace ducklib::render {
void compile_shader(const std::byte* bytecode, uint32_t bytecode_size, ShaderType shader_type, const char* entry_point,
    Shader* shader_out) {
    IDxcCompiler3* compiler;
    // TODO: Check results and hresults (maybe make Result into RenderResult with both  api-specific error code and my mapped one
    auto hresult = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler));
    if (FAILED(hresult)) {
        render_log(LogLevel::ERROR, Result(hresult), "Failed to create DXC instance for shader compilation");
    }

    DxcBuffer source_buffer{ bytecode, bytecode_size, DXC_CP_UTF8 };
    IDxcResult* result;
    IDxcBlob* spirv_code;
    compiler->Compile(&source_buffer, nullptr, 0, nullptr, IID_PPV_ARGS(&result));
    result->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&spirv_code), nullptr);
    shader_out->bytecode_size = spirv_code->GetBufferSize();
    shader_out->bytecode = spirv_code->GetBufferPointer();
}
}
