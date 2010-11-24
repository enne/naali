// For conditions of distribution and use, see copyright notice in license.txt

#include "StableHeaders.h"
#include "DebugOperatorNew.h"

#include "ECComponentEditor.h"
#include "ECEditorModule.h"

#include "IAttribute.h"
#include "ECAttributeEditor.h"
#include "IComponent.h"
#include "Transform.h"
#include "AssetReference.h"

#include <QtTreePropertyBrowser>
#include <QtGroupPropertyManager>
#include <QtProperty>

#include "MemoryLeakCheck.h"

namespace ECEditor
{
    // static
    //! @todo Replace this more practical implementation where new attribute type support would be more pratical.
    //! Like somesort of factory that is ownd by ComponentManager.
    ECAttributeEditorBase *ECComponentEditor::CreateAttributeEditor(
        QtAbstractPropertyBrowser *browser,
        ECComponentEditor *editor,
        ComponentPtr component,
        const QString &name,
        const QString &type)
        //IAttribute &attribute) 
    {
        ECAttributeEditorBase *attributeEditor = 0;
        if(type == "real")
            attributeEditor = new ECAttributeEditor<float>(browser, component, name, editor);
        else if(type == "int")
            attributeEditor = new ECAttributeEditor<int>(browser, component, name, editor);
        else if(type == "vector3df")
            attributeEditor = new ECAttributeEditor<Vector3df>(browser, component, name, editor);
        else if(type == "color")
            attributeEditor = new ECAttributeEditor<Color>(browser, component, name, editor);
        else if(type == "string")
            attributeEditor = new ECAttributeEditor<QString>(browser, component, name, editor);
        else if(type == "bool")
            attributeEditor = new ECAttributeEditor<bool>(browser, component, name, editor);
        else if(type == "qvariant")
            attributeEditor = new ECAttributeEditor<QVariant>(browser, component, name, editor);
        else if(type == "qvariantlist")
            attributeEditor = new ECAttributeEditor<QVariantList>(browser, component, name, editor);
        else if(type == "assetreference")
            attributeEditor = new ECAttributeEditor<AssetReference>(browser, component, name, editor);
        else if(type == "transform")
            attributeEditor = new ECAttributeEditor<Transform>(browser, component, name, editor);

        return attributeEditor;
    }

    ECComponentEditor::ECComponentEditor(ComponentPtr component, QtAbstractPropertyBrowser *propertyBrowser):
        QObject(propertyBrowser),
        groupProperty_(0),
        groupPropertyManager_(0),
        propertyBrowser_(propertyBrowser)
    {
        assert(component);
        typeName_ = component->TypeName();
        name_ = component->Name();

        assert(propertyBrowser_);
        if(!propertyBrowser_)
           return;

        groupPropertyManager_ = new QtGroupPropertyManager(this);
        if(groupPropertyManager_)
        {
            groupProperty_ = groupPropertyManager_->addProperty();
            AddNewComponent(component, true);
            CreateAttributeEditors(component);
        }

        propertyBrowser_->addProperty(groupProperty_);
    }
    
    ECComponentEditor::~ECComponentEditor()
    {
        propertyBrowser_->unsetFactoryForManager(groupPropertyManager_);
        SAFE_DELETE(groupProperty_)
        SAFE_DELETE(groupPropertyManager_)
        while(!attributeEditors_.empty())
        {
            SAFE_DELETE(attributeEditors_.begin()->second)
            attributeEditors_.erase(attributeEditors_.begin());
        }
    }

    void ECComponentEditor::CreateAttributeEditors(ComponentPtr component)
    {
        AttributeVector attributes = component->GetAttributes();
        for(uint i = 0; i < attributes.size(); i++)
        {
            ECAttributeEditorBase *attributeEditor = 0;
            attributeEditor = ECComponentEditor::CreateAttributeEditor(propertyBrowser_,
                                                                       this, 
                                                                       component, 
                                                                       QString::fromStdString(attributes[i]->GetNameString()),
                                                                       QString::fromStdString(attributes[i]->TypenameToString()));
            if(!attributeEditor)
                continue;
            attributeEditors_[attributes[i]->GetName()] = attributeEditor;
            attributeEditor->UpdateEditorUI();
            groupProperty_->setToolTip("Component type is " + component->TypeName());
            groupProperty_->addSubProperty(attributeEditor->GetProperty());
            connect(attributeEditor, SIGNAL(EditorChanged(const QString &)), this, SLOT(OnEditorChanged(const QString &)));
        }
    }

