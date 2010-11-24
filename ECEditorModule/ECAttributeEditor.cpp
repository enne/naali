// For conditions of distribution and use, see copyright notice in license.txt

#include "StableHeaders.h"
#include "DebugOperatorNew.h"

#include "ECEditorModule.h"
#include "ECAttributeEditor.h"
#include "IAttribute.h"
#include "MultiEditPropertyManager.h"
#include "MultiEditPropertyFactory.h"
#include "LineEditPropertyFactory.h"
#include "Transform.h"
#include "AssetReference.h"

// QtPropertyBrowser headers.
#include <qtvariantproperty.h>
#include <qtpropertymanager.h>
#include <qtpropertybrowser.h>
#include <qteditorfactory.h>

#include "LoggingFunctions.h"
DEFINE_POCO_LOGGING_FUNCTIONS("ECAttributeEditor")

#include "MemoryLeakCheck.h"

namespace ECEditor
{
    ECAttributeEditorBase::ECAttributeEditorBase(QtAbstractPropertyBrowser *owner,
                                                 ComponentPtr component,
                                                 const QString &name,
                                                 QObject *parent):
        QObject(parent),
        owner_(owner),
        name_(name),
        rootProperty_(0),
        factory_(0),
        propertyMgr_(0),
        listenEditorChangedSignal_(false),
        useMultiEditor_(false),
        editorState_(Uninitialized),
        metaDataFlag_(0)
    {
        if(FindAttribute(component))
            components_.push_back(ComponentWeakPtr(component));
    }

    ECAttributeEditorBase::~ECAttributeEditorBase()
    {
        UnInitialize();
    }

    bool ECAttributeEditorBase::ContainProperty(QtProperty *property) const
    {
        QSet<QtProperty *> properties = propertyMgr_->properties();
        QSet<QtProperty *>::const_iterator iter = properties.find(property);
        if(iter != properties.end())
            return true;
        return false;
    }

    void ECAttributeEditorBase::UpdateEditorUI()
    {
        if(components_.size() == 1)
        {
            if(!useMultiEditor_ && editorState_ != Uninitialized)
            {
                listenEditorChangedSignal_ = false;
                Update(); 
                listenEditorChangedSignal_ = true;
            }
            else
            {
                useMultiEditor_ = false;
                Initialize();
            }
        }
        else if(components_.size() > 1)
        {
            if(!IsIdentical())
            {
                if(!useMultiEditor_)
                {
                    useMultiEditor_ = true;
                    UnInitialize();
                }
            }
            else
            {
                if(useMultiEditor_)
                {
                    useMultiEditor_ = false;
                    UnInitialize();
                }
            }

            if(editorState_ == Uninitialized)
                Initialize();
            else
            {
                listenEditorChangedSignal_ = false;
                Update(); 
                listenEditorChangedSignal_ = true;
            }
        }
    }

    void ECAttributeEditorBase::AddComponent(ComponentPtr component)
    {
        // Before we add new component we make sure that it's not already added
        // and it contains right attribute.
        if(!HasComponent(component) && component->GetAttribute(name_))
            components_.push_back(ComponentWeakPtr(component));
        UpdateEditorUI();
    }

    void ECAttributeEditorBase::RemoveComponent(ComponentPtr component)
    {
        ComponentWeakPtrList::iterator iter = FindComponent(component);
        if(iter != components_.end())
            components_.erase(iter);
        if(!components_.size())
            deleteLater();
        UpdateEditorUI();
    }

    bool ECAttributeEditorBase::HasComponent(ComponentPtr component)
    {
        ComponentWeakPtrList::iterator iter = FindComponent(component);
        if(iter != components_.end())
            return true;
        return false;
    }

    bool ECAttributeEditorBase::IsIdentical() const
    {
        //No point to continue if there is only single component added.
        if(components_.size() <= 1)
            return true;

        for(uint i = 0; i < (components_.size() - 1); i++)
            for(uint j = 1; j < components_.size(); j++)
                if (!components_[i].expired() &&
                    !components_[j].expired() &&
                    components_[i].lock()->GetAttribute(name_)->ToString() != components_[j].lock()->GetAttribute(name_)->ToString())
                    return false;
        return true;
    }

    IAttribute *ECAttributeEditorBase::FindAttribute(ComponentPtr component)
    {
        if(component)
            return component->GetAttribute(name_);
        LogError("Component has expired.");
        return 0;
    }

    QList<ComponentWeakPtr>::iterator ECAttributeEditorBase::FindComponent(ComponentPtr component)
    {
        ComponentWeakPtrList::iterator iter = components_.begin();
        for(; iter != components_.end(); iter++)
            if(!(*iter).expired() && component.get() == (*iter).lock().get())
                return iter;
        return components_.end();
    }

    void ECAttributeEditorBase::PreInitialize()
    {
        if(propertyMgr_ || factory_ || rootProperty_)
            UnInitialize();
        editorState_ = WaitingForResponse;
    }

