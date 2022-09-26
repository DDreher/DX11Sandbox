#include "Core/ModelImporter.h"

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "assimp/GltfMaterial.h"

#include "Engine/World.h"
#include "Engine/Entity.h"

SharedPtr<Entity> ModelImporter::ImportModel(const ModelDesc& scene_desc, World& world)
{
    LOG("Loading Scene: {}", scene_desc.path);
    auto root_path = std::filesystem::path(scene_desc.path).parent_path();

    SharedPtr<Model> model = MakeShared<Model>();

    Assimp::Importer ai_importer;
    uint32 importer_flags = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_MaxQuality;
    const aiScene* ai_scene = ai_importer.ReadFile(scene_desc.path, importer_flags);
    CHECK_MSG(ai_scene != nullptr, "Failed to load mesh from file: {}. \n Error: {}", scene_desc.path, ai_importer.GetErrorString());

    aiNode* root = ai_scene->mRootNode;
    SharedPtr<Entity> entity = ProcessNode(scene_desc, ai_scene, root, nullptr, world);
    return entity;
}

SharedPtr<Entity> ModelImporter::ProcessNode(const ModelDesc& scene_desc, const aiScene* scene, aiNode* node, Entity* parent, World& world)
{
    SharedPtr<Entity> entity = MakeShared<Entity>();
    entity->name_ = node->mName.C_Str();
    world.Add(entity);

    bool apply_correction_transform = true; // If we want to correct the import, we only have to touch the first node in the tree.
    if(parent != nullptr)
    {
        apply_correction_transform = false;
        parent->AddChild(entity.get());
    }

    aiVector3D scaling;
    aiQuaternion rotation;
    aiVector3D translation;
    node->mTransformation.Decompose(scaling, rotation, translation);
    
    Transform import_transform = {
        { scaling.x, scaling.y, scaling.z },
        { rotation.x, rotation.y, rotation.z, rotation.w },
        { translation.x, translation.y, translation.z }
    };

    if(apply_correction_transform)
    {
        import_transform = import_transform * scene_desc.import_correction_transform;
    }

    entity->transform_->SetLocalTransform(import_transform);

    for(uint32 mesh_idx = 0; mesh_idx < node->mNumMeshes; ++mesh_idx)
    {
        SharedPtr<Entity> mesh_entity = MakeShared<Entity>();
        mesh_entity->name_ = node->mName.C_Str();

        StaticMeshComponent* mesh_component = mesh_entity->AddComponent<StaticMeshComponent>();
        mesh_component->model_ = ModelImporter::ProcessMesh(scene_desc, scene, node, mesh_idx);

        world.Add(mesh_entity);
        entity->AddChild(mesh_entity.get());
    }

    for (uint32 i = 0; i < node->mNumChildren; ++i)
    {
        ProcessNode(scene_desc, scene, node->mChildren[i], entity.get(), world);
    }

    return entity;
}

