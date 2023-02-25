#include "AppShadowMapping.h"

#include "backends/imgui_impl_sdl.h"
#include "SDL_events.h"
#include "spdlog/fmt/bundled/ostream.h"

#include "Engine/Entity.h"

#include "Core/SceneImporter.h"
#include "Renderer/Renderer.h"

AppShadowMapping::AppShadowMapping()
{
    application_name_ = "Shadow Mapping";
}

void AppShadowMapping::Init()
{
    BaseApplication::Init();

    // Meshes
    {
        String model_path = "assets/models/sponza/Sponza.gltf";
        Transform import_correction_transform;
        import_correction_transform.SetWorldRotation(Quat::FromAxisAngle(Vec3::UP, MathUtils::DegToRad(90.0f)));
        import_correction_transform.SetWorldTranslation({ 0.0f, 0.0f, 0.0f });

        SceneDescription scene_desc
        {
            .path = model_path,
            .import_correction_transform = import_correction_transform
        };
        SceneImporter::ImportScene(scene_desc, world);
    }

    {
        String model_path = "assets/models/dragon/scene.gltf";
        Transform import_correction_transform;
        import_correction_transform.SetWorldScaling({ 0.01f, 0.01f, 0.01f });
        import_correction_transform.SetWorldRotation(Quat::FromAxisAngle(Vec3::UP, MathUtils::DegToRad(45.0f)));
        import_correction_transform.SetWorldTranslation({ 0.5f, -.05f, -6.0f });

        SceneDescription scene_desc
        {
            .path = model_path,
            .import_correction_transform = import_correction_transform
        };
        SceneImporter::ImportScene(scene_desc, world);
    }

    // Unlit material for lights
    MaterialDesc unlit_mat_desc
    {
        .vs_path = "assets/shaders/forward_phong_vs.hlsl",
        .ps_path = "assets/shaders/forward_phong_ps.hlsl",
        .rasterizer_state = RasterizerState::CullCounterClockwise,
        .blend_state = BlendState::Opaque,
        .depth_stencil_state = DepthStencilState::Default,
        .is_alpha_cutoff = false,
        .is_lit = false
    };
    Handle<Material> unlit_mat_handle = gfx::resource_manager->materials.Create(unlit_mat_desc);
    Material* unlit_material = gfx::resource_manager->materials.Get(unlit_mat_handle);
    CHECK(unlit_material != nullptr);
    unlit_material->SetParam("base_color", Vec3{ 1.0f, 1.0f, 1.0f });
    unlit_material->SetParam("bound_texture_bits", 0);

    // Lights
    {
        SharedPtr<Entity> entity = MakeShared<Entity>();
        entity->name_ = "DirectionalLight";
        DirectionalLightComponent* directional_light = entity->AddComponent<DirectionalLightComponent>();
        directional_light->brightness_ = 0.7f;
        entity->transform_->SetWorldRotation(Quat::FromAxisAngle(Vec3::RIGHT, MathUtils::DegToRad(66.0f)));
        world.Add(entity);
    }

    {
        SharedPtr<Entity> entity = MakeShared<Entity>();
        entity->transform_->SetWorldTranslation({ 0.0f, 1.0f, -5.0f });
        entity->transform_->SetWorldRotation(Quat::FromAxisAngle(Vec3::UP, MathUtils::DegToRad(180.0f)));
        entity->name_ = "SpotLight";
        SpotLightComponent* light = entity->AddComponent<SpotLightComponent>();
        light->color_ = { 1.0f, 0.0f, 0.0f };
        light->cone_angle_ = 33.33f;
        world.Add(entity);
    }

    {
        String model_path = "assets/meshes/sphere.gltf";
        Transform import_correction_transform;
        import_correction_transform.SetWorldTranslation({ 0.0f, 1.0f, -3.0f });
        import_correction_transform.SetWorldScaling({ 0.33f, 0.33f, 0.33f });

        SceneDescription scene_desc
        {
            .path = model_path,
            .import_correction_transform = import_correction_transform
        };

        SharedPtr<Entity> entity = SceneImporter::ImportScene(scene_desc, world);
        entity->transform_->SetWorldTranslation({ 0.0f, 1.0f, -5.0f });
        entity->transform_->SetWorldRotation(Quat::FromAxisAngle(Vec3::UP, MathUtils::DegToRad(180.0f)));
        entity->name_ = "PointLight";
        
        PointLightComponent* light = entity->AddComponent<PointLightComponent>();
        light->color_ = { 0.0f, 0.0f, 1.0f };

        StaticMeshComponent* mesh_component = entity->GetChild(0)->GetComponent<StaticMeshComponent>();
        mesh_component->model_->materials_[0] = unlit_mat_handle;
        mesh_component->is_shadow_receiver_ = false;
    }
}

