#pragma once
#include "Engine/Transform.h"
#include "Renderer/Mesh.h"

enum class ComponentType : uint32
{
    Transform,
    StaticMesh,
    Camera,
    DirectionalLight,
    PointLight,
    SpotLight,
    Invalid
};

class IComponent
{
public:
    virtual void Update() {}
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
    void Update() override;
};

class StaticMeshComponent : public BaseComponent<ComponentType::StaticMesh>
{
public:
    StaticMeshComponent()
    {
    }

    StaticMeshComponent(const String& model_path, Transform import_transform)
    {
        model_ = MeshImporter::LoadFromFile({ model_path, import_transform });
    }

    SharedPtr<Model> model_;
    bool is_shadow_receiver_ = true;
    bool is_visible_ = true;
};

class CameraComponent : public BaseComponent<ComponentType::Camera>
{
};

class DirectionalLightComponent : public BaseComponent<ComponentType::DirectionalLight>
{
public:
    bool is_enabled_ = true;
    Vec3 color_ = Vec3(1.0f, 1.0f, 1.0f);
    float brightness_ = 1.0f;
    float ambient_intensity_ = 0.05f;
};

class PointLightComponent : public BaseComponent<ComponentType::PointLight>
{
public:
    bool is_enabled_ = true;
    Vec3 color_ = Vec3(1.0f, 1.0f, 1.0f);
    float brightness_ = 1.0f;
    float ambient_intensity_ = 0.01f;
    float attenuation_ = 1.0f;
};

class SpotLightComponent : public BaseComponent<ComponentType::SpotLight>
{
public:
    bool is_enabled_ = true;
    Vec3 color_ = Vec3(1.0f, 1.0f, 1.0f);
    float brightness_ = 1.0f;
    float ambient_intensity_ = 0.01f;
    float cone_angle_ = 45.0f;
    float attenuation_ = 1.0f;
};
