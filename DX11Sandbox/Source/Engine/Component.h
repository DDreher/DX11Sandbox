#pragma once
#include "Engine/Transform.h"
#include "Renderer/Mesh.h"

enum class ComponentType : uint32
{
    Transform,
    StaticMesh,
    Camera,
    Invalid
};

class IComponent
{
public:
    virtual void Update(double dt) {}
    virtual ComponentType GetType() const = 0;

private:
};

template<ComponentType T>
class BaseComponent : public IComponent
{
public:
    static inline constexpr ComponentType Type = T;
    virtual ComponentType GetType() const final { return T; };
};

class TransformComponent : public BaseComponent<ComponentType::Transform>, public Transform
{
public:
    void Update(double dt) override;

};

class StaticMeshComponent : public BaseComponent<ComponentType::StaticMesh>
{
public:
    StaticMeshComponent()
    {
    }

    StaticMeshComponent(const String& model_path, Transform import_transform)
    {
        model_ = ModelImporter::LoadFromFile({ model_path, import_transform });
    }

    SharedPtr<Model> model_;
};

class CameraComponent : public BaseComponent<ComponentType::Camera>
{
};
