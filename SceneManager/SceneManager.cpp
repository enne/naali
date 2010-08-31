// For conditions of distribution and use, see copyright notice in license.txt

#include "StableHeaders.h"
#include "DebugOperatorNew.h"

#include "SceneManager.h"
#include "Entity.h"
#include "SceneEvents.h"
#include "Framework.h"
#include "ComponentManager.h"
#include "EventManager.h"
#include "ComponentInterface.h"
#include "ForwardDefines.h"

#include "MemoryLeakCheck.h"

namespace Scene
{
    uint SceneManager::gid_ = 0;

    SceneManager::~SceneManager()
    {
        EntityMap::iterator it = entities_.begin();
        while (it != entities_.end())
        {
            // If entity somehow manages to live, at least it doesn't belong to the scene anymore
            it->second->SetScene(0);
            ++it;
        }
        entities_.clear();
    }
    
    Scene::EntityPtr SceneManager::CreateEntity(entity_id_t id, const QStringList &components, AttributeChange::Type change)
    {
        // Figure out new entity id
        entity_id_t newentityid = 0;
        if(id == 0)
            newentityid = GetNextFreeId();
        else
        {
            if(entities_.find(id) != entities_.end())
            {
                Foundation::RootLogError("Can't create entity with given id because it's already used: " + ToString(id));
                return Scene::EntityPtr();
            }
            else
                newentityid = id;
        }

        Scene::EntityPtr entity = Scene::EntityPtr(new Scene::Entity(framework_, newentityid, this));
        for (size_t i=0 ; i<components.size() ; ++i)
            entity->AddComponent(framework_->GetComponentManager()->CreateComponent(components[i])); //change the param to a qstringlist or so \todo XXX

        entities_[entity->GetId()] = entity;

        EmitEntityCreated(entity.get(), change);
        
        // Send event.
        Events::SceneEventData event_data(entity->GetId());
        event_category_id_t cat_id = framework_->GetEventManager()->QueryEventCategory("Scene");
        framework_->GetEventManager()->SendEvent(cat_id, Events::EVENT_ENTITY_ADDED, &event_data);

        return entity;
    }

    Scene::EntityPtr SceneManager::GetEntity(entity_id_t id) const
    {
        EntityMap::const_iterator it = entities_.find(id);
        if (it != entities_.end())
            return it->second;

        return Scene::EntityPtr();
    }

    entity_id_t SceneManager::GetNextFreeId()
    {
        while(entities_.find(gid_) != entities_.end())
            gid_ = (gid_ + 1) % static_cast<uint>(-1);

        return gid_;
    }

    void SceneManager::RemoveEntity(entity_id_t id, AttributeChange::Type change)
    {
        EntityMap::iterator it = entities_.find(id);
        if (it != entities_.end())
        {
            Scene::EntityPtr del_entity = it->second;

            EmitEntityRemoved(del_entity.get(), change);
        
            // Send event.
            Events::SceneEventData event_data(id);
            event_category_id_t cat_id = framework_->GetEventManager()->QueryEventCategory("Scene");
            framework_->GetEventManager()->SendEvent(cat_id, Events::EVENT_ENTITY_DELETED, &event_data);

            entities_.erase(it);
            // If entity somehow manages to live, at least it doesn't belong to the scene anymore
            del_entity->SetScene(0);
            del_entity.reset();
        }
    }

    EntityList SceneManager::GetEntitiesWithComponent(const QString &type_name)
    {
        std::list<EntityPtr> entities;
        EntityMap::const_iterator it = entities_.begin();
        while(it != entities_.end())
        {
            EntityPtr entity = it->second;
            if (entity->HasComponent(type_name))
                entities.push_back(entity);
            ++it;
        }

        return entities;
    }
    
    void SceneManager::EmitComponentChanged(Foundation::ComponentInterface* comp, AttributeChange::Type change)
    {
        emit ComponentChanged(comp, change);
    }
    
    void SceneManager::EmitComponentAdded(Scene::Entity* entity, Foundation::ComponentInterface* comp, AttributeChange::Type change)
    {
        emit ComponentAdded(entity, comp, change);
    }
    
    void SceneManager::EmitComponentRemoved(Scene::Entity* entity, Foundation::ComponentInterface* comp, AttributeChange::Type change)
    {
        emit ComponentRemoved(entity, comp, change);
    }

    void SceneManager::EmitAttributeChanged(Foundation::ComponentInterface* comp, Foundation::AttributeInterface* attribute, AttributeChange::Type change)
    {
        emit AttributeChanged(comp, attribute, change);
    }

  /*void SceneManager::EmitComponentInitialized(Foundation::ComponentInterface* comp)
    {
        emit ComponentInitialized(comp);
        }*/
 
    void SceneManager::EmitEntityCreated(Scene::Entity* entity, AttributeChange::Type change)
    {
        emit EntityCreated(entity, change);
    }
    
    void SceneManager::EmitEntityRemoved(Scene::Entity* entity, AttributeChange::Type change)
    {
        emit EntityRemoved(entity, change);
    }

    QVariantList SceneManager::GetEntityIdsWithComponent(const QString &type_name)
    {
        EntityList list = GetEntitiesWithComponent(type_name);
        QVariantList ids;
        EntityList::iterator iter;

        for(iter = list.begin(); iter != list.end(); iter++)
        {
            EntityPtr ent = *iter;
            Entity* e = ent.get();
            entity_id_t id = e->GetId();
            QVariant qv(id);
            ids.append(qv);
        }
        return ids;
    }
}

