#pragma once
#include "Engine/Component.h"

class Entity
{
public:
    Entity();
    ~Entity();

    void Update(double dt)
    {
        for(auto& component : components_)
        {
            component->Update(dt);
        }
    }

    template<typename T, typename... Args>
    T* AddComponent(Args&&... args)
    {
        T* component = new T(std::forward<Args>(args)...);
        components_.push_back(component);
        return component;
    }

    template<typename T>
    T* GetComponent()
    {
        for(IComponent* c : components_)
        {
            if(c->GetType() == T::Type)
            {
                return (T*) c;
            }
        }

        return nullptr;
    }

    void AddChild(Entity* entity)
    {
        CHECK(entity != nullptr);
        transform_->AddChild(entity->transform_);
    }

    String name_;
    TransformComponent* transform_;

private:
    std::vector<IComponent*> components_;
};
