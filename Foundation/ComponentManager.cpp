// For conditions of distribution and use, see copyright notice in license.txt

#include "StableHeaders.h"
#include "DebugOperatorNew.h"
#include "ComponentManager.h"
#include "Framework.h"
#include "IComponentFactory.h"
#include "IComponent.h"
#include "IAttribute.h"
#include "AssetInterface.h"
#include "Transform.h"

#include <QVariant>

#include "MemoryLeakCheck.h"

namespace Foundation
{
    ComponentManager::ComponentManager(Framework *framework) : framework_(framework)
    {
        attributeTypes_.push_back("string");
        attributeTypes_.push_back("int");
        attributeTypes_.push_back("real");
        attributeTypes_.push_back("color");
        attributeTypes_.push_back("vector3df");
        attributeTypes_.push_back("bool");
        attributeTypes_.push_back("uint");
        attributeTypes_.push_back("quaternion");
        attributeTypes_.push_back("assetreference");
        attributeTypes_.push_back("qvariant");
        attributeTypes_.push_back("qvariantlist");
        attributeTypes_.push_back("transform");
    }

    void ComponentManager::RegisterFactory(const QString &component, const ComponentFactoryPtr &factory)
    {
        if (factories_.find(component) == factories_.end())
            factories_[component] = factory;
    }

    void ComponentManager::UnregisterFactory(const QString &component)
    {
        ComponentFactoryMap::iterator iter = factories_.find(component);
        if (iter != factories_.end())
            factories_.erase(iter);
    }

    bool ComponentManager::CanCreate(const QString &type_name)
    {
        return (factories_.find(type_name) != factories_.end());
    }

    ComponentPtr ComponentManager::CreateComponent(const QString &type_name)
    {
        ComponentFactoryMap::const_iterator iter = factories_.find(type_name);
        if (iter == factories_.end())
            return ComponentPtr();

        ComponentPtr component = (*iter->second.get())();
        return component;
    }

    ComponentPtr ComponentManager::CreateComponent(const QString &type_name, const QString &name)
    {
        ComponentFactoryMap::const_iterator iter = factories_.find(type_name);
        if (iter == factories_.end())
            return ComponentPtr();

        ComponentPtr component = (*iter->second.get())();
        component->SetName(name);
        return component;
    }

    ComponentPtr ComponentManager::CloneComponent(const ComponentPtr &component)
    {
        ComponentFactoryMap::const_iterator iter = factories_.find(component->TypeName());
        if (iter == factories_.end())
            return ComponentPtr();

        ComponentPtr newComponent = (*iter->second.get())(component);
        return newComponent;
    }

    IAttribute *ComponentManager::CreateAttribute(IComponent*owner, const std::string &typeName, const std::string &name)
    {
        // The dynamically created attributes are deleted at the EC_DynamicComponent dtor.
        IAttribute *attribute = 0;
        if(typeName == "string")
            attribute = new Attribute<QString>(owner, name.c_str());
        else if(typeName == "int")
            attribute = new Attribute<int>(owner, name.c_str());
        else if(typeName == "real")
            attribute = new Attribute<float>(owner, name.c_str());
        else if(typeName == "color")
            attribute = new Attribute<Color>(owner, name.c_str());
        else if(typeName == "vector3df")
            attribute = new Attribute<Vector3df>(owner, name.c_str());
        else if(typeName == "bool")
            attribute = new Attribute<bool>(owner, name.c_str());
        else if(typeName == "uint")
            attribute = new Attribute<uint>(owner, name.c_str());
        else if(typeName == "quaternion")
            attribute = new Attribute<Quaternion>(owner, name.c_str());
        else if(typeName == "assetreference")
            attribute = new Attribute<AssetReference>(owner, name.c_str());
        else if(typeName == "qvariant")
            attribute = new Attribute<QVariant>(owner, name.c_str());
        else if(typeName == "qvariantlist")
            attribute = new Attribute<QVariantList >(owner, name.c_str());
        else if(typeName == "transform")
            attribute = new Attribute<Transform>(owner, name.c_str());
        return attribute;
    }

    StringVector ComponentManager::GetAttributeTypes() const
    {
        return attributeTypes_;
    }
}
