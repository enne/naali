/**
 *  For conditions of distribution and use, see copyright notice in license.txt
 *
 *  @file   ECEditorWindow.h
 *  @brief  Entity-component editor window.
 */

#ifndef incl_ECEditorModule_ECEditorWindow_h
#define incl_ECEditorModule_ECEditorWindow_h

#include "ForwardDefines.h"
#include "CoreTypes.h"
#include <QMap>
#include <QSet>

#include <QListWidgetItem>
#include <QPointer>
#include "Entity.h"

#include <QWidget>

extern std::vector<std::string> AttributeTypenames;

QT_BEGIN_NAMESPACE;
class QPushButton;
class QListWidget;
class QTreeWidget;
class QPoint;
QT_END_NAMESPACE;

struct EntityComponentSelection
{
    Scene::EntityPtr entity;
    std::vector<ComponentPtr> components;
};

namespace ECEditor
{
    class ECBrowser;
    class AddComponentDialog;

    //! Contains entity pointer as a QPointer. This class is used to indentify a right item using an entity ID.
    //! \ingroup ECEditorModuleClient.
    class EntityListWidgetItem: public QListWidgetItem
    {
    public:
        EntityListWidgetItem(const QString &name, QListWidget *list, Scene::Entity *entity):
            QListWidgetItem(name, list),
            entity_ptr_(entity)
        {
        }
        QPointer<Scene::Entity> GetEntity() const { return entity_ptr_; }
    private:
        //Weak pointer to entity switch will get released and setted to null when QObject's destructor is called.
        QPointer<Scene::Entity> entity_ptr_;
    };
    
    //! ECEditorWindow
    /*! /todo add description.
     *  \ingroup ECEditorModuleClient.
     */
    class ECEditorWindow : public QWidget
    {
        Q_OBJECT

    public:
        /// Constructor
        /// @param framework Framework.
        explicit ECEditorWindow(Foundation::Framework* framework);

        /// Destructor.
        ~ECEditorWindow();

        /// Adds new entity to the entity list.
        void AddEntity(entity_id_t entity_id);

        /// Removes entity from the entity list.
        void RemoveEntity(entity_id_t entity_id);

        /// Clears entity list.
        void ClearEntities();

    public slots:
        /// Deletes selected entity entries from the list (does not delete the entity itself).
        void DeleteEntitiesFromList();

        //! Remove coponent from entity and refresh property browser.
        void DeleteComponent(const QString &componentType, const QString &name);

        /// Opens a dialog that will handle new entity creation.
        /** After the dialog is done, ComponentDialogFinished method is called.
         */
        void CreateComponent();

        /// If entity selection different from previous update change browser to fit those changes.
        void RefreshPropertyBrowser();

        /// Shows context menu for entities.
        /// @param pos Mouse position of right-click event.
        void ShowEntityContextMenu(const QPoint &pos);

        /// Shows EC XML editor.for entity's all components.
        void ShowXmlEditorForEntity();

        /// Shows EC XML editor.for each components.
        void ShowXmlEditorForComponent(std::vector<ComponentPtr> components);

        /// Shows EC XML editor.for a single component.
        //void ShowXmlEditorForComponent();
        void ShowXmlEditorForComponent(const std::string &componentType);

        /// Show/Hide entity list.
        void ToggleEntityList();

        /// Checks if deleted entity is located in editor's list and if so remove it from the editor.
        void EntityRemoved(Scene::Entity* entity);

        /// Set editor focus boolean value. Only one ECEditorWindow can have focus and if focus
        /// is setted to true, this method will iterate all other ECEditorWindow intances and set their focus to false.
        void SetFocus(bool focus);

        /// Listens when editor's proxy widget gets the focus. If event type was focusInEvent
        /// then method will call SetFocus method.
        void FocusChanged(QFocusEvent *e);

    signals:
        /// Emitted user wants to edit entity's EC attributes in XML editor.
        void EditEntityXml(Scene::EntityPtr entity);

        /// Emitted user wants to edit EC attributes in XML editor.
        void EditComponentXml(ComponentPtr component);

        /// Emitted user wants to edit entity's EC attributes in XML editor.
        /// @param entities list of entities
        void EditEntityXml(const QList<Scene::EntityPtr> &entities);

        /// Emitted user wants to edit EC attributes in XML editor.
        /// @param list of components
        void EditComponentXml(const QList<ComponentPtr> & components);

    protected:
        /// QWidget override.
        void hideEvent(QHideEvent *hide_event);

        /// QWidget override.
        void changeEvent(QEvent *change_event);

    private slots:
        /// Listens SceneManager's ActionTriggered signal and if action was "MousePress"
        /// add entity to editor window (assuming that editor has a focus).
        void ActionTriggered(Scene::Entity *entity, const QString &action);

        /// Deletes entity.
        void DeleteEntity();

        /// Copy serializable component values to clipboard.
        void CopyEntity();

        /// Paste create a new entity and add serializable components.
        void PasteEntity();

        /// Highlights all entities from the entities_list that own a isntace of given component.
        void HighlightEntities(IComponent *component);

        /// Listenes when default world scene has changed and clear the editor window.
        /// @param scene new default world scene.
        void DefaultSceneChanged(const Scene::ScenePtr &scene);

        //When user have pressed ok or cancel button in component dialog this mehtod is called.
        void ComponentDialogFinished(int result);

    private:
        /// Find given entity from the QListWidget and if it's found, bold QListWidgetItem's font.
        void BoldEntityListItem(entity_id_t, bool bold = true);

        /// Initializes the widget.
        void Initialize();

        /// Returns list of selected entities.
        QList<Scene::EntityPtr> GetSelectedEntities() const;

        /// Framework pointer.
        Foundation::Framework *framework_;

        QPushButton* toggle_entities_button_;
        QListWidget* entity_list_;
        ECBrowser *browser_;
        AddComponentDialog *component_dialog_;
        bool has_focus_;
    };
}

#endif