    void ECComponentEditor::UpdateGroupPropertyText()
    {
        if(!groupProperty_ || !components_.size())
            return;
        QString componentName = typeName_;
        componentName.replace("EC_", "");
        QString groupPropertyName = componentName;
        if(!name_.isEmpty())
            groupPropertyName += " (" + name_ + ") ";
        if(components_.size() > 1)
            groupPropertyName += QString(" (%1 components)").arg(components_.size());
        groupProperty_->setPropertyName(groupPropertyName);
    }

    bool ECComponentEditor::ContainProperty(QtProperty *property) const
    {
        AttributeEditorMap::const_iterator constIter = attributeEditors_.begin();
        while(constIter != attributeEditors_.end())
        {
            if(constIter->second->ContainProperty(property))
                return true;
            constIter++;
        }
        return false;
    }

    void ECComponentEditor::AddNewComponent(ComponentPtr component, bool updateUi)
    {
        //! Check that component type is same as editor's typename (We only want to add same type of components to editor).
        if(component->TypeName() != typeName_)
            return;

        components_.insert(component);
        //! insert new component for each attribute editor.
        AttributeEditorMap::iterator iter = attributeEditors_.begin();
        while(iter != attributeEditors_.end())
        {
            IAttribute *attribute = component->GetAttribute(iter->second->GetAttributeName());
            if(attribute)
                iter->second->AddComponent(component);
            iter++;
        }
        QObject::connect(component.get(), SIGNAL(OnAttributeChanged(IAttribute*, AttributeChange::Type)), this, SLOT(AttributeChanged(IAttribute*, AttributeChange::Type)));
        UpdateGroupPropertyText();
    }

    void ECComponentEditor::RemoveComponent(ComponentPtr component)
    {
        if(!component)
            return;

        if(component->TypeName() != typeName_)
            return;

        ComponentSet::iterator iter = components_.begin();
        while(iter != components_.end())
        {
            ComponentPtr componentPtr = (*iter).lock();
            if(componentPtr.get() == component.get())
            {
                AttributeEditorMap::iterator attributeIter = attributeEditors_.begin();
                while(attributeIter != attributeEditors_.end())
                {
                    IAttribute *attribute = componentPtr->GetAttribute(attributeIter->second->GetAttributeName());
                    if(attribute)
                        attributeIter->second->RemoveComponent(component);//RemoveAttribute(attribute);
                    attributeIter++;
                }
                disconnect(componentPtr.get(), SIGNAL(OnAttributeChanged(IAttribute*, AttributeChange::Type)), this, SLOT(AttributeChanged(IAttribute*, AttributeChange::Type)));
                components_.erase(iter);
                break;
            }
            iter++;
        }
        UpdateGroupPropertyText();
    }

    void ECComponentEditor::OnEditorChanged(const QString &name)
    {
        ECAttributeEditorBase *editor = qobject_cast<ECAttributeEditorBase*>(sender());
        if(!editor)
        {
            ECEditorModule::LogWarning("Fail to convert signal sender to ECAttributeEditorBase.");
            return;
        }
        groupProperty_->addSubProperty(editor->GetProperty());
    }

    void ECComponentEditor::AttributeChanged(IAttribute* attribute, AttributeChange::Type change)
    {
        IComponent *component = dynamic_cast<IComponent *>(sender());
        if((!component) || (!attribute))
            return;
        if(component->TypeName() != typeName_)
            return;

        AttributeEditorMap::iterator iter = attributeEditors_.find(attribute->GetName());
        if(iter != attributeEditors_.end())
            iter->second->UpdateEditorUI();
    }
}