    void ECAttributeEditorBase::UnInitialize()
    {
        if(owner_)
        {
            owner_->unsetFactoryForManager(propertyMgr_);
            for(uint i = 0; i < optionalPropertyManagers_.size(); i++)
                owner_->unsetFactoryForManager(optionalPropertyManagers_[i]);
        }
        if(propertyMgr_)
        {
            propertyMgr_->deleteLater();
            propertyMgr_ = 0;
        }
        while(!optionalPropertyManagers_.empty())
        {
            optionalPropertyManagers_.back()->deleteLater();
            optionalPropertyManagers_.pop_back();
        }
        if(factory_)
        {
            factory_->deleteLater();
            factory_ = 0;
        }
        while(!optionalPropertyFactories_.empty())
        {
            optionalPropertyFactories_.back()->deleteLater();
            optionalPropertyFactories_.pop_back();
        }
        editorState_ = Uninitialized;
    }

    //-------------------------REAL ATTRIBUTE TYPE-------------------------

    template<> void ECAttributeEditor<float>::Initialize()
    {
        ECAttributeEditorBase::PreInitialize();
        if(!useMultiEditor_)
        {
            QtVariantPropertyManager *realPropertyManager = new QtVariantPropertyManager(this);
            QtVariantEditorFactory *variantFactory = new QtVariantEditorFactory(this);
            propertyMgr_ = realPropertyManager;
            factory_ = variantFactory;
            rootProperty_ = realPropertyManager->addProperty(QVariant::Double, name_);

            ComponentPtr comp = components_[0].lock();
            IAttribute *attribute = FindAttribute(comp);
            if(!attribute) 
            {
                //! @todo add log error.
                return;
            }

            AttributeMetadata *metaData = attribute->GetMetadata();
            if(metaData)
            {
                if(!metaData->min.isEmpty())
                    metaDataFlag_ |= UsingMinValue;
                if(!metaData->max.isEmpty())
                    metaDataFlag_ |= UsingMaxValue;
                if(!metaData->step.isEmpty())
                    metaDataFlag_ |= UsingStepValue;
                if((metaDataFlag_ & UsingMinValue) != 0)
                    realPropertyManager->setAttribute(rootProperty_, "minimum", ::ParseString<float>(metaData->min.toStdString()));
                if((metaDataFlag_ & UsingMaxValue) != 0)
                    realPropertyManager->setAttribute(rootProperty_, "maximum", ::ParseString<float>(metaData->max.toStdString()));
                if((metaDataFlag_ & UsingStepValue) != 0)
                    realPropertyManager->setAttribute(rootProperty_, "singleStep", ::ParseString<float>(metaData->step.toStdString()));
            }

            if(rootProperty_)
            {
                Update();
                QObject::connect(propertyMgr_, SIGNAL(propertyChanged(QtProperty*)), this, SLOT(PropertyChanged(QtProperty*)));
            }
            owner_->setFactoryForManager(realPropertyManager, variantFactory);
        }
        else
        {
            InitializeMultiEditor();
        }
        emit EditorChanged(name_);
    }


    template<> void ECAttributeEditor<float>::Update()
    {
        if(!useMultiEditor_)
        {
            //AttributeList::iterator iter = attributes_.begin();
            QtVariantPropertyManager *realPropertyManager = dynamic_cast<QtVariantPropertyManager *>(propertyMgr_);
            assert(realPropertyManager);
            if(!realPropertyManager)
                return;

            if(rootProperty_)
            {
                Attribute<float> *attribute = dynamic_cast<Attribute<float>*>(FindAttribute(components_[0].lock()));
                realPropertyManager->setValue(rootProperty_, attribute->Get());
            }
        }
        else
            UpdateMultiEditorValue();
    }

    template<> void ECAttributeEditor<float>::Set(QtProperty *property)
    {
        if(listenEditorChangedSignal_)
        {
            float newValue = ParseString<float>(property->valueText().toStdString());
            SetValue(newValue);
        }
    }

    //-------------------------INT ATTRIBUTE TYPE-------------------------

    template<> void ECAttributeEditor<int>::Update()
    {
        if(!useMultiEditor_)
        {
            QtVariantPropertyManager *intPropertyManager = dynamic_cast<QtVariantPropertyManager *>(propertyMgr_);
            assert(intPropertyManager);
            if(!intPropertyManager)
                return;

            if(rootProperty_)
            {
                Attribute<int> *attribute = dynamic_cast<Attribute<int>*>(FindAttribute(components_[0].lock()));
                intPropertyManager->setValue(rootProperty_, attribute->Get());
            }
        }
        else
            UpdateMultiEditorValue();
    }

