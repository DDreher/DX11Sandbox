#pragma once

#include "Engine/Entity.h"

class World 
{
public:
    World() = default;
    ~World() = default;

    void Update(double dt);

    void Add(const SharedPtr<Entity>& entity);

    const std::vector<SharedPtr<Entity>>& GetEntities() const;

private:
    std::vector<SharedPtr<Entity>> entities_;
};
