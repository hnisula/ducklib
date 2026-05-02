#ifdef DL_VK
#include <memory>
#include <dxc/dxcapi.h>
#include "ducklib/render/rhi/vk/vk.h"
#include "render/src/render_util.h"

#define CHECK_V(expr) \
    do { \
        auto hresult = (expr); \
        if (FAILED(hresult)) { \
            auto result = map_hresult(hresult); \
            log(LogLevel::ERROR, result, #expr); \
            return; \
    } \
    } while (false)

namespace ducklib::render {
void compile_shader(const std::byte* bytecode, uint32_t bytecode_size, ShaderType shader_type, const char* entry_point,
    Shader* shader_out) {
    IDxcCompiler3* compiler;
    IDxcUtils* utils;
    CHECK_V(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler)));
    CHECK_V(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&utils)));

    DxcBuffer source_buffer{ bytecode, bytecode_size, DXC_CP_UTF8 };
    IDxcResult* result;
    IDxcBlob* spirv_code;
    IDxcCompilerArgs* args;
    constexpr LPCWSTR flags[] = {
        L"-spirv",
        L"fspv-target-env=vulkan1.3",
        L"fvk-use-dx-layout",
        L"fvk-use-dx-position-w"
    };
    wchar_t entry_point_wide[256];
    mbstowcs(entry_point_wide, entry_point, strlen(entry_point));

    CHECK_V(
        utils->BuildArguments(L"dummy", entry_point_wide, map_vk_shader_type(shader_type), (LPCWSTR*)flags,
            std::size(flags), nullptr, 0, &args));
    CHECK_V(compiler->Compile(&source_buffer, nullptr, 0, nullptr, IID_PPV_ARGS(&result)));
    CHECK_V(result->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&spirv_code), nullptr));
    shader_out->bytecode_size = spirv_code->GetBufferSize();
    shader_out->bytecode = spirv_code->GetBufferPointer();

    args->Release();
    spirv_code->Release();
    result->Release();
}
}
#endif