    template<> void ECAttributeEditor<int>::Initialize()
    {
        ECAttributeEditorBase::PreInitialize();
        if(!useMultiEditor_)
        {
            ComponentPtr comp = components_[0].lock();
            IAttribute *attribute = FindAttribute(comp);
            if(!attribute)
            {
                //! @todo add log error 
                return;
            }

            //Check if int need to have min and max value set and also enum types are presented as a int value.
            AttributeMetadata *metaData = attribute->GetMetadata();
            if(metaData)
            {
                if(!metaData->enums.empty())
                    metaDataFlag_ |= UsingEnums;
                else
                {
                    if(!metaData->min.isEmpty())
                        metaDataFlag_ |= UsingMinValue;
                    if(!metaData->max.isEmpty())
                        metaDataFlag_ |= UsingMaxValue;
                    if(!metaData->step.isEmpty())
                        metaDataFlag_ |= UsingStepValue;
                }
                if(!metaData->description.isEmpty())
                    metaDataFlag_ |= UsingDescription;
            }

            QtVariantPropertyManager *intPropertyManager = new QtVariantPropertyManager(this);
            QtVariantEditorFactory *variantFactory = new QtVariantEditorFactory(this);
            // Check if attribute want to use enums.
            if((metaDataFlag_ & UsingEnums) != 0)
            {
                QtVariantProperty *prop = 0;
                prop = intPropertyManager->addProperty(QtVariantPropertyManager::enumTypeId(), name_);
                rootProperty_ = prop;
                QStringList enumNames;
                AttributeMetadata::EnumDescMap_t::iterator iter = metaData->enums.begin();
                for(; iter != metaData->enums.end(); iter++)
                    enumNames << QString::fromStdString(iter->second);

                prop->setAttribute(QString("enumNames"), enumNames);
            }
            else
            {
                rootProperty_ = intPropertyManager->addProperty(QVariant::Int, name_);
                if((metaDataFlag_ & UsingMinValue) != 0)
                    intPropertyManager->setAttribute(rootProperty_, "minimum", ::ParseString<int>(metaData->min.toStdString()));
                if((metaDataFlag_ & UsingMaxValue) != 0)
                    intPropertyManager->setAttribute(rootProperty_, "maximum", ::ParseString<int>(metaData->max.toStdString()));
                if((metaDataFlag_ & UsingStepValue) != 0)
                    intPropertyManager->setAttribute(rootProperty_, "singleStep", ::ParseString<int>(metaData->step.toStdString()));
            }
            propertyMgr_ = intPropertyManager;
            factory_ = variantFactory;
            if(rootProperty_)
            {
                Update();
                QObject::connect(propertyMgr_, SIGNAL(propertyChanged(QtProperty*)), this, SLOT(PropertyChanged(QtProperty*)));
            }
            owner_->setFactoryForManager(intPropertyManager, variantFactory);
        }
        else
        {
            InitializeMultiEditor();
        }
        emit EditorChanged(name_);
    }

    template<> void ECAttributeEditor<int>::Set(QtProperty *property)
    {
        if(listenEditorChangedSignal_)
        {
            int newValue = 0;
            std::string valueString = property->valueText().toStdString();
            if((metaDataFlag_ & UsingEnums) != 0)
            {
                ComponentPtr comp = components_[0].lock();
                IAttribute *attribute = FindAttribute(comp);
                if(!attribute)
                {
                    //! @todo add log error
                    return;
                }

                AttributeMetadata *metaData = attribute->GetMetadata();
                AttributeMetadata::EnumDescMap_t::iterator iter = metaData->enums.begin();
                for(; iter != metaData->enums.end(); iter++)
                    if(valueString == iter->second)
                        newValue = iter->first;
            }
            else
                newValue = ParseString<int>(valueString);
            SetValue(newValue);
        }
    }

    //-------------------------BOOL ATTRIBUTE TYPE-------------------------

    template<> void ECAttributeEditor<bool>::Initialize()
    {
        ECAttributeEditorBase::PreInitialize();
        if(!useMultiEditor_)
        {
            QtVariantPropertyManager *boolPropertyManager = new QtVariantPropertyManager(this);
            QtVariantEditorFactory *variantFactory = new QtVariantEditorFactory(this);
            propertyMgr_ = boolPropertyManager;
            factory_ = variantFactory;
            rootProperty_ = boolPropertyManager->addProperty(QVariant::Bool, name_);
            if(rootProperty_)
            {
                Update();
                QObject::connect(propertyMgr_, SIGNAL(propertyChanged(QtProperty*)), this, SLOT(PropertyChanged(QtProperty*)));
            }
            owner_->setFactoryForManager(boolPropertyManager, variantFactory);
        }
        else
        {
            InitializeMultiEditor();
        }
        emit EditorChanged(name_);
    }

    template<> void ECAttributeEditor<bool>::Set(QtProperty *property)
    {
        if(listenEditorChangedSignal_)
        {
            if(property->valueText().toStdString() == "True")
                SetValue(true);
            else
                SetValue(false);
        }
    }

