#include "Engine/Entity.h"

Entity::Entity()
{
    transform_ = AddComponent<TransformComponent>();
}

Entity::~Entity()
{
    for(IComponent* c : components_)
    {
        if(c != nullptr)
        {
            delete c;
            c = nullptr;
        }
    }
}

//Handle<Entity> EntityManager::CreateEntity()
//{
//    Handle<Entity> out_handle = entity_pool_.Create();
//    //Entity* entity = Get(out_handle);
//    return out_handle;
//}
//
//Entity* EntityManager::Get(const Handle<Entity> entity)
//{
//    return entity_pool_.Get(entity);
//}
//
//void EntityManager::Remove(const Handle<Entity> entity)
//{
//    entity_pool_.Destroy(entity);
//}
//
//bool EntityManager::Has(const Handle<Entity> entity)
//{
//    return entity_pool_.Get(entity) != nullptr;
//}
