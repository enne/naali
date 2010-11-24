// For conditions of distribution and use, see copyright notice in license.txt

#include "StableHeaders.h"
#include "OgreRenderingModule.h"
#include "Renderer.h"
#include "Entity.h"
#include "EC_Placeable.h"
#include "EC_OgreCustomObject.h"

#include <Ogre.h>

using namespace OgreRenderer;

EC_OgreCustomObject::EC_OgreCustomObject(IModule* module) :
    IComponent(module->GetFramework()),
    renderer_(checked_static_cast<OgreRenderingModule*>(module)->GetRenderer()),
    entity_(0),
    attached_(false),
    cast_shadows_(false),
    draw_distance_(0.0f)
{
}

EC_OgreCustomObject::~EC_OgreCustomObject()
{
    if (renderer_.expired())
        return;

    DestroyEntity();
}

void EC_OgreCustomObject::SetPlaceable(ComponentPtr placeable)
{
    if (!dynamic_cast<EC_Placeable*>(placeable.get()))
    {
        OgreRenderingModule::LogError("Attempted to set placeable which is not " + EC_Placeable::TypeNameStatic().toStdString());
        return;
    }
    
    DetachEntity();
    placeable_ = placeable;
    AttachEntity();
}

bool EC_OgreCustomObject::CommitChanges(Ogre::ManualObject* object)
{
    if (!object)
        return false;
    
    if (renderer_.expired())
        return false;
    RendererPtr renderer = renderer_.lock();
            
    DestroyEntity();
    
    // If placeable is not set yet, set it manually by searching it from the parent entity
    if (!placeable_)
    {
        Scene::Entity* entity = GetParentEntity();
        if (entity)
        {
            ComponentPtr placeable = entity->GetComponent(EC_Placeable::TypeNameStatic());
            if (placeable)
                placeable_ = placeable;
        }
    }
    
    if (!object->getNumSections())
        return true;
        
    try
    {
        std::string mesh_name = renderer->GetUniqueObjectName();
        object->convertToMesh(mesh_name);
        object->clear();
    
        Ogre::SceneManager* scene_mgr = renderer->GetSceneManager();

        entity_ = scene_mgr->createEntity(renderer->GetUniqueObjectName(), mesh_name);
        if (entity_)
        {
            AttachEntity();
            entity_->setRenderingDistance(draw_distance_);
            entity_->setCastShadows(cast_shadows_);
            entity_->setUserAny(Ogre::Any(GetParentEntity()));
            // Set UserAny also on subentities
            for (uint i = 0; i < entity_->getNumSubEntities(); ++i)
                entity_->getSubEntity(i)->setUserAny(entity_->getUserAny());
        }
        else
        {
            OgreRenderingModule::LogError("Could not create entity from manualobject mesh");
            return false;
        }
    }   
    catch (Ogre::Exception& e)
    {
        OgreRenderingModule::LogError("Could not convert manualobject to mesh: " + std::string(e.what()));
        return false;
    }
    
    return true;
}

void EC_OgreCustomObject::SetDrawDistance(float draw_distance)
{
    draw_distance_ = draw_distance;
    if (entity_)
        entity_->setRenderingDistance(draw_distance);
}

void EC_OgreCustomObject::SetCastShadows(bool enabled)
{
    cast_shadows_ = enabled;
    if (entity_)
        entity_->setCastShadows(enabled);
}

bool EC_OgreCustomObject::SetMaterial(uint index, const std::string& material_name)
{
    if (!entity_)
        return false;
    
    if (index >= entity_->getNumSubEntities())
    {
        OgreRenderingModule::LogError("Could not set material " + material_name + ": illegal submesh index " + ToString<uint>(index));
        return false;
    }
    
    try
    {
        entity_->getSubEntity(index)->setMaterialName(material_name);
    }
    catch (Ogre::Exception& e)
    {
        OgreRenderingModule::LogError("Could not set material " + material_name + ": " + std::string(e.what()));
        return false;
    }
    
    return true;
}

uint EC_OgreCustomObject::GetNumMaterials() const
{
    if (!entity_)
        return 0;
        
    return entity_->getNumSubEntities();
}

const std::string& EC_OgreCustomObject::GetMaterialName(uint index) const
{
    const static std::string empty;
    
    if (!entity_)
        return empty;
    
    if (index >= entity_->getNumSubEntities())
        return empty;
    
    return entity_->getSubEntity(index)->getMaterialName();
}    
       
void EC_OgreCustomObject::AttachEntity()
{
    if ((placeable_) && (!attached_) && (entity_))
    {
        EC_Placeable* placeable = checked_static_cast<EC_Placeable*>(placeable_.get());
        Ogre::SceneNode* node = placeable->GetSceneNode();
        node->attachObject(entity_);
        attached_ = true;
    }
}

void EC_OgreCustomObject::DetachEntity()
{
    if ((placeable_) && (attached_) && (entity_))
    {
        EC_Placeable* placeable = checked_static_cast<EC_Placeable*>(placeable_.get());
        Ogre::SceneNode* node = placeable->GetSceneNode();
        node->detachObject(entity_);
        attached_ = false;
    }
}

void EC_OgreCustomObject::DestroyEntity()
{
    if (renderer_.expired())
        return;
    RendererPtr renderer = renderer_.lock();
            
    Ogre::SceneManager* scene_mgr = renderer->GetSceneManager();
    
    if (entity_)
    {
        DetachEntity();
        std::string mesh_name = entity_->getMesh()->getName();
        scene_mgr->destroyEntity(entity_);
        entity_ = 0;
        try
        {
            Ogre::MeshManager::getSingleton().remove(mesh_name);
        }
        catch (...) {}
    }
}

void EC_OgreCustomObject::GetBoundingBox(Vector3df& min, Vector3df& max) const
{
    if (!entity_)
    {
        min = Vector3df(0.0, 0.0, 0.0);
        max = Vector3df(0.0, 0.0, 0.0);
        return;
    }
 
    const Ogre::AxisAlignedBox& bbox = entity_->getBoundingBox();//getMesh()->getBounds();
    const Ogre::Vector3& bboxmin = bbox.getMinimum();
    const Ogre::Vector3& bboxmax = bbox.getMaximum();
    
    min = Vector3df(bboxmin.x, bboxmin.y, bboxmin.z);
    max = Vector3df(bboxmax.x, bboxmax.y, bboxmax.z);
}