    template<> void ECAttributeEditor<bool>::Update()
    {
        if(!useMultiEditor_)
        {
            QtVariantPropertyManager *boolPropertyManager = dynamic_cast<QtVariantPropertyManager *>(propertyMgr_);
            if(!boolPropertyManager)
                return;

            if(rootProperty_)
            {
                ComponentPtr comp = components_[0].lock();
                Attribute<bool> *attribute = dynamic_cast<Attribute<bool>*>(FindAttribute(comp));
                boolPropertyManager->setValue(rootProperty_, attribute->Get());
            }
        }
        else
            UpdateMultiEditorValue();
    }

    //-------------------------VECTOR3DF ATTRIBUTE TYPE-------------------------

    template<> void ECAttributeEditor<Vector3df>::Update()
    {
        if(!useMultiEditor_)
        {
            QtVariantPropertyManager *variantManager = dynamic_cast<QtVariantPropertyManager *>(propertyMgr_);
            if(rootProperty_)
            {
                QList<QtProperty *> children = rootProperty_->subProperties();
                if(children.size() >= 3)
                {
                    ComponentPtr comp = components_[0].lock();
                    Attribute<Vector3df> *attribute = dynamic_cast<Attribute<Vector3df> *>(FindAttribute(comp));
                    if(!attribute)
                        return;

                    Vector3df vectorValue = attribute->Get();
                    variantManager->setValue(children[0], vectorValue.x);
                    variantManager->setValue(children[1], vectorValue.y);
                    variantManager->setValue(children[2], vectorValue.z);
                }
            }
        }
        else
            UpdateMultiEditorValue();
    }

    template<> void ECAttributeEditor<Vector3df>::Initialize()
    {
        ECAttributeEditorBase::PreInitialize();
        if(!useMultiEditor_)
        {
            QtVariantPropertyManager *variantManager = new QtVariantPropertyManager(this);
            QtVariantEditorFactory *variantFactory = new QtVariantEditorFactory(this);
            propertyMgr_ = variantManager;
            factory_ = variantFactory;
            rootProperty_ = variantManager->addProperty(QtVariantPropertyManager::groupTypeId(), name_);

            if(rootProperty_)
            {
                QtProperty *childProperty = 0;
                childProperty = variantManager->addProperty(QVariant::Double, "x");
                rootProperty_->addSubProperty(childProperty);

                childProperty = variantManager->addProperty(QVariant::Double, "y");
                rootProperty_->addSubProperty(childProperty);

                childProperty = variantManager->addProperty(QVariant::Double, "z");
                rootProperty_->addSubProperty(childProperty);
                Update();
                QObject::connect(propertyMgr_, SIGNAL(propertyChanged(QtProperty*)), this, SLOT(PropertyChanged(QtProperty*)));
            }
            owner_->setFactoryForManager(variantManager, variantFactory);
        }
        else
        {
            InitializeMultiEditor();
        }
        emit EditorChanged(name_);
    }

    template<> void ECAttributeEditor<Vector3df>::Set(QtProperty *property)
    {
        if(listenEditorChangedSignal_)
        {
            QList<QtProperty *> children = rootProperty_->subProperties();
            if(children.size() >= 3)
            {
                ComponentPtr comp = components_[0].lock();
                Attribute<Vector3df> *attribute = dynamic_cast<Attribute<Vector3df> *>(FindAttribute(comp));
                if(!attribute)
                    return;

                Vector3df newValue = attribute->Get();
                QString propertyName = property->propertyName();
                if(propertyName == "x")
                    newValue.x = ParseString<float>(property->valueText().toStdString());
                else if(propertyName == "y")
                    newValue.y = ParseString<float>(property->valueText().toStdString());
                else if(propertyName == "z")
                    newValue.z = ParseString<float>(property->valueText().toStdString());
                SetValue(newValue);
            }
        }
    }

    //-------------------------COLOR ATTRIBUTE TYPE-------------------------

    template<> void ECAttributeEditor<Color>::Update()
    {
        if(!useMultiEditor_)
        {
            QtVariantPropertyManager *variantManager = dynamic_cast<QtVariantPropertyManager *>(propertyMgr_);
            if(rootProperty_)
            {
                QList<QtProperty *> children = rootProperty_->subProperties();
                if(children.size() >= 4)
                {
                    ComponentPtr comp = components_[0].lock();
                    Attribute<Color> *attribute = dynamic_cast<Attribute<Color> *>(FindAttribute(comp));
                    if(!attribute)
                        return;

                    Color colorValue = attribute->Get();
                    variantManager->setValue(children[0], colorValue.r * 255);
                    variantManager->setValue(children[1], colorValue.g * 255);
                    variantManager->setValue(children[2], colorValue.b * 255);
                    variantManager->setValue(children[3], colorValue.a * 255);
                }
            }
        }
        else
            UpdateMultiEditorValue();
    }

