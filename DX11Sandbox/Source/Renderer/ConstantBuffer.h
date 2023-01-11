#pragma once
#include <d3d11.h>

#include "Renderer/DX11Types.h"

enum class ParameterType
{
    Float = 0,
    Vec2,
    Vec3,
    Vec4,
    Mat3,
    Mat4,
    Int,
    Unknown
};

static inline const std::unordered_map<ParameterType, size_t> PARAMETER_TYPE_TO_SIZE_TABLE
{
    { ParameterType::Float, sizeof(float)},
    { ParameterType::Vec3, sizeof(Vec3)},
    { ParameterType::Mat4, sizeof(Mat4)},
};

struct CBufferParam
{
    ParameterType type = ParameterType::Unknown;
    UINT offset = 0;

    bool operator==(const CBufferParam& other) const
    {
        return type == other.type && offset == other.offset;
    }
};

struct CBufferBindingDesc
{
    std::string name;
    uint32 slot = 0;
    std::unordered_map<std::string, CBufferParam> param_map;

    bool operator==(const CBufferBindingDesc& other) const
    {
        return name == other.name && slot == other.slot && param_map == other.param_map;
    }
};

MAKE_HASHABLE(CBufferBindingDesc, t.name, t.slot);

class ConstantBuffer
{
public:
    ConstantBuffer(size_t size);
    ConstantBuffer(const CBufferBindingDesc& desc);
    ~ConstantBuffer();

    void Init();

    bool SetFloat(const std::string& param_name, float val);
    bool SetVec3(const std::string& param_name, Vec3 val);
    bool SetMat4(const std::string& param_name, Mat4 val);
    void SetData(const uint8* data, size_t data_size);

    void Upload();
    void Upload(const uint8* data, size_t data_size);

    static inline const std::string CBUFFER_NAME_PER_FRAME = "PerFrameData";
    static inline const std::string CBUFFER_NAME_PER_VIEW= "PerViewData";
    static inline const std::string CBUFFER_NAME_PER_OBJECT = "PerObjectData";
    static inline const std::string CBUFFER_NAME_PER_INSTANCE = "PerInstanceData";
    static inline const std::string CBUFFER_NAME_PER_MATERIAL= "PerMaterialData";

    std::unordered_map<std::string, CBufferParam> param_map_;

    size_t size_ = 0;
    uint8* data_ = nullptr;
    uint32 slot_ = 0;
    ComPtr<ID3D11Buffer> buffer_ = nullptr;
    bool is_dirty_ = false;
};
