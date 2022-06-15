#include "Engine/World.h"

void World::Update(double dt)
{
    for(auto entity : entities_)
    {
        entity->Update(dt);
    }
}

void World::Add(const SharedPtr<Entity>& entity)
{
    entities_.push_back(entity);
}

const std::vector<SharedPtr<Entity>>& World::GetEntities() const
{
    return entities_;
}
