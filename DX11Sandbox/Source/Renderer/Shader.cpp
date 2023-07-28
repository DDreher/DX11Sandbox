#pragma comment(lib, "dxguid.lib")

#include "Renderer/Shader.h"

#include <d3dcompiler.h>

#include "Core/FileIO.h"
#include "Renderer/ConstantBuffer.h"
#include "Renderer/DX11Util.h"
#include "Renderer/GraphicsContext.h"

namespace
{
    static const char* ENTRYPOINT = "Main";
    static const std::unordered_map<EShaderType, const char*> SHADER_TARGET_MAP =
    {
        { EShaderType::VS, "vs_5_0" },
        { EShaderType::PS, "ps_5_0" }
    };

    static DXGI_FORMAT GetDXGIFormat(const D3D11_SIGNATURE_PARAMETER_DESC& parameter_desc)
    {
        BYTE component_mask = parameter_desc.Mask;
        int num_components = 0;
        while (component_mask)
        {
            if (component_mask & 0x01)
            {
                num_components++;
            }

            component_mask = component_mask >> 1;
        }

        if (parameter_desc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
        {
            switch (num_components)
            {
            case 1: return DXGI_FORMAT_R32_FLOAT;
            case 2: return DXGI_FORMAT_R32G32_FLOAT;
            case 3: return DXGI_FORMAT_R32G32B32_FLOAT;
            case 4: return DXGI_FORMAT_R32G32B32A32_FLOAT;
            default: CHECK_NO_ENTRY();
            }
        }
        else if (parameter_desc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
        {
            switch (num_components)
            {
            case 1: return DXGI_FORMAT_R32_SINT;
            case 2: return DXGI_FORMAT_R32G32_SINT;
            case 3: return DXGI_FORMAT_R32G32B32_SINT;
            case 4: return DXGI_FORMAT_R32G32B32A32_SINT;
            default: CHECK_NO_ENTRY();
            }
        }
        else if (parameter_desc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
        {
            switch (num_components)
            {
            case 1: return DXGI_FORMAT_R32_UINT;
            case 2: return DXGI_FORMAT_R32G32_UINT;
            case 3: return DXGI_FORMAT_R32G32B32_UINT;
            case 4: return DXGI_FORMAT_R32G32B32A32_UINT;
            default: CHECK_NO_ENTRY();
            }
        }
        else
        {
            CHECK_NO_ENTRY();
        }

        return DXGI_FORMAT_UNKNOWN;
    }
}

HRESULT ShaderCompiler::Compile(const std::string& asset_path, const std::vector<uint8>& shader_bytes, const std::vector<ShaderMacro>& defines, const char* entry_point,
    const char* shader_target, ComPtr<ID3DBlob>& out_shader_blob)
{
    CHECK(shader_bytes.size() > 0);

    // See https://docs.microsoft.com/en-us/windows/win32/direct3dhlsl/d3dcompile-constants
    uint32_t compile_flags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifndef NDEBUG
    compile_flags |= D3DCOMPILE_DEBUG;
    compile_flags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    std::vector<D3D_SHADER_MACRO> shader_macros;
    for (const auto& d : defines)
    {
        shader_macros.push_back({
            .Name = d.name.c_str(),
            .Definition = d.value.c_str()
        });
    }

    constexpr size_t NUM_DEFAULT_DEFINES = 1;
    static const std::array<D3D_SHADER_MACRO, NUM_DEFAULT_DEFINES> DEFAULT_DEFINES = {
        { nullptr, nullptr }
    };
    shader_macros.insert(shader_macros.end(), DEFAULT_DEFINES.begin(), DEFAULT_DEFINES.end());

    ComPtr<ID3DBlob> error_blob = nullptr;
    HRESULT result = D3DCompile
    (
        shader_bytes.data(),    // src to compile
        shader_bytes.size(),    // src size
        asset_path.c_str(),                // src name
        shader_macros.data(),
        D3D_COMPILE_STANDARD_FILE_INCLUDE,                // includes
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

//////////////////////////////////////////////////////////////////////////

bool UncompiledShader::LoadFromFile(const std::string asset_path)
{
    LOG("Loading shader: {}", asset_path);
    asset_path_ = asset_path;
    shader_code_ = FileIO::ReadFile(asset_path_);
    return true;
}

//////////////////////////////////////////////////////////////////////////

ShaderBase::ShaderBase(const std::string& asset_path, EShaderType shader_type, const std::vector<ShaderMacro>& defines)
    : asset_path_(asset_path), shader_type_(shader_type), defines_(defines)
{
    Handle<UncompiledShader> shader_handle = gfx::resource_manager->uncompiled_shaders.GetHandle({ asset_path });
    UncompiledShader* uncompiled_shader = gfx::resource_manager->uncompiled_shaders.Get(shader_handle);
    CHECK(uncompiled_shader != nullptr);
    Compile(uncompiled_shader->shader_code_);
}

ShaderBase::~ShaderBase()
{
    LOG("Destroy shader: {}", asset_path_);
}

bool ShaderBase::Compile(const std::vector<uint8>& bytes)
{
    const auto it = ::SHADER_TARGET_MAP.find(shader_type_);
    CHECK_MSG(it != ::SHADER_TARGET_MAP.end(), "Tried to compile unknown shader type");

    LOG("Compiling shader: {}", asset_path_);
    bool did_compilation_succeed = SUCCEEDED(ShaderCompiler::Compile(asset_path_, bytes, defines_, ::ENTRYPOINT, it->second, shader_blob_));
    
    // For now we'll assert until shader hot reloading is implemented.
    CHECK_MSG(did_compilation_succeed, "Shader compilation failed.");

    return did_compilation_succeed;
}

void ShaderBase::Reflect()
{
    CHECK(shader_blob_ != nullptr);
    DX11_VERIFY(D3DReflect(shader_blob_->GetBufferPointer(), shader_blob_->GetBufferSize(), IID_ID3D11ShaderReflection, &shader_reflection_));
    DX11_VERIFY(shader_reflection_->GetDesc(&shader_desc_));
    
    for (size_t i = 0; i < shader_desc_.BoundResources; i++)
    {
        D3D11_SHADER_INPUT_BIND_DESC binding_desc;
        shader_reflection_->GetResourceBindingDesc(static_cast<uint32>(i), &binding_desc);

        if(binding_desc.Type == D3D_SHADER_INPUT_TYPE::D3D_SIT_CBUFFER)
        {
            const std::string& cbuffer_name(binding_desc.Name);
            if(cbuffer_name != ConstantBuffer::CBUFFER_NAME_PER_MATERIAL)
            {
                // Skip - These will be set from outside shaders / materials
                continue;
            }

            ID3D11ShaderReflectionConstantBuffer* cbuffer = shader_reflection_->GetConstantBufferByName(binding_desc.Name);
            CHECK(cbuffer != nullptr);

            D3D11_SHADER_BUFFER_DESC shader_buffer_desc;
            DX11_VERIFY(cbuffer->GetDesc(&shader_buffer_desc));

            if (shader_buffer_desc.Type == D3D11_CBUFFER_TYPE::D3D11_CT_CBUFFER)
            {
                LOG("Cbuffer: {}", shader_buffer_desc.Name);

                CBufferBindingDesc cbuffer_desc;
                cbuffer_desc.name = shader_buffer_desc.Name;
                cbuffer_desc.slot = static_cast<uint32>(binding_desc.BindPoint);

                for (UINT var_idx = 0; var_idx < shader_buffer_desc.Variables; ++var_idx)
                {
                    ID3D11ShaderReflectionVariable* var = cbuffer->GetVariableByIndex(var_idx);
                    CHECK(var != nullptr);

                    D3D11_SHADER_VARIABLE_DESC var_desc;
                    DX11_VERIFY(var->GetDesc(&var_desc));
                    LOG("-- Var: {}", var_desc.Name);

                    ID3D11ShaderReflectionType* var_type = var->GetType();
                    CHECK(var_type != nullptr);

                    D3D11_SHADER_TYPE_DESC var_type_desc;
                    DX11_VERIFY(var_type->GetDesc(&var_type_desc));

                    CBufferParam cbuffer_param;
                    cbuffer_param.offset = var_desc.StartOffset;

                    if (var_type_desc.Class == D3D_SHADER_VARIABLE_CLASS::D3D10_SVC_MATRIX_COLUMNS)
                    {
                        if (var_type_desc.Columns == 3)
                        {
                            cbuffer_param.type = ParameterType::Mat3;
                        }
                        else if (var_type_desc.Columns == 4)
                        {
                            cbuffer_param.type = ParameterType::Mat4;
                        }
                    }
                    else if (var_type_desc.Class == D3D_SHADER_VARIABLE_CLASS::D3D_SVC_VECTOR)
                    {
                        if (var_type_desc.Columns == 2)
                        {
                            cbuffer_param.type = ParameterType::Vec2;
                        }
                        else if (var_type_desc.Columns == 3)
                        {
                            cbuffer_param.type = ParameterType::Vec3;
                        }
                        else if (var_type_desc.Columns == 4)
                        {
                            cbuffer_param.type = ParameterType::Vec4;
                        }
                    }
                    else if (var_type_desc.Class == D3D_SHADER_VARIABLE_CLASS::D3D_SVC_SCALAR)
                    {
                        // TODO: Check var_type_desc.Type for float / int etc
                        if(var_type_desc.Type == D3D_SHADER_VARIABLE_TYPE::D3D_SVT_FLOAT)
                        {
                            cbuffer_param.type = ParameterType::Float;
                        }
                        else if(var_type_desc.Type == D3D_SHADER_VARIABLE_TYPE::D3D_SVT_INT ||
                            var_type_desc.Type == D3D_SHADER_VARIABLE_TYPE::D3D_SVT_UINT)
                        {
                            cbuffer_param.type = ParameterType::Int;
                        }
                    }
                    else
                    {
                        LOG("Unparsed parameter type");
                    }

                    cbuffer_desc.param_map[var_desc.Name] = cbuffer_param;
                }

                cbuffer_bindings_.push_back(cbuffer_desc);
            }
        }
        
        if (binding_desc.Type == D3D_SHADER_INPUT_TYPE::D3D_SIT_TEXTURE)
        {
            LOG("Texture binding: {}", binding_desc.Name);
            TextureBindingDesc desc
            {
                .name = binding_desc.Name,
                .slot = binding_desc.BindPoint
            };
            texture_bindings_.push_back(desc);
        }

        if (binding_desc.Type == D3D_SHADER_INPUT_TYPE::D3D_SIT_SAMPLER)
        {
            LOG("Sampler binding {}", binding_desc.Name);
        }
    }
}

//////////////////////////////////////////////////////////////////////////

VertexShader::VertexShader(const VertexShaderDesc& desc)
    : ShaderBase(desc.path, EShaderType::VS, desc.defines)
{
    CHECK(shader_blob_ != nullptr);
    CHECK(native_ptr_ == nullptr);
    DX11_VERIFY(gfx::device->CreateVertexShader(shader_blob_->GetBufferPointer(), shader_blob_->GetBufferSize(), nullptr, &native_ptr_));
    SetDebugName(native_ptr_.Get(), asset_path_.c_str());

    Reflect();
}

void VertexShader::Bind()
{
    CHECK(input_layout_ != nullptr);
    gfx::SetInputLayout(input_layout_.Get());

    CHECK(native_ptr_ != nullptr);
    gfx::SetVertexShader(native_ptr_.Get());
}

void VertexShader::Reflect()
{
    ShaderBase::Reflect();
    CreateInputLayoutFromReflection();
}

void VertexShader::CreateInputLayoutFromReflection()
{
    CHECK(shader_reflection_ != nullptr);

    std::vector<D3D11_INPUT_ELEMENT_DESC> layout_desc;
    for (UINT i = 0; i < shader_desc_.InputParameters; ++i)
    {
        D3D11_SIGNATURE_PARAMETER_DESC param_desc;
        DX11_VERIFY(shader_reflection_->GetInputParameterDesc(i, &param_desc));

        D3D11_INPUT_ELEMENT_DESC element_desc = {};
        element_desc.SemanticName = param_desc.SemanticName;
        element_desc.SemanticIndex = param_desc.SemanticIndex;
        element_desc.InputSlot = static_cast<UINT>(layout_desc.size());
        element_desc.AlignedByteOffset = 0;
        element_desc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        element_desc.InstanceDataStepRate = 0;
        element_desc.Format = ::GetDXGIFormat(param_desc);
        layout_desc.push_back(element_desc);
    }

    DX11_VERIFY(gfx::device->CreateInputLayout(layout_desc.data(), static_cast<UINT>(layout_desc.size()),
        shader_blob_->GetBufferPointer(), shader_blob_->GetBufferSize(), &input_layout_));
    SetDebugName(input_layout_.Get(), asset_path_.c_str());
}

//////////////////////////////////////////////////////////////////////////

PixelShader::PixelShader(const PixelShaderDesc& desc)
    : ShaderBase(desc.path, EShaderType::PS, desc.defines)
{
    CHECK(shader_blob_ != nullptr);
    CHECK(native_ptr_ == nullptr);
    DX11_VERIFY(gfx::device->CreatePixelShader(shader_blob_->GetBufferPointer(), shader_blob_->GetBufferSize(), nullptr, &native_ptr_));
    SetDebugName(native_ptr_.Get(), asset_path_.c_str());
    Reflect();
}

void PixelShader::Bind()
{
    CHECK(native_ptr_ != nullptr);
    gfx::SetPixelShader(native_ptr_.Get());
}
