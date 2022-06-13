#pragma once
#include "Engine/Transform.h"
#include "Renderer/Bindable.h"
#include "Renderer/ConstantBufferTypes.h"
#include "Renderer/GraphicsContext.h"
#include "Renderer/IndexBuffer.h"
#include "Renderer/Material.h"
#include "Renderer/VertexBuffer.h"

struct MeshData : public IBindable
{
    virtual void Bind(GraphicsContext& context) override;

    static SharedPtr<MeshData> LoadFromFile(const GraphicsContext& context, const std::string& asset_path);
    
    SharedPtr<IndexBuffer> index_buffer;
    SharedPtr<VertexBuffer> pos;
    SharedPtr<VertexBuffer> uv;
    SharedPtr<VertexBuffer> normals;
};

struct CubeMeshData : public MeshData
{
    CubeMeshData(GraphicsContext& context)
    {
        std::vector<uint16> vertex_indices = {
            // front
            0,  1,  2,
            0,  2,  3,
            // back
            4,  5,  6,
            4,  6,  7,
            // top
            8,  9, 10,
            8, 10, 11,
            // bottom
            12, 13, 14,
            12, 14, 15,
            // left
            16, 17, 18,
            16, 18, 19,
            // right
            20, 21, 22,
            20, 22, 23
        };

        std::vector<Vec3> vertex_pos =
        {
            // front
            Vec3(-1.0f, -1.0f,  -1.0f),
            Vec3(-1.0f,  1.0f,  -1.0f),
            Vec3(1.0f,  1.0f,   -1.0f),
            Vec3(1.0f, -1.0f,   -1.0f),
            // back
            Vec3(-1.0f, -1.0f, 1.0f),
            Vec3(1.0f, -1.0f, 1.0f), 
            Vec3(1.0f,  1.0f, 1.0f), 
            Vec3(-1.0f,  1.0f, 1.0f),
            // top
            Vec3(-1.0f, 1.0f, -1.0f),
            Vec3(-1.0f, 1.0f,  1.0f),
            Vec3(1.0f, 1.0f,  1.0f), 
            Vec3(1.0f, 1.0f, -1.0f), 
            // bottom
            Vec3(-1.0f, -1.0f, -1.0f),
            Vec3(1.0f, -1.0f, -1.0f), 
            Vec3(1.0f, -1.0f,  1.0f), 
            Vec3(-1.0f, -1.0f,  1.0f),
            // left
            Vec3(-1.0f, -1.0f,  1.0f),
            Vec3(-1.0f,  1.0f,  1.0f),
            Vec3(-1.0f,  1.0f, -1.0f),
            Vec3(-1.0f, -1.0f, -1.0f),
            // right
            Vec3(1.0f, -1.0f, -1.0f),
            Vec3(1.0f,  1.0f, -1.0f),
            Vec3(1.0f,  1.0f,  1.0f),
            Vec3(1.0f, -1.0f,  1.0f)
        };

        std::vector<Vec3> vertex_normals =
        {
            // TODO: Set proper normals

            // front
            Vec3(-1.0f, -1.0f,  -1.0f),
            Vec3(-1.0f,  1.0f,  -1.0f),
            Vec3(1.0f,  1.0f,   -1.0f),
            Vec3(1.0f, -1.0f,   -1.0f),
            // back
            Vec3(-1.0f, -1.0f, 1.0f),
            Vec3(1.0f, -1.0f, 1.0f),
            Vec3(1.0f,  1.0f, 1.0f),
            Vec3(-1.0f,  1.0f, 1.0f),
            // top
            Vec3(-1.0f, 1.0f, -1.0f),
            Vec3(-1.0f, 1.0f,  1.0f),
            Vec3(1.0f, 1.0f,  1.0f),
            Vec3(1.0f, 1.0f, -1.0f),
            // bottom
            Vec3(-1.0f, -1.0f, -1.0f),
            Vec3(1.0f, -1.0f, -1.0f),
            Vec3(1.0f, -1.0f,  1.0f),
            Vec3(-1.0f, -1.0f,  1.0f),
            // left
            Vec3(-1.0f, -1.0f,  1.0f),
            Vec3(-1.0f,  1.0f,  1.0f),
            Vec3(-1.0f,  1.0f, -1.0f),
            Vec3(-1.0f, -1.0f, -1.0f),
            // right
            Vec3(1.0f, -1.0f, -1.0f),
            Vec3(1.0f,  1.0f, -1.0f),
            Vec3(1.0f,  1.0f,  1.0f),
            Vec3(1.0f, -1.0f,  1.0f)
        };

        std::vector<Vec2> vertex_uv =
        {
            // front
            Vec2(0.0f, 1.0f),
            Vec2(0.0f, 0.0f),
            Vec2(1.0f, 0.0f),
            Vec2(1.0f, 1.0f),
            // back
            Vec2(1.0f, 1.0f),
            Vec2(0.0f, 1.0f),
            Vec2(0.0f, 0.0f),
            Vec2(1.0f, 0.0f),
            // top
            Vec2(0.0f, 1.0f),
            Vec2(0.0f, 0.0f),
            Vec2(1.0f, 0.0f),
            Vec2(1.0f, 1.0f),
            // botttom
            Vec2(1.0f, 1.0f),
            Vec2(0.0f, 1.0f),
            Vec2(0.0f, 0.0f),
            Vec2(1.0f, 0.0f),
            // left
            Vec2(0.0f, 1.0f),
            Vec2(0.0f, 0.0f),
            Vec2(1.0f, 0.0f),
            Vec2(1.0f, 1.0f),
            // right
            Vec2(0.0f, 1.0f),
            Vec2(0.0f, 0.0f),
            Vec2(1.0f, 0.0f),
            Vec2(1.0f, 1.0f)
        };

        index_buffer = MakeShared<IndexBuffer>(context, vertex_indices.data(), (uint32)vertex_indices.size());
        pos = MakeShared<VertexBuffer>(context, vertex_pos.data(), (uint32)vertex_pos.size(), sizeof(Vec3), VertexBufferSlots::POS);
        normals = MakeShared<VertexBuffer>(context, vertex_normals.data(), (uint32)vertex_normals.size(), sizeof(Vec3), VertexBufferSlots::NORMALS);
        uv = MakeShared<VertexBuffer>(context, vertex_uv.data(), (uint32)vertex_uv.size(), sizeof(Vec2), VertexBufferSlots::TEX_COORD);
    }
};

struct Mesh : public IBindable
{
    Mesh();
    ~Mesh();

    void Update(float dt);
    void Render(GraphicsContext& context);

    virtual void Bind(GraphicsContext& context) override;

    float rotation_angle_ = 0.0f;
    Transform transform_;

    SharedPtr<MeshData> mesh_data_;
    SharedPtr<Material> material_;

    CBufferPerObject per_object_data_;
    ComPtr<ID3D11Buffer> cbuffer_per_object_ = nullptr;
};

//struct Model
//{
//    std::vector<Mesh> 
//};