SharedPtr<Model> ModelImporter::ProcessMesh(const ModelDesc& scene_desc, const aiScene* scene, aiNode* node, uint32 mesh_idx)
{
    SharedPtr<Model> model = MakeShared<Model>();
    auto root_path = std::filesystem::path(scene_desc.path).parent_path();

    VertexData vertex_data;
    uint32 num_model_indices = 0;
    uint32 num_model_vertices = 0;

    aiMesh* ai_mesh = scene->mMeshes[node->mMeshes[mesh_idx]];
    aiMaterial* ai_material = scene->mMaterials[ai_mesh->mMaterialIndex];

    BlendState material_blendstate = BlendState::Opaque;
    aiString blend_mode_string;
    ai_material->Get(AI_MATKEY_GLTF_ALPHAMODE, blend_mode_string);

    int shading_model;
    ai_material->Get(AI_MATKEY_SHADING_MODEL, shading_model);

    bool is_alpha_cutoff = false;
    float alpha_cutoff = 0.0f;

    if (blend_mode_string == aiString("MASK"))
    {
        is_alpha_cutoff = true;
        alpha_cutoff = 1.0f;
        ai_material->Get(AI_MATKEY_GLTF_ALPHACUTOFF, alpha_cutoff);
        material_blendstate = BlendState::NonPremultipliedAlpha;
    }
    else if (blend_mode_string == aiString("BLEND"))
    {
        material_blendstate = BlendState::NonPremultipliedAlpha;
    }

    int is_two_sided = 0;
    ai_material->Get(AI_MATKEY_TWOSIDED, is_two_sided);

    MaterialDesc mat_desc_textured
    {
        .vs_path = "assets/shaders/forward_unlit_vs.hlsl",
        .ps_path = "assets/shaders/forward_unlit_ps.hlsl",
        .rasterizer_state = is_two_sided ? RasterizerState::CullNone : RasterizerState::CullCounterClockwise,
        .blend_state = material_blendstate,
        .depth_stencil_state = DepthStencilState::Default,
        .is_alpha_cutoff = is_alpha_cutoff,
        .alpha_cutoff_val = alpha_cutoff
    };

    Handle<Material> mat_handle = gfx::resource_manager->materials.Create(mat_desc_textured);
    Material* mat = gfx::resource_manager->materials.Get(mat_handle);

    aiString diffuse_tex_path;
    if (ai_material->GetTexture(aiTextureType_BASE_COLOR, 0, &diffuse_tex_path) == AI_SUCCESS &&
        diffuse_tex_path.C_Str() != "")
    {
        std::filesystem::path tex_path = root_path / std::filesystem::path(diffuse_tex_path.C_Str());
        Handle<Texture> tex = gfx::resource_manager->textures.GetHandle(TextureDesc{ tex_path.string() });
        mat->SetTexture("tex", tex);
    }

    model->materials_.push_back(mat_handle);

    // Geometry
    uint32 num_mesh_indices = 0;
    for (uint32 i = 0; i < ai_mesh->mNumFaces; ++i)
    {
        const aiFace& face = ai_mesh->mFaces[i];
        CHECK(face.mNumIndices == 3);
        vertex_data.indices.push_back(num_model_vertices + face.mIndices[0]);
        vertex_data.indices.push_back(num_model_vertices + face.mIndices[1]);
        vertex_data.indices.push_back(num_model_vertices + face.mIndices[2]);
        num_mesh_indices += 3;
    }
    num_model_indices += num_mesh_indices;

    for (uint32 vertex_id = 0; vertex_id < ai_mesh->mNumVertices; ++vertex_id)
    {
        const aiVector3D& vertex = ai_mesh->mVertices[vertex_id];
        vertex_data.pos.push_back({ vertex.x, vertex.y, vertex.z });

        const aiVector3D& normal = ai_mesh->HasNormals() ? ai_mesh->mNormals[vertex_id] : aiVector3D(0.0f, 0.0f, 0.0f);
        vertex_data.normals.push_back({ normal.x, normal.y, normal.z });

        const aiVector3D& uv = ai_mesh->HasTextureCoords(0) ? ai_mesh->mTextureCoords[0][vertex_id] : aiVector3D(0.0f, 0.0f, 0.0f);
        vertex_data.uvs.push_back({ uv.x, uv.y });

        num_model_vertices++;
    }

    StaticMesh mesh;
    mesh.start_idx = num_model_indices - num_mesh_indices;
    mesh.offset = 0;
    mesh.num_indices = num_mesh_indices;
    mesh.material_slot = (uint32) (model->materials_.size() - 1);
    mesh.model = model.get();
    model->meshes_.push_back(mesh);

    D3D11_BUFFER_DESC cbuffer_desc = {};
    cbuffer_desc.Usage = D3D11_USAGE_DEFAULT;   // Read / Write access
    cbuffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbuffer_desc.ByteWidth = sizeof(CBufferPerObject);
    cbuffer_desc.CPUAccessFlags = 0;
    DX11_VERIFY(gfx::device->CreateBuffer(&cbuffer_desc, nullptr, &model->cbuffer_per_object));

    model->index_buffer = MakeShared<IndexBuffer>(vertex_data.indices.data(), (uint32)vertex_data.indices.size());
    model->pos = MakeShared<VertexBuffer>(vertex_data.pos.data(), (uint32)vertex_data.pos.size(), sizeof(Vec3), VertexBufferSlots::POS);
    model->normals = MakeShared<VertexBuffer>(vertex_data.normals.data(), (uint32)vertex_data.normals.size(), sizeof(Vec3), VertexBufferSlots::NORMALS);
    model->uv = MakeShared<VertexBuffer>(vertex_data.uvs.data(), (uint32)vertex_data.uvs.size(), sizeof(Vec2), VertexBufferSlots::TEX_COORD);

    for (StaticMesh& mesh : model->meshes_)
    {
        mesh.index_buffer = model->index_buffer;
        mesh.pos = model->pos;
        mesh.normals = model->normals;
        mesh.uv = model->uv;
    }

    return model;
}
