#pragma comment(lib, "dxguid.lib")

#include "Shader.h"

#include <d3dcompiler.h>

#include "Core/FileIO.h"
#include "Renderer/ConstantBuffer.h"

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


HRESULT ShaderCompiler::Compile(const std::string& asset_path, const std::vector<char>& shader_bytes, const char* entry_point,
    const char* shader_target, ComPtr<ID3DBlob>& out_shader_blob)
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
        asset_path.c_str(),                // src name
        nullptr,                // defines
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

ShaderBase::ShaderBase(GraphicsContext* context, const std::string& asset_path, EShaderType shader_type)
    : context_(context),
    shader_type_(shader_type)
{
    CHECK(context != nullptr);
    LoadFromFile(asset_path);
    Compile(*context_, shader_code_);
}

ShaderBase::~ShaderBase()
{
    LOG("Destroy shader: {}", asset_path_);
}

bool ShaderBase::LoadFromFile(const std::string asset_path)
{
    LOG("Loading shader: {}", asset_path);
    asset_path_ = asset_path;
    shader_code_ = FileIO::ReadFile(asset_path_);
    return true;
}

bool ShaderBase::Compile(const GraphicsContext& context, const std::vector<char>& bytes)
{
    LOG("Compiling shader: {}", asset_path_);

    const auto it = ::SHADER_TARGET_MAP.find(shader_type_);
    if(it != ::SHADER_TARGET_MAP.end())
    {
        if(SUCCEEDED(ShaderCompiler::Compile(asset_path_, bytes, ::ENTRYPOINT, it->second, shader_blob_)))
        {
            return true;
        }
    }

    return false;
}

void ShaderBase::Reflect(GraphicsContext& context)
{
    CHECK(shader_blob_ != nullptr);
    DX11_VERIFY(D3DReflect(shader_blob_->GetBufferPointer(), shader_blob_->GetBufferSize(), IID_ID3D11ShaderReflection, &shader_reflection_));
    DX11_VERIFY(shader_reflection_->GetDesc(&shader_desc_));
    
    for (size_t i = 0; i < shader_desc_.BoundResources; i++)
    {
        D3D11_SHADER_INPUT_BIND_DESC binding_desc;
        shader_reflection_->GetResourceBindingDesc(static_cast<UINT>(i), &binding_desc);

        if(binding_desc.Type == D3D_SHADER_INPUT_TYPE::D3D_SIT_CBUFFER)
        {
            const std::string& cbuffer_name(binding_desc.Name);
            if(cbuffer_name == ConstantBuffer::CBUFFER_NAME_PER_FRAME ||
                cbuffer_name == ConstantBuffer::CBUFFER_NAME_PER_INSTANCE)
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
                        cbuffer_param.type = ParameterType::Float;
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

VertexShader::VertexShader(GraphicsContext* context, const std::string& asset_path)
    : ShaderBase(context, asset_path, EShaderType::VS)
{
    Create(*context_);
    Reflect(*context_);
}

void VertexShader::Create(GraphicsContext& context)
{
    CHECK(context.device != nullptr);
    CHECK(shader_blob_ != nullptr);
    CHECK(native_ptr_ == nullptr);
    DX11_VERIFY(context.device->CreateVertexShader(shader_blob_->GetBufferPointer(), shader_blob_->GetBufferSize(), nullptr, &native_ptr_));
}

void VertexShader::Bind(GraphicsContext& context)
{
    CHECK(context.device != nullptr);
    
    CHECK(input_layout_ != nullptr);
    context.device_context->IASetInputLayout(input_layout_.Get());

    CHECK(native_ptr_ != nullptr);
    context.device_context->VSSetShader(native_ptr_.Get(), nullptr, 0);
}

void VertexShader::Reflect(GraphicsContext& context)
{
    ShaderBase::Reflect(context);
    CreateInputLayoutFromReflection(context);
}

void VertexShader::CreateInputLayoutFromReflection(const GraphicsContext& context)
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

    DX11_VERIFY(context.device->CreateInputLayout(layout_desc.data(), static_cast<UINT>(layout_desc.size()),
        shader_blob_->GetBufferPointer(), shader_blob_->GetBufferSize(), &input_layout_));
}


//////////////////////////////////////////////////////////////////////////

PixelShader::PixelShader(GraphicsContext* context, const std::string& asset_path)
    : ShaderBase(context, asset_path, EShaderType::PS)
{
    Create(*context_);
    Reflect(*context_);
}

void PixelShader::Create(GraphicsContext& context)
{
    CHECK(context.device != nullptr);
    CHECK(shader_blob_ != nullptr);
    CHECK(native_ptr_ == nullptr);
    DX11_VERIFY(context.device->CreatePixelShader(shader_blob_->GetBufferPointer(), shader_blob_->GetBufferSize(), nullptr, &native_ptr_));
}

void PixelShader::Bind(GraphicsContext& context)
{
    CHECK(context.device != nullptr);
    CHECK(native_ptr_ != nullptr);

    context.device_context->PSSetShader(native_ptr_.Get(), nullptr, 0);
}