    template<> void ECAttributeEditor<Color>::Initialize()
    {
        ECAttributeEditorBase::PreInitialize();
        if(!useMultiEditor_)
        {
            QtVariantPropertyManager *variantManager = new QtVariantPropertyManager(this);
            QtVariantEditorFactory *variantFactory = new QtVariantEditorFactory(this);
            propertyMgr_ = variantManager;
            factory_ = variantFactory;

            rootProperty_ = variantManager->addProperty(QtVariantPropertyManager::groupTypeId(), name_);
            if(rootProperty_)
            {
                QtVariantProperty *childProperty = 0;
                childProperty = variantManager->addProperty(QVariant::Int, "Red");
                rootProperty_->addSubProperty(childProperty);
                variantManager->setAttribute(childProperty, "minimum", QVariant(0));
                variantManager->setAttribute(childProperty, "maximum", QVariant(255));

                childProperty = variantManager->addProperty(QVariant::Int, "Green");
                rootProperty_->addSubProperty(childProperty);
                variantManager->setAttribute(childProperty, "minimum", QVariant(0));
                variantManager->setAttribute(childProperty, "maximum", QVariant(255));

                childProperty = variantManager->addProperty(QVariant::Int, "Blue");
                rootProperty_->addSubProperty(childProperty);
                variantManager->setAttribute(childProperty, "minimum", QVariant(0));
                variantManager->setAttribute(childProperty, "maximum", QVariant(255));

                childProperty = variantManager->addProperty(QVariant::Int, "Alpha");
                rootProperty_->addSubProperty(childProperty);
                variantManager->setAttribute(childProperty, "minimum", QVariant(0));
                variantManager->setAttribute(childProperty, "maximum", QVariant(255));

                Update();
                QObject::connect(propertyMgr_, SIGNAL(propertyChanged(QtProperty*)), this, SLOT(PropertyChanged(QtProperty*)));
            }
            owner_->setFactoryForManager(variantManager, variantFactory);
        }
        else
        {
            InitializeMultiEditor();
        }
        emit EditorChanged(name_);
    }

    template<> void ECAttributeEditor<Color>::Set(QtProperty *property)
    {
        if(listenEditorChangedSignal_)
        {
           QList<QtProperty *> children = rootProperty_->subProperties();
            if(children.size() >= 4)
            {
                ComponentPtr comp = components_[0].lock();
                Attribute<Color> *attribute = dynamic_cast<Attribute<Color> *>(FindAttribute(comp));
                if(!attribute)
                    return;

                Color newValue = attribute->Get();
                QString propertyName = property->propertyName();
                if(propertyName == "Red")
                    newValue.r = ParseString<int>(property->valueText().toStdString()) / 255.0f;
                else if(propertyName == "Green")
                    newValue.g = ParseString<int>(property->valueText().toStdString()) / 255.0f;
                else if(propertyName == "Blue")
                    newValue.b = ParseString<int>(property->valueText().toStdString()) / 255.0f;
                else if(propertyName == "Alpha")
                    newValue.a = ParseString<int>(property->valueText().toStdString()) / 255.0f;
                SetValue(newValue);
            }
        }
    }

    //-------------------------QSTRING ATTRIBUTE TYPE-------------------------

    template<> void ECAttributeEditor<QString>::Initialize()
    {
        ECAttributeEditorBase::PreInitialize();
        if(!useMultiEditor_)
        {
            QtStringPropertyManager *qStringPropertyManager = new QtStringPropertyManager(this);
            ECEditor::LineEditPropertyFactory *lineEditFactory = new ECEditor::LineEditPropertyFactory(this);
            propertyMgr_ = qStringPropertyManager;
            factory_ = lineEditFactory;
            rootProperty_ = qStringPropertyManager->addProperty(name_);
            if(rootProperty_)
            {
                Update();
                QObject::connect(propertyMgr_, SIGNAL(propertyChanged(QtProperty*)), this, SLOT(PropertyChanged(QtProperty*)));
            }
            owner_->setFactoryForManager(qStringPropertyManager, lineEditFactory);
        }
        else
        {
            InitializeMultiEditor();
        }
        emit EditorChanged(name_);
    }

    template<> void ECAttributeEditor<QString>::Set(QtProperty *property)
    {
        if (listenEditorChangedSignal_)
            SetValue(property->valueText());
    }

    template<> void ECAttributeEditor<QString>::Update()
    {
        if(!useMultiEditor_)
        {
            ComponentPtr comp = components_[0].lock();
            QtStringPropertyManager *qStringPropertyManager = dynamic_cast<QtStringPropertyManager *>(propertyMgr_);

            assert(qStringPropertyManager);
            if(!qStringPropertyManager)
                return;

            if (rootProperty_)
            {
                Attribute<QString> *attribute = dynamic_cast<Attribute<QString>*>(FindAttribute(comp));
                if(!attribute)
                {
                    //! @todo add log error.
                    return;
                }
                qStringPropertyManager->setValue(rootProperty_, attribute->Get());
            }
        }
        else
            UpdateMultiEditorValue();
    }

    //-------------------------QVARIANT ATTRIBUTE TYPE-------------------------