void AppShadowMapping::Cleanup()
{
    BaseApplication::Cleanup();
}

void AppShadowMapping::Update()
{
    BaseApplication::Update();

    for (auto entity : world.GetEntities())
    {
        // Update material base color for light representations
        if(PointLightComponent* light_component = entity->GetComponent<PointLightComponent>())
        {
            if(entity->GetChild(0))
            {
                if(StaticMeshComponent* mesh_component = entity->GetChild(0)->GetComponent<StaticMeshComponent>())
                {
                    if (light_component != nullptr && mesh_component != nullptr)
                    {
                        Material* material = gfx::resource_manager->materials.Get(mesh_component->model_->materials_[0]);
                        const Vec3 light_color = light_component->brightness_ * light_component->color_;
                        material->SetParam("base_color", light_color);
                        static Vec3 initial_scale = entity->GetChild(0)->transform_->GetWorldScaling();
                        Vec3 scale = initial_scale * light_component->brightness_;
                        entity->GetChild(0)->transform_->SetWorldScaling(scale);
                        mesh_component->is_visible_ = light_component->is_enabled_;
                    }
                }
            }
        }
    }
}

void AppShadowMapping::Render()
{
    for (auto entity : world.GetEntities())
    {
        StaticMeshComponent* mesh_component = entity->GetComponent<StaticMeshComponent>();
        if(mesh_component != nullptr && mesh_component->model_ != nullptr)
        {
            if (mesh_component->is_visible_)
            {
                mesh_component->model_->transform = *entity->transform_;
                for(StaticMesh& mesh : mesh_component->model_->meshes_)
                {
                    RenderWorkItem item;
                    item.mesh = &mesh;
                    item.sort_key = Vec3::DistanceSquared(gfx::camera.GetPosition(), mesh_component->model_->transform.GetWorldTranslation());
                    // Note: For now we calculate the distance from camera to entity... This is not ideal, especially for the render order of meshes w/ transparent materials.
                    // Problems for future me, I guess :>
                    item.is_shadow_receiver = mesh_component->is_shadow_receiver_;

                    Material* material = gfx::resource_manager->materials.Get(mesh_component->model_->materials_[mesh.material_slot]);
                    CHECK(material != nullptr);
                    gfx::renderer->Enqueue(item, material->blend_state_);
                }
            }
        }

        {
            DirectionalLightComponent* light = entity->GetComponent<DirectionalLightComponent>();
            if (light != nullptr && light->is_enabled_)
            {
                Renderer* renderer = (Renderer*) gfx::renderer;
                CHECK(renderer);

                renderer->Enqueue(DirectionalLight(entity->transform_->GetWorldForward().xyz().Normalize(), light->ambient_intensity_,
                    light->color_, light->brightness_));
            }
        }

        {
            PointLightComponent* light = entity->GetComponent<PointLightComponent>();
            if (light != nullptr && light->is_enabled_)
            {
                Renderer* renderer = (Renderer*) gfx::renderer;
                CHECK(renderer);

                const Vec3 light_pos = entity->transform_->GetWorldTranslation();

                PointLight l;
                l.position_ws = light_pos;
                l.ambient_intensity = light->ambient_intensity_;
                l.color = light->color_;
                l.attenuation = light->attenuation_;
                l.brightness = light->brightness_;

                static constexpr int32 NUM_VIEW_DIRS = 6;
                static constexpr int32 IDX_RIGHT = 0;
                static constexpr int32 IDX_LEFT = 1;
                static constexpr int32 IDX_UP = 2;
                static constexpr int32 IDX_DOWN = 3;
                static constexpr int32 IDX_FORWARD = 4;
                static constexpr int32 IDX_BACKWARD = 5;
                static constexpr float ASPECT_RATIO = 1.0f;

                const float near_z = 0.1f;
                const float far_z = 30.0f; // TODO: Max shadow distance should be a global / per light property?

                Mat4 light_projection = Mat4::PerspectiveFovLH(PI_DIV2, ASPECT_RATIO, near_z, far_z);

                // Right
                {
                    const Mat4 light_view = Mat4::LookAt(light_pos, light_pos + Vec3::RIGHT, Vec3::UP);
                    l.view_projections[IDX_RIGHT] = (light_view * light_projection).Transpose();
                }
                
                // Left
                {
                    const Mat4 light_view = Mat4::LookAt(light_pos, light_pos + Vec3::LEFT, Vec3::UP);
                    l.view_projections[IDX_LEFT] = (light_view * light_projection).Transpose();
                }

                // Up
                {
                    const Mat4 light_view = Mat4::LookAt(light_pos, light_pos + Vec3::UP, Vec3::BACKWARD);
                    l.view_projections[IDX_UP] = (light_view * light_projection).Transpose();;
                }

                // Down
                {
                    const Mat4 light_view = Mat4::LookAt(light_pos, light_pos + Vec3::DOWN, Vec3::FORWARD);
                    l.view_projections[IDX_DOWN] = (light_view * light_projection).Transpose();
                }

                // Forward
                {
                    const Mat4 light_view = Mat4::LookAt(light_pos, light_pos + Vec3::FORWARD, Vec3::UP);
                    l.view_projections[IDX_FORWARD] = (light_view * light_projection).Transpose();
                }

                // Backward
                {
                    const Mat4 light_view = Mat4::LookAt(light_pos, light_pos + Vec3::BACKWARD, Vec3::UP);
                    l.view_projections[IDX_BACKWARD] = (light_view * light_projection).Transpose();
                }

                renderer->Enqueue(l);
            }
        }

        {
            SpotLightComponent* light = entity->GetComponent<SpotLightComponent>();
            if (light != nullptr && light->is_enabled_)
            {
                Renderer* renderer = (Renderer*)gfx::renderer;
                CHECK(renderer);

                static constexpr float ASPECT_RATIO = 1.0f;

                const Vec3 light_pos = entity->transform_->GetWorldTranslation();
                const Vec3 light_dir = entity->transform_->GetWorldForward().xyz().Normalize();
                const float near_z = 0.1f;
                const float far_z = 30.0f; // TODO: Max shadow distance should be a global / per light property?

                const Mat4 light_view = Mat4::LookAt(light_pos, light_pos + light_dir, Vec3::UP);
                const Mat4 light_projection = Mat4::PerspectiveFovLH(PI_DIV2, ASPECT_RATIO, near_z, far_z);
                const Mat4 light_view_projection = light_view * light_projection;

                renderer->Enqueue(SpotLight
                    {
                        .position_ws = light_pos,
                        .ambient_intensity = light->ambient_intensity_,
                        .color = light->color_,
                        .attenuation = light->attenuation_,
                        .cone_dir_ws = light_dir,
                        .cos_cone_angle = cosf(MathUtils::DegToRad(light->cone_angle_)),
                        .view_projection = light_view_projection.Transpose(),
                        .brightness = light->brightness_
                    });
            }
        }
    }

    BaseApplication::Render();
}

