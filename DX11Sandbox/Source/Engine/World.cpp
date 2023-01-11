#include "Engine/World.h"
#include "Renderer/GraphicsContext.h"

void World::Update()
{
    for(auto entity : entities_)
    {
        entity->Update();
    }

    gfx::camera.Update();
}

void World::Add(const SharedPtr<Entity>& entity)
{
    entities_.push_back(entity);
}

const std::vector<SharedPtr<Entity>>& World::GetEntities() const
{
    return entities_;
}