    template<> void ECAttributeEditor<QVariant>::Initialize()
    {
        ECAttributeEditorBase::PreInitialize();
        if(!useMultiEditor_)
        {
            QtStringPropertyManager *qStringPropertyManager = new QtStringPropertyManager(this);
            ECEditor::LineEditPropertyFactory *lineEditFactory = new ECEditor::LineEditPropertyFactory(this);
            propertyMgr_ = qStringPropertyManager;
            factory_ = lineEditFactory;
            rootProperty_ = qStringPropertyManager->addProperty(name_);
            if(rootProperty_)
            {
                Update();
                QObject::connect(propertyMgr_, SIGNAL(propertyChanged(QtProperty*)), this, SLOT(PropertyChanged(QtProperty*)));
            }
            owner_->setFactoryForManager(qStringPropertyManager, lineEditFactory);
        }
        else
        {
            InitializeMultiEditor();
        }
        emit EditorChanged(name_);
    }

    template<> void ECAttributeEditor<QVariant>::Set(QtProperty *property)
    {
        if (listenEditorChangedSignal_)
        {
            QVariant value(property->valueText());
            SetValue(value);
        }
    }

    template<> void ECAttributeEditor<QVariant>::Update()
    {
        if(!useMultiEditor_)
        {
            QtStringPropertyManager *qStringPropertyManager = dynamic_cast<QtStringPropertyManager *>(propertyMgr_);
            assert(qStringPropertyManager);
            if(!qStringPropertyManager)
                return;

            if (rootProperty_)
            {
                ComponentPtr comp = components_[0].lock();
                Attribute<QVariant> *attribute = dynamic_cast<Attribute<QVariant>*>(FindAttribute(comp));
                qStringPropertyManager->setValue(rootProperty_, attribute->Get().toString());
            }
        }
        else
            UpdateMultiEditorValue();
    }

    //-------------------------QVARIANTLIST ATTRIBUTE TYPE---------------------------

    template<> void ECAttributeEditor<QVariantList >::Initialize()
    {
        ECAttributeEditorBase::PreInitialize();
        if(!useMultiEditor_)
        {
            QtGroupPropertyManager *groupManager = new QtGroupPropertyManager(this);
            QtStringPropertyManager *stringManager = new QtStringPropertyManager(this);
            LineEditPropertyFactory *lineEditFactory = new LineEditPropertyFactory(this);
            propertyMgr_ = groupManager;
            factory_ = lineEditFactory;
            optionalPropertyManagers_.push_back(stringManager);

            rootProperty_ = groupManager->addProperty();
            rootProperty_->setPropertyName(name_);
            if(rootProperty_)
            {
                QtProperty *childProperty = 0;
                // Get number of elements in attribute array and create for property for each array element.
                ComponentPtr comp = components_[0].lock();
                Attribute<QVariantList > *attribute = dynamic_cast<Attribute<QVariantList >*>(FindAttribute(comp));
                if(!attribute)
                {
                    //! @todo Add log error.
                    return;
                }
                QVariantList variantArray = attribute->Get();
                for(uint i = 0; i < variantArray.size(); i++)
                {
                    childProperty = stringManager->addProperty(QString::fromStdString("[" + ::ToString<uint>(i) + "]"));
                    rootProperty_->addSubProperty(childProperty);
                }
                childProperty = stringManager->addProperty(QString::fromStdString("[" + ::ToString<uint>(variantArray.size()) + "]"));
                rootProperty_->addSubProperty(childProperty);

                Update();
                QObject::connect(stringManager, SIGNAL(propertyChanged(QtProperty*)), this, SLOT(PropertyChanged(QtProperty*)));
            }
            owner_->setFactoryForManager(stringManager, lineEditFactory);
        }
        else
            InitializeMultiEditor();
        emit EditorChanged(name_);
    }

    template<> void ECAttributeEditor<QVariantList >::Set(QtProperty *property)
    {
        if (listenEditorChangedSignal_)
        {
            ComponentPtr comp = components_[0].lock();
            Attribute<QVariantList > *attribute = dynamic_cast<Attribute<QVariantList >*>(FindAttribute(comp));
            if (!attribute)
            {
                //! @todo add log error
                return;
            }
            QtStringPropertyManager *stringManager = dynamic_cast<QtStringPropertyManager *>(optionalPropertyManagers_[0]);
            QList<QtProperty*> children = rootProperty_->subProperties();
            QVariantList value;
            for(int i = 0; i < children.size(); i++)
            {
                QVariant variant = QVariant(stringManager->value(children[i]));
                if(variant.toString() == "" && i == children.size() - 1)
                    continue;
                value.push_back(variant.toString());
            }
            //We wont allow double empty array elements.
            if(value.size() >= 1)
                if(value[value.size() - 1] == "")
                    value.pop_back();
            SetValue(value);
        }
    }

