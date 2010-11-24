// For conditions of distribution and use, see copyright notice in license.txt

#ifndef incl_ECEditorModule_ECBrowser_h
#define incl_ECEditorModule_ECBrowser_h

#define QT_QTPROPERTYBROWSER_IMPORT

#include "ForwardDefines.h"
#include "CoreTypes.h"
#include "ComponentGroup.h"
#include "AttributeChangeType.h"

#include <QtTreePropertyBrowser>
#include <map>
#include <set>

class QtTreePropertyBrowser;
class QtGroupPropertyManager;
class QtBrowserItem;
class QTreeWidget;
class QMenu;
class QMimeData;

class TreeWidgetItemExpandMemory;

namespace ECEditor
{
    class ECComponentEditor;
    typedef std::vector<ComponentWeakPtr> ComponentWeakPtrVector;
    typedef std::list<ComponentGroup*> ComponentGroupList;

    //! Widget that will display all selected entity components and their attributes.
    /*! The ECBrowser will iterate all entity's components and pass them to an ECComponentEditor,
        which is responsible to handling component's attribute editing.

        User can add new editable entities by using AddEntity() and RemoveEntity() methods and the browser will 
        iterate through all the entity's components and pass them to ECComponentEditors.
        ECBrowser has implemented options to add, delete, copy and paste components from the selected entities by
        using a CopyComponent(), DeleteComponent(), PasteComponent() and EditXml() methods.

        User can add attributes to dynamic component by using CreateAttribute() and remove attributes with RemoveAttribute().

        \todo Try to find a way to remove the unecessary paint events when we are updating the browser parameters.
        \ingroup ECEditorModuleClient.
     */
    class ECBrowser: public QtTreePropertyBrowser
    {
        Q_OBJECT
    public:
        ECBrowser(Foundation::Framework *framework, QWidget *parent = 0);
        virtual ~ECBrowser();

        //! Insert new entity to browser.
        //! Note! to get changes visible you need to call UpdateBrowser method.
        /*! @param enity a new entity that we want to edit on the ECEditor.
         */
        void AddEntity(Scene::EntityPtr entity);

        //! Remove edited entity from the browser widget.
        /*! @param entity Entity that we want to remove on the ECEditor.
         */
        void RemoveEntity(Scene::EntityPtr entity);

        //! Return list of entities that has added to browser widget. Return empty list if no entities have been added.
        QList<Scene::EntityPtr> GetEntities() const;

        /// Sets used item expand memory. Expand memory is used to load and save the expanded items in the tree widget.
        /** @param expandMem Tree widget item expand memory.
        */
        void SetItemExpandMemory(boost::shared_ptr<TreeWidgetItemExpandMemory> expandMem) { expandMemory_ = expandMem; }

    public slots:
        //! Reset browser state to where it was after the browser initialization. Override method from the QtTreePropertyBrowser.
        void clear();

        //! Update editor data and browser ui elements if needed.
        void UpdateBrowser();

    signals:
        //! User want to open xml editor for that spesific component type.
        /*! @param componentType type name of a component.
         */
        void ShowXmlEditorForComponent(const std::string &componentType);

        //! User want to add new component for selected entities.
        void CreateNewComponent();

        //! Emitted when component is selected from the browser widget.
        /*! @param component Pointer to a component that has just been selected.
         */
        void ComponentSelected(IComponent *component);

    protected:
        //! Override from QWidget.
        void dragEnterEvent(QDragEnterEvent *event);

        //! Override from QWidget.
        void dropEvent(QDropEvent *event);

        //! Override from QWidget.
        void dragMoveEvent(QDragMoveEvent *event);

        void focusInEvent(QFocusEvent *event);

        //! dropEvent will call this after feching the information that is need from the QDropEvent data.
        bool dropMimeData(QTreeWidgetItem *item, int index, const QMimeData *data, Qt::DropAction action);

    private slots:
        //! User have right clicked the browser and QMenu need to be open to display copy, paste, delete ations etc.
        /*! @param pos Mouse click position.
         */
        void ShowComponentContextMenu(const QPoint &pos);

        //! QTreeWidget has changed it's focus and we need to highlight new entities from the editor window.
        void SelectionChanged();

        //! Called when a new component have been added to a entity.
        /*! @param comp a new component that has added into the entity.
         *  @param type attribute change type
         */
        void OnComponentAdded(IComponent* comp, AttributeChange::Type type);

        //! Called when component have been removed from the entity.
        /*! @param comp Component that will soon get removed from the entity.
         *  @param type attribute change type
         */
        void OnComponentRemoved(IComponent* comp, AttributeChange::Type type);

        //! User has selected xml edit action from a QMenu.
        void OpenComponentXmlEditor();

        //! User has selected copy action from a QMenu.
        void CopyComponent();

        //! User has selected paste action from a QMenu.
        void PasteComponent();

        //! Component's name has been changed and we need to remove component from it's previous
        //! ComponentGroup and create/add component to another componentgroup.
        void DynamicComponentChanged();

        //! Component's name has been changed and we need to remove component from it's previous ComponentGroup and create/add component to another componentgroup.
        /*! @param newName component's new name.
         */
        void ComponentNameChanged(const QString &newName);

        //! Show dialog, so that user can create a new attribute.
        //! @Note: Only works with dynamic component.
        void CreateAttribute();

        //! Remove component or attribute based on selected QTreeWidgeItem.
        /*! If selected TreeWidgetItem is a root item, then we can assume that we want to remove component.
         *  But if item has parent setted, we can assume that selected item is attribute or it's value is selected.
         */
        void OnDeleteAction();

    private:
        //! Initialize browser widget and create all connections for different QObjects.
        void InitBrowser();

        //! Try to find the right component group for spesific component type. if found return it's position on the list as in iterator format.
        //! If any component group wasn't found return .end() iterator value.
        /*! @param comp component that we want to find in some of the component group.
         */
        ComponentGroupList::iterator FindSuitableGroup(ComponentPtr comp);

        //! Try to find component group for spesific QTreeWidgetItem.
        /*! @param item QTreeWidgetItem that we want to use to find a right component group.
         */
        ComponentGroupList::iterator FindSuitableGroup(const QTreeWidgetItem &item);

        //! Add new component to existing component group if same type of component have been already added to editor,
        /*! if component type is not included, create new component group and add it to editor.
         *  @param comp a new component that we want to add into the enity.
         */
        void AddNewComponentToGroup(ComponentPtr comp);

        //! Remove component from registered componentgroup. Do nothing if component was not found of any component groups.
        /*! @param comp that we want to remove from  the component group.
         */
        void RemoveComponentFromGroup(ComponentPtr comp);

        //! Remove whole component group object from the browser.
        /*! componentGroup component group object that we want to remove from the editor.
         */
        void RemoveComponentGroup(ComponentGroup *componentGroup);

        //! Checks if entity is already added to this editor.
        bool HasEntity(Scene::EntityPtr entity) const;

        //! Remove selected attribute item from the dynamic component.
        /** @Note: This mehtod will only work with dynamic components.
        */
        void DeleteAttribute(QTreeWidgetItem *item);

        //! Remove selected component item from selected entities.
        void DeleteComponent(QTreeWidgetItem *item);

        ComponentGroupList componentGroups_;
        typedef QList<Scene::EntityWeakPtr> EntityWeakPtrList;
        EntityWeakPtrList entities_;
        QMenu *menu_;
        QTreeWidget *treeWidget_;
        Foundation::Framework *framework_;
        boost::weak_ptr<TreeWidgetItemExpandMemory> expandMemory_;
    };
}

#endif