void AppShadowMapping::RenderUI()
{
    BaseApplication::RenderUI();

    ImGui::Begin("Debug Menu");

    ImGui::Text("Camera");
    Vec3 camera_pos = gfx::camera.GetPosition();
    ImGui::SliderFloat3("Position##Camera", reinterpret_cast<float*>(&camera_pos), -10.0, 10.0f,
        "%.3f", ImGuiSliderFlags_None);
    gfx::camera.SetPosition(camera_pos);
    float fov = MathUtils::RadToDeg(gfx::camera.GetFov());
    ImGui::SliderFloat("FoV", reinterpret_cast<float*>(&fov),
        0.0f, 180.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
    gfx::camera.SetFov(MathUtils::DegToRad(fov));

    for (SharedPtr<Entity> e : world.GetEntities())
    {
        if(DirectionalLightComponent* l = e->GetComponent<DirectionalLightComponent>())
        {
            ImGui::Text("%s", e->name_.c_str());
            ImGui::Checkbox("enabled", &l->is_enabled_);

            Vec3 world_rot = e->transform_->GetWorldRotation().ToEuler();
            ImGui::SliderFloat3(fmt::format("Rotation##{}", e->name_).c_str(), reinterpret_cast<float*>(&world_rot), -2.0, 2.0f,
                "%.3f", ImGuiSliderFlags_None);
            e->transform_->SetWorldRotation(Quat::Normalize(Quat::FromPitchYawRoll(world_rot)));

            ImGui::SliderFloat(fmt::format("Brightness##{}", e->name_).c_str(), reinterpret_cast<float*>(&l->brightness_),
                0.0f, 1.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
            ImGui::ColorPicker3("LightColor", reinterpret_cast<float*>(&l->color_));
            ImGui::SliderFloat("AmbientIntensity", reinterpret_cast<float*>(&l->ambient_intensity_), 0.0f, 1.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
        }

        if (PointLightComponent* l = e->GetComponent<PointLightComponent>())
        {
            ImGui::Text("%s", e->name_.c_str());
            ImGui::Checkbox(fmt::format("is_enabled##{}", e->name_).c_str(), &l->is_enabled_);
            Vec3 pos = e->transform_->GetWorldTranslation();
            ImGui::SliderFloat3(fmt::format("Pos##{}", e->name_).c_str(), reinterpret_cast<float*>(&pos), -10.0f, 10.0f, "%.3f", ImGuiSliderFlags_None);
            e->transform_->SetWorldTranslation(pos);
            ImGui::SliderFloat(fmt::format("Brightness##{}", e->name_).c_str(), reinterpret_cast<float*>(&l->brightness_),
                0.0f, 1.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
            ImGui::ColorPicker3(fmt::format("Color##{}", e->name_).c_str(), reinterpret_cast<float*>(&l->color_));
            ImGui::SliderFloat(fmt::format("AmbientIntensity##{}", e->name_).c_str(), reinterpret_cast<float*>(&l->ambient_intensity_),
                0.0f, 1.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
            ImGui::SliderFloat(fmt::format("Attenuation##{}", e->name_).c_str(), reinterpret_cast<float*>(&l->attenuation_),
                0.0f, 100.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
        }

        if (SpotLightComponent* l = e->GetComponent<SpotLightComponent>())
        {
            ImGui::Text("%s", e->name_.c_str());
            ImGui::Checkbox(fmt::format("is_enabled##{}", e->name_).c_str(), &l->is_enabled_);

            Vec3 pos = e->transform_->GetWorldTranslation();
            ImGui::SliderFloat3(fmt::format("Pos##{}", e->name_).c_str(), reinterpret_cast<float*>(&pos), -15.0f, 15.0f, "%.3f", ImGuiSliderFlags_None);
            e->transform_->SetWorldTranslation(pos);

            Vec3 world_rot = e->transform_->GetWorldRotation().ToEuler();
            ImGui::SliderFloat3(fmt::format("Rot##{}", e->name_).c_str(), reinterpret_cast<float*>(&world_rot), -3.0, 3.0f,
                "%.3f", ImGuiSliderFlags_None);
            e->transform_->SetWorldRotation(Quat::Normalize(Quat::FromPitchYawRoll(world_rot)));
            
            ImGui::SliderFloat(fmt::format("Brightness##{}", e->name_).c_str(), reinterpret_cast<float*>(&l->brightness_),
                0.0f, 1.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
            ImGui::SliderFloat(fmt::format("ConeAngle##{}", e->name_).c_str(), reinterpret_cast<float*>(&l->cone_angle_),
                0.01f, 90.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
            ImGui::ColorPicker3(fmt::format("Color##{}", e->name_).c_str(), reinterpret_cast<float*>(&l->color_));
            ImGui::SliderFloat(fmt::format("AmbientIntensity##{}", e->name_).c_str(), reinterpret_cast<float*>(&l->ambient_intensity_),
                0.0f, 1.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
            ImGui::SliderFloat(fmt::format("Attenuation##{}", e->name_).c_str(), reinterpret_cast<float*>(&l->attenuation_),
                0.0f, 100.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
        }
    }

    ImGui::End();
}

void AppShadowMapping::HandleSDLEvent(const SDL_Event& sdl_event)
{
    BaseApplication::HandleSDLEvent(sdl_event);
    ImGui_ImplSDL2_ProcessEvent(&sdl_event);
}