    template<> void ECAttributeEditor<QVariantList >::Update()
    {
        if(!useMultiEditor_)
        {
            ComponentPtr comp = components_[0].lock();
            Attribute<QVariantList > *attribute = dynamic_cast<Attribute<QVariantList >*>(FindAttribute(comp));
            if (!attribute)
            {
                //! @todo add log error.
                return;
            }
            QtStringPropertyManager *stringManager = dynamic_cast<QtStringPropertyManager *>(optionalPropertyManagers_[0]);
            QList<QtProperty*> children = rootProperty_->subProperties();
            QVariantList value = attribute->Get();
            //! @todo It's tend to be heavy operation to reinitialize all ui elements when new parameters have been added.
            //! replace this so that only single vector's element is added/deleted from the editor.
            if(value.size() + 1 != children.size())
            {
                UnInitialize();
                Initialize();
            }
            if(value.size() <= children.size())
            {
                for(uint i = 0; i < value.size(); i++)
                {
                    stringManager->setValue(children[i], value[i].toString());
                }
            }
        }
        else
            UpdateMultiEditorValue();
    }

    //-------------------------ASSETREFERENCE ATTRIBUTE TYPE-------------------------

    template<> void ECAttributeEditor<AssetReference>::Update()
    {
        if(!useMultiEditor_)
        {
            QList<QtProperty*> children = rootProperty_->subProperties();
            if(children.size() == 2)
            {
                ComponentPtr comp = components_[0].lock();
                QtStringPropertyManager *stringManager = dynamic_cast<QtStringPropertyManager *>(children[0]->propertyManager());
                Attribute<AssetReference> *attribute = dynamic_cast<Attribute<AssetReference> *>(FindAttribute(comp));
                if(!attribute || !stringManager)
                {
                    //! @todo add log error.
                    return;
                }

                stringManager->setValue(children[0], attribute->Get().id);
                stringManager->setValue(children[1], attribute->Get().type);
            }
        }
        else
            UpdateMultiEditorValue();
    }

    template<> void ECAttributeEditor<AssetReference>::Initialize()
    {
        ECAttributeEditorBase::PreInitialize();
        if(!useMultiEditor_)
        {
            QtGroupPropertyManager *groupManager = new QtGroupPropertyManager(this);
            QtStringPropertyManager *stringManager = new QtStringPropertyManager(this);
            LineEditPropertyFactory *lineEditFactory = new LineEditPropertyFactory(this);
            propertyMgr_ = groupManager;
            factory_ = lineEditFactory;
            optionalPropertyManagers_.push_back(stringManager);

            rootProperty_ = groupManager->addProperty();
            rootProperty_->setPropertyName(name_);
            if(rootProperty_)
            {
                QtProperty *childProperty = 0;
                childProperty = stringManager->addProperty("Asset ID");
                rootProperty_->addSubProperty(childProperty);

                childProperty = stringManager->addProperty("Asset type");
                rootProperty_->addSubProperty(childProperty);

                Update();
                QObject::connect(stringManager, SIGNAL(propertyChanged(QtProperty*)), this, SLOT(PropertyChanged(QtProperty*)));
            }
            owner_->setFactoryForManager(stringManager, lineEditFactory);
        }
        else
            InitializeMultiEditor();
        emit EditorChanged(name_);
    }

    template<> void ECAttributeEditor<AssetReference>::Set(QtProperty *property)
    {
        if(listenEditorChangedSignal_)
        {
            QList<QtProperty*> children = rootProperty_->subProperties();
            if(children.size() == 2)
            {
                ComponentPtr comp = components_[0].lock();
                QtStringPropertyManager *strMgr = dynamic_cast<QtStringPropertyManager *>(children[0]->propertyManager());
                Attribute<AssetReference> *attribute = dynamic_cast<Attribute<AssetReference> *>(FindAttribute(comp));
                if(!attribute || !strMgr)
                {
                    //! @todo add log error.
                    return;
                }

                SetValue(AssetReference(strMgr->value(children[0]), strMgr->value(children[1])));
            }
        }
    }

    //---------------------------TRANSFORM----------------------------
    template<> void ECAttributeEditor<Transform>::Update()
    {
        if(!useMultiEditor_)
        {
            QtVariantPropertyManager *variantManager = dynamic_cast<QtVariantPropertyManager *>(propertyMgr_);
            if(rootProperty_)
            {
                QList<QtProperty *> children = rootProperty_->subProperties();
                if(children.size() >= 3)
                {
                    ComponentPtr comp = components_[0].lock();
                    Attribute<Transform> *attribute = dynamic_cast<Attribute<Transform> *>(FindAttribute(comp));
                    if(!attribute)
                    {
                        //! @todo add log error.
                        return;
                    }

                    Transform transformValue = attribute->Get();
                    QList<QtProperty *> positions = children[0]->subProperties();
                    variantManager->setValue(positions[0], transformValue.position.x);
                    variantManager->setValue(positions[1], transformValue.position.y);
                    variantManager->setValue(positions[2], transformValue.position.z);

                    QList<QtProperty *> rotations = children[1]->subProperties();
                    variantManager->setValue(rotations[0], transformValue.rotation.x);
                    variantManager->setValue(rotations[1], transformValue.rotation.y);
                    variantManager->setValue(rotations[2], transformValue.rotation.z);

                    QList<QtProperty *> scales    = children[2]->subProperties();
                    variantManager->setValue(scales[0], transformValue.scale.x);
                    variantManager->setValue(scales[1], transformValue.scale.y);
                    variantManager->setValue(scales[2], transformValue.scale.z);
                }
            }
        }
        else
            UpdateMultiEditorValue();
    }

