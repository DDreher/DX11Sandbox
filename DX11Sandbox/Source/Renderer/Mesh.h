#pragma once
#include "Engine/Transform.h"
#include "Renderer/ConstantBufferTypes.h"
#include "Renderer/GraphicsContext.h"
#include "Renderer/IndexBuffer.h"
#include "Renderer/Material.h"
#include "Renderer/VertexBuffer.h"

struct aiScene;
struct aiNode;
struct aiMesh;

struct VertexData {
    std::vector<uint16> indices;
    std::vector<Vec3> pos;
    std::vector<Vec3> normals;
    std::vector<Vec2> uvs;
};

struct CubeMeshData
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
};

struct StaticMesh
{
    void PrepareRender();
    void Bind() const;
    void Render() const;

    uint32 start_idx = 0;
    uint32 num_indices = 0;
    uint32 offset = 0;
    uint32 material_slot = 0;

    SharedPtr<IndexBuffer> index_buffer;
    SharedPtr<VertexBuffer> pos;
    SharedPtr<VertexBuffer> uv;
    SharedPtr<VertexBuffer> normals;
    struct Model* model = nullptr;
};

struct Model
{
    void Bind();
    void Render();

    CBufferPerObject per_object_data;
    ComPtr<ID3D11Buffer> cbuffer_per_object = nullptr;
    std::vector<Handle<Material>> materials_;
    std::vector<StaticMesh> meshes_;

    Transform transform;
    SharedPtr<IndexBuffer> index_buffer;
    SharedPtr<VertexBuffer> pos;
    SharedPtr<VertexBuffer> uv;
    SharedPtr<VertexBuffer> normals;
};

struct MeshFileDesc
{
    String path;
    Transform correction_transform;
};

class MeshImporter
{
public:
    static SharedPtr<Model> LoadFromFile(const MeshFileDesc& desc);
};
