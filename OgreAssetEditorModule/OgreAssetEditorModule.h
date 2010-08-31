/**
 *  For conditions of distribution and use, see copyright notice in license.txt
 *
 *  @file   OgreAssetEditorModule.h
 *  @brief  OgreAssetEditorModule.provides editing and previewing tools for
 *          OGRE assets such as meshes and material scripts.
 */

#ifndef incl_OgreAssetEditorModule_OgreAssetEditorModule_h
#define incl_OgreAssetEditorModule_OgreAssetEditorModule_h

#include "ModuleInterface.h"
#include "ModuleLoggingFunctions.h"

#include <QObject>

namespace Foundation
{
    class UiServiceInterface;
    class EventDataInterface;
}

namespace Inventory
{
    class InventoryUploadEventData;
    class InventoryUploadBufferEventData;
}

namespace Naali
{
    class MaterialWizard;
    class EditorManager;

    class OgreAssetEditorModule : public QObject, public Foundation::ModuleInterface
    {
        Q_OBJECT

    public:
        /// Default constructor.
        OgreAssetEditorModule();

        /// Destructor.
        ~OgreAssetEditorModule();

        /// ModuleInterfaceImpl overrides.
        void Initialize();
        void PostInitialize();
        void Uninitialize();
        void Update(f64 frametime);
        bool HandleEvent(event_category_id_t category_id, event_id_t event_id, Foundation::EventDataInterface* data);

        MODULE_LOGGING_FUNCTIONS

        /// Returns name of this module. Needed for logging.
        static const std::string &NameStatic() { return type_name_static_; }

    public slots:
        /// Uploads new asset from file.
        /// @param data Inventory upload event data.
        void UploadFile(Inventory::InventoryUploadEventData *data);

        /// Uploads new asset from buffer.
        /// @param data Inventory buffer upload event data.
        void UploadBuffer(Inventory::InventoryUploadBufferEventData *data);

    private:
        Q_DISABLE_COPY(OgreAssetEditorModule);

        /// Type name of this module.
        static std::string type_name_static_;

        /// UI service.
        boost::weak_ptr<Foundation::UiServiceInterface> uiService_;

        /// Event manager pointer.
        Foundation::EventManagerPtr eventManager_;

        /// Inventory event category.
        event_category_id_t frameworkEventCategory_;

        /// Inventory event category.
        event_category_id_t inventoryEventCategory_;

        /// Asset event category.
        event_category_id_t assetEventCategory_ ;

        /// Resource event category.
        event_category_id_t resourceEventCategory_;

        /// NetworkState event category.
        event_category_id_t networkStateEventCategory_;

        /// Editor manager.
        EditorManager *editorManager_;

        /// Material wizard.
        MaterialWizard *materialWizard_;
    };
}

#endif