    template<> void ECAttributeEditor<Transform>::Initialize()
    {
        ECAttributeEditorBase::PreInitialize();
        if(!useMultiEditor_)
        {
            QtVariantPropertyManager *variantManager = new QtVariantPropertyManager(this);
            QtVariantEditorFactory *variantFactory = new QtVariantEditorFactory(this);
            propertyMgr_ = variantManager;
            factory_ = variantFactory;

            rootProperty_ = variantManager->addProperty(QtVariantPropertyManager::groupTypeId(), name_);
            if(rootProperty_)
            {
                QtVariantProperty *childProperty = 0;
                QtVariantProperty *positionProperty = variantManager->addProperty(QtVariantPropertyManager::groupTypeId(), "Position");
                rootProperty_->addSubProperty(positionProperty);
                childProperty = variantManager->addProperty(QVariant::Double, "x");
                positionProperty->addSubProperty(childProperty);

                childProperty = variantManager->addProperty(QVariant::Double, "y");
                positionProperty->addSubProperty(childProperty);

                childProperty = variantManager->addProperty(QVariant::Double, "z");
                positionProperty->addSubProperty(childProperty);

                QtVariantProperty *rotationProperty = variantManager->addProperty(QtVariantPropertyManager::groupTypeId(), "Rotation");
                rootProperty_->addSubProperty(rotationProperty);
                childProperty = variantManager->addProperty(QVariant::Double, "x");
                rotationProperty->addSubProperty(childProperty);

                childProperty = variantManager->addProperty(QVariant::Double, "y");
                rotationProperty->addSubProperty(childProperty);

                childProperty = variantManager->addProperty(QVariant::Double, "z");
                rotationProperty->addSubProperty(childProperty);

                QtVariantProperty *scaleProperty = variantManager->addProperty(QtVariantPropertyManager::groupTypeId(), "Scale");
                rootProperty_->addSubProperty(scaleProperty);
                childProperty = variantManager->addProperty(QVariant::Double, "x");
                scaleProperty->addSubProperty(childProperty);

                childProperty = variantManager->addProperty(QVariant::Double, "y");
                scaleProperty->addSubProperty(childProperty);

                childProperty = variantManager->addProperty(QVariant::Double, "z");
                scaleProperty->addSubProperty(childProperty);

                Update();
                QObject::connect(propertyMgr_, SIGNAL(propertyChanged(QtProperty*)), this, SLOT(PropertyChanged(QtProperty*)));
            }
            owner_->setFactoryForManager(variantManager, variantFactory);
        }
        else
        {
            InitializeMultiEditor();
        }
        emit EditorChanged(name_);
    }

    template<> void ECAttributeEditor<Transform>::Set(QtProperty *property)
    {
        if(listenEditorChangedSignal_)
        {
            QList<QtProperty *> children = rootProperty_->subProperties();
            if(children.size() >= 3)
            {
                ComponentPtr comp = components_[0].lock();
                Attribute<Transform> *attribute = dynamic_cast<Attribute<Transform> *>(FindAttribute(comp));
                if(!attribute)
                {
                    //! @todo add log error.
                    return;
                }
                Transform trans = attribute->Get();

                int foundIndex = -1;
                for(uint i = 0; i < children.size(); i++)
                {
                    QList<QtProperty *> properties = children[i]->subProperties();
                    for(uint j = 0; j < properties.size(); j++)
                    {
                        if(properties[j] == property)
                        {
                            foundIndex = (i * 3) + j;
                            break;
                        }
                    }
                }
                if(foundIndex != -1)
                {
                    bool success = false;
                    float value = property->valueText().toFloat(&success);
                    if(!success)
                    {
                        ECEditorModule::LogError("Failed to convert the property value in float format.");
                        return;
                    } 

                    switch(foundIndex)
                    {
                    case 0:
                        trans.position.x = value;
                        break;
                    case 1:
                        trans.position.y = value;
                        break;
                    case 2:
                        trans.position.z = value;
                        break;
                    case 3:
                        trans.rotation.x = value;
                        break;
                    case 4:
                        trans.rotation.y = value;
                        break;
                    case 5:
                        trans.rotation.z = value;
                        break;
                    case 6:
                        trans.scale.x = value;
                        break;
                    case 7:
                        trans.scale.y = value;
                        break;
                    case 8:
                        trans.scale.z = value;
                        break;
                    }
                    SetValue(trans);
                }
            }
        }
    }
}