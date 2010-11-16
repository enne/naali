/**
 *  For conditions of distribution and use, see copyright notice in license.txt
 *
 *  @file   IComponent.cpp
 *  @brief  Base class for all components. Inherit from this class when creating new components.
 */

#include "StableHeaders.h"

#include "IComponent.h"

#include "Framework.h"
#include "Entity.h"
#include "SceneManager.h"
#include "EventManager.h"

#include <QDomDocument>

IComponent::IComponent(Foundation::Framework* framework) :
    parent_entity_(0),
    framework_(framework),
    network_sync_(true),
    updatemode_(AttributeChange::Replicate)
{
}

IComponent::IComponent(const IComponent &rhs) :
    framework_(rhs.framework_),
    parent_entity_(rhs.parent_entity_),
    network_sync_(rhs.network_sync_),
    updatemode_(rhs.updatemode_)
{
}

IComponent::~IComponent()
{
    // Removes itself from EventManager
    if (framework_)
        framework_->GetEventManager()->UnregisterEventSubscriber(this);
}

void IComponent::SetName(const QString& name)
{
    // no point to send a signal if name have stayed same as before.
    if(name_ == name)
        return;

    QString oldName = name_;
    name_ = name;
    emit OnComponentNameChanged(name, oldName);
}

void IComponent::SetUpdateMode(AttributeChange::Type defaultmode)
{
    // Note: we can't allow default mode to be Default, because that would be meaningless
    if (defaultmode != AttributeChange::Default)
        updatemode_ = defaultmode;
}

void IComponent::SetParentEntity(Scene::Entity* entity)
{
    parent_entity_ = entity;
    if (parent_entity_)
        emit ParentEntitySet();
    else
        emit ParentEntityDetached();
}

Scene::Entity* IComponent::GetParentEntity() const
{
    return parent_entity_;
}

void IComponent::SetNetworkSyncEnabled(bool enabled)
{
    network_sync_ = enabled;
}

IAttribute* IComponent::GetAttribute(QString name) const
{
    for(unsigned int i = 0; i < attributes_.size(); ++i)
        if(attributes_[i]->GetNameString() == name.toStdString())
            return attributes_[i];
    return 0;
}

QDomElement IComponent::BeginSerialization(QDomDocument& doc, QDomElement& base_element) const
{
    QDomElement comp_element = doc.createElement("component");
    comp_element.setAttribute("type", TypeName());
    if (!name_.isEmpty())
        comp_element.setAttribute("name", name_);
    
    if (!base_element.isNull())
        base_element.appendChild(comp_element);
    else
        doc.appendChild(comp_element);
    
    return comp_element;
}

void IComponent::WriteAttribute(QDomDocument& doc, QDomElement& comp_element, const QString& name, const QString& value) const
{
    QDomElement attribute_element = doc.createElement("attribute");
    attribute_element.setAttribute("name", name);
    attribute_element.setAttribute("value", value);
    comp_element.appendChild(attribute_element);
}

void IComponent::WriteAttribute(QDomDocument& doc, QDomElement& comp_element, const QString& name, const QString& value, const QString &type) const
{
    QDomElement attribute_element = doc.createElement("attribute");
    attribute_element.setAttribute("name", name);
    attribute_element.setAttribute("value", value);
    attribute_element.setAttribute("type", type);
    comp_element.appendChild(attribute_element);
}

bool IComponent::BeginDeserialization(QDomElement& comp_element)
{
    QString type = comp_element.attribute("type");
    if (type == TypeName())
    {
        SetName(comp_element.attribute("name"));
        return true;
    }
    return false;
}

QString IComponent::ReadAttribute(QDomElement& comp_element, const QString &name) const
{
    QDomElement attribute_element = comp_element.firstChildElement("attribute");
    while (!attribute_element.isNull())
    {
        if (attribute_element.attribute("name") == name)
            return attribute_element.attribute("value");
        
        attribute_element = attribute_element.nextSiblingElement("attribute");
    }
    
    return QString();
}

QString IComponent::ReadAttributeType(QDomElement& comp_element, const QString &name) const
{
    QDomElement attribute_element = comp_element.firstChildElement("attribute");
    while (!attribute_element.isNull())
    {
        if (attribute_element.attribute("name") == name)
            return attribute_element.attribute("type");
        
        attribute_element = attribute_element.nextSiblingElement("attribute");
    }
    
    return QString();
}

void IComponent::AttributeChanged(IAttribute* attribute, AttributeChange::Type change)
{
    if (change == AttributeChange::Default)
        change = updatemode_;
    if (change == AttributeChange::Disconnected)
        return; // No signals
    
    // Trigger scenemanager signal
    if (parent_entity_)
    {
        Scene::SceneManager* scene = parent_entity_->GetScene();
        if (scene)
            scene->EmitAttributeChanged(this, attribute, change);
    }
    
    // Trigger internal signal
    emit OnAttributeChanged(attribute, change);
}

void IComponent::AttributeChanged(const QString& attributeName, AttributeChange::Type change)
{
    if (change == AttributeChange::Disconnected)
        return; // No signals
    
    // Roll through attributes and check name match
    for (uint i = 0; i < attributes_.size(); ++i)
    {
        if (attributes_[i]->GetName() == attributeName)
        {
            AttributeChanged(attributes_[i], change);
            break;
        }
    }
}

void IComponent::SerializeTo(QDomDocument& doc, QDomElement& base_element) const
{
    if (!IsSerializable())
        return;

    QDomElement comp_element = BeginSerialization(doc, base_element);

    for (uint i = 0; i < attributes_.size(); ++i)
        WriteAttribute(doc, comp_element, attributes_[i]->GetNameString().c_str(), attributes_[i]->ToString().c_str());
}

void IComponent::DeserializeFrom(QDomElement& element, AttributeChange::Type change)
{
    if (!IsSerializable())
        return;

    if (!BeginDeserialization(element))
        return;

    for (uint i = 0; i < attributes_.size(); ++i)
    {
        std::string attr_str = ReadAttribute(element, attributes_[i]->GetNameString().c_str()).toStdString();
        attributes_[i]->FromString(attr_str, change);
    }
}

void IComponent::ComponentChanged(AttributeChange::Type change)
{
    for (uint i = 0; i < attributes_.size(); ++i)
        AttributeChanged(attributes_[i], change);
    
    //! \todo: this is deprecated and will be removed in the future. Do not rely on OnChanged() signal.
    OnChanged();
}
