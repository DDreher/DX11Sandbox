#pragma once

#include <d3d11shader.h>

#include "Core/FileIO.h"
#include "Renderer/ConstantBuffer.h"
#include "Renderer/DX11Types.h"
#include "Renderer/DX11Util.h"
#include "Renderer/Vertex.h"

class ConstantBuffer;

struct ShaderMacro
{
    std::string name;
    std::string value;

    bool operator==(const ShaderMacro& other) const
    {
        return name == other.name && value == other.value;
    }
};
MAKE_HASHABLE(ShaderMacro, t.name, t.value);

struct ShaderCompiler
{
    static HRESULT Compile(const std::string& asset_path, const std::vector<char>& shader_bytes, const std::vector<ShaderMacro>& defines, const char* entry_point,
        const char* shader_target, ComPtr<ID3DBlob>& out_shader_blob);
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

struct TextureBindingDesc
{
    std::string name;
    UINT slot;
};

class ShaderBase
{
    friend class Material;

public:
    ShaderBase(const std::string& asset_path, EShaderType shader_type, const std::vector<ShaderMacro>& defines);
    virtual ~ShaderBase();

    bool LoadFromFile(const std::string asset_path);
    bool Compile(const std::vector<char>& bytes);

protected:
    virtual void Reflect();

    EShaderType shader_type_;
    std::string asset_path_;
    ComPtr<ID3DBlob> shader_blob_;
    ComPtr<ID3D11ShaderReflection> shader_reflection_;
    D3D11_SHADER_DESC shader_desc_;

    std::vector<char> shader_code_;

    std::vector<CBufferBindingDesc> cbuffer_bindings_;
    std::vector<TextureBindingDesc> texture_bindings_;
    std::vector<ShaderMacro> defines_;
};

struct VertexShaderDesc
{
    std::string path;
    std::vector<ShaderMacro> defines;

    bool operator==(const VertexShaderDesc& other) const
    {
        return path == other.path && defines == other.defines;
    }
};

namespace std
{
    template<>
    struct hash<VertexShaderDesc>
    {
        std::size_t operator()(const VertexShaderDesc& desc) const
        {
            std::size_t seed = 0;
            Hash::HashCombine(seed, desc.path);
            Hash::HashCombine(seed, desc.defines.size());
            for(const auto& d : desc.defines)
            {
                Hash::HashCombine(seed, d.name);
                Hash::HashCombine(seed, d.value);
            }

            return seed;
        }
    };
}

class VertexShader : public ShaderBase
{
public:
    VertexShader(const VertexShaderDesc& desc);
    virtual ~VertexShader() {};

    void Bind();

    ComPtr<ID3D11VertexShader> GetNativePtr() const
    {
        return native_ptr_;
    }

    ComPtr<ID3D11InputLayout> GetInputLayout() const
    {
        return input_layout_;
    }

protected:
    virtual void Reflect() override;

    void CreateInputLayoutFromReflection();

    ComPtr<ID3D11VertexShader> native_ptr_;
    ComPtr<ID3D11InputLayout> input_layout_;
};

struct PixelShaderDesc
{
    std::string path;
    std::vector<ShaderMacro> defines;

    bool operator==(const PixelShaderDesc& other) const
    {
        return path == other.path && defines == other.defines;
    }
};

namespace std
{
    template<>
    struct hash<PixelShaderDesc>
    {
        std::size_t operator()(const PixelShaderDesc& desc) const
        {
            std::size_t seed = 0;
            Hash::HashCombine(seed, desc.path);
            Hash::HashCombine(seed, desc.defines.size());
            for (const auto& d : desc.defines)
            {
                Hash::HashCombine(seed, d.name);
                Hash::HashCombine(seed, d.value);
            }

            return seed;
        }
    };
}

class PixelShader : public ShaderBase
{
public:
    PixelShader(const PixelShaderDesc& desc);
    virtual ~PixelShader() {};

    void Bind();

protected:
    ComPtr<ID3D11PixelShader> native_ptr_;
};
