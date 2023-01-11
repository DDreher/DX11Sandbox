#pragma once
#include "Engine/World.h"
#include "Renderer/Mesh.h"

struct ModelDesc
{
    String path;
    Transform import_correction_transform;
};

struct aiScene;
struct aiNode;

class ModelImporter
{
public:
    static SharedPtr<Entity> ImportModel(const ModelDesc& model_desc, World& world);

private:

    static SharedPtr<Entity> ProcessNode(const ModelDesc& model_desc, const aiScene* scene, aiNode* node, Entity* parent, World& world);
    static SharedPtr<Model> ProcessMesh(const ModelDesc& model_desc, const aiScene* scene, aiNode* node, uint32 mesh_idx);
};
