#pragma once
#include <d3dcompiler.h>

#include "Core/FileIO.h"
#include "Renderer/DX11Types.h"
#include "Renderer/DX11Util.h"
#include "Renderer/GraphicsContext.h"
#include "Vertex.h"
#include "Bindable.h"

struct ShaderCompiler
{
    static HRESULT Compile(const std::vector<char>& shader_bytes, const char* entry_point, const char* shader_target, ComPtr<ID3DBlob>& out_shader_blob)
    {
        CHECK(shader_bytes.size() > 0);

        // See https://docs.microsoft.com/en-us/windows/win32/direct3dhlsl/d3dcompile-constants
        uint32_t compile_flags = D3DCOMPILE_ENABLE_STRICTNESS; 
#ifndef NDEBUG
        compile_flags |= D3DCOMPILE_DEBUG;
#endif

        ComPtr<ID3DBlob> error_blob = nullptr;
        HRESULT result = D3DCompile
        (
            shader_bytes.data(),    // src to compile
            shader_bytes.size(),    // src size
            nullptr,                // src name
            nullptr,                // defines
            nullptr,                // includes
            entry_point,
            shader_target,
            compile_flags,          // compile constants
            0,                      // effect constants. ignored => 0
            &out_shader_blob,       // the compiled shader
            &error_blob             // error information
        );

        // Sanity Check
        if (FAILED(result) && error_blob != nullptr)
        {
            char* error_str = static_cast<char*>(error_blob->GetBufferPointer());
            LOG_ERROR("Shader compilation failed:\n{}", error_str);
        }

        return result;
    }
};

enum class EShaderType
{
    VS,
    PS,
    CS,
    DS,
    GS,
    HS,
};

template<EShaderType>
struct ShaderTargetLookup;

template<>
struct ShaderTargetLookup<EShaderType::VS>
{
    static inline const char* VERSION = "vs_5_0";
};

template<>
struct ShaderTargetLookup<EShaderType::PS>
{
    static inline const char* VERSION = "ps_5_0";
};

template<EShaderType>
struct ShaderTypeToNativeTypeMap;

template<>
struct ShaderTypeToNativeTypeMap<EShaderType::VS>
{
    typedef ID3D11VertexShader Type;
};

template<>
struct ShaderTypeToNativeTypeMap<EShaderType::PS>
{
    typedef ID3D11PixelShader Type;
};

template<EShaderType T>
class Shader
{
public:
    typedef typename ShaderTypeToNativeTypeMap<T>::Type NativeType;

    Shader(const GraphicsContext& context, const std::string& asset_path)
        : asset_path_(asset_path)
    {
        bool success = Load(context);
        CHECK(success);
    }

    ~Shader() = default;

    const ComPtr<NativeType>& GetNativePtr() const { return native_shader_; }
    const ComPtr<ID3DBlob>& GetBlob() const { return shader_blob_; }

    bool Load(const GraphicsContext& graphics_context)
    {
        LOG("Loading shader: {}", asset_path_);

        std::vector<char> shader_bytes = FileIO::ReadFile(asset_path_);
        if FAILED(ShaderCompiler::Compile(shader_bytes, "Main", ShaderTargetLookup<T>::VERSION, shader_blob_))
        {
            return false;
        }

        native_shader_ = Shader::Create<T>(graphics_context, shader_blob_);
        if(native_shader_ == nullptr)
        {
            return false;
        }

        return true;
    }

    template<EShaderType T>
    static auto Create(const GraphicsContext& graphics_context, const ComPtr<ID3DBlob>& shader_blob);

    template<>
    static auto Create<EShaderType::VS>(const GraphicsContext& graphics_context, const ComPtr<ID3DBlob>& shader_blob)
    {
        CHECK(graphics_context.device != nullptr);
        CHECK(shader_blob != nullptr);
        NativeType* native_shader = nullptr;
        HRESULT result = graphics_context.device->CreateVertexShader(shader_blob->GetBufferPointer(), shader_blob->GetBufferSize(), nullptr, &native_shader);
        CHECK(native_shader != nullptr);

        return native_shader;
    }

    template<>
    static auto Create<EShaderType::PS>(const GraphicsContext& graphics_context, const ComPtr<ID3DBlob>& shader_blob)
    {
        CHECK(graphics_context.device != nullptr);
        CHECK(shader_blob != nullptr);
        NativeType* native_shader = nullptr;
        HRESULT result = graphics_context.device->CreatePixelShader(shader_blob->GetBufferPointer(), shader_blob->GetBufferSize(), nullptr, &native_shader);
        CHECK(native_shader != nullptr);
        return native_shader;
    }

private:
    std::string asset_path_;
    ComPtr<ID3DBlob> shader_blob_;
    ComPtr<NativeType> native_shader_;
};
