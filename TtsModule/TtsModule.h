/**
 *  For conditions of distribution and use, see copyright notice in license.txt
 *
 *  @file   TtsChatModule.h
 *  @brief  Simple OpenSim world chat module. Listens for ChatFromSimulator packets and shows the chat on the UI.
 *          Outgoing chat sent using ChatFromViewer packets. Manages EC_ChatBubbles, EC_Billboards, chat logging etc.
 */

#ifndef incl_TtsModule_h
#define incl_TtsModule_h

#include "TtsModuleApi.h"
#include "ModuleInterface.h"
#include "ModuleLoggingFunctions.h"
#include "Core.h"
#include "TtsService.h"
#include "TtsProviderInterface.h"
#include "TtsChatProvider.h"
#include <QObject>


class RexUUID;

namespace Foundation
{
    class EventDataInterface;
}

namespace ProtocolUtilities
{
    class ProtocolModuleInterface;
    class WorldStream;
    typedef boost::weak_ptr<ProtocolModuleInterface> ProtocolWeakPtr;
    typedef boost::shared_ptr<WorldStream> WorldStreamPtr;
}

namespace UiServices
{
    class UiModule;
}

QT_BEGIN_NAMESPACE
class QColor;
class QFile;
QT_END_NAMESPACE

namespace TTS
{

	class TtsService;

    class TTS_MODULE_API TtsModule :  public QObject, public Foundation::ModuleInterface
    {
        Q_OBJECT

    public:
        /// Default constructor.
        TtsModule();

        /// Destructor 
        virtual ~TtsModule();

        /// ModuleInterfaceImpl overrides.
        void Load();
		void UnLoad();
        void Initialize();
        void PostInitialize();
        void Uninitialize();
        void Update(f64 frametime);
        bool HandleEvent(event_category_id_t category_id, event_id_t event_id, Foundation::EventDataInterface* data);
        
        /// Returns name of this module. Needed for logging.
        static const std::string &NameStatic() { return moduleName_; } 

		//! Logging
        MODULE_LOGGING_FUNCTIONS

	signals:

		void ServiceTtsAvailable();

    private:
        Q_DISABLE_COPY(TtsModule);

        /// Returns primitive or avatar entity with the wanted ID.
        /// @param id ID of the entity.
        /// @return Entity pointer matching the id or 0 if not found.
        Scene::Entity *GetEntityWithId(const RexUUID &id);
        /// Creates file for logging.
        /// @return True if the creation was succesful, false otherwise.
        bool CreateLogFile();
		// Recoge la información sobre el avatar
		//virtual bool GetAvatarPosition(Vector3df& position, Vector3df& direction);


        /// Name of this module.
        static const std::string moduleName_;
		/// Servicio del tts
		TtsServicePtr tts_service_;
		/// Provider del Tts
		TtsChatProvider* chat_tts_provider_;


        /// NetworkState event category.
        event_category_id_t networkStateEventCategory_;
        /// NetworkIn event category.
        event_category_id_t networkInEventCategory_;
        /// Framework event category
        event_category_id_t frameworkEventCategory_;

        /// WorldStream pointer
        ProtocolUtilities::WorldStreamPtr currentWorldStream_ ;
        /// UiModule pointer.
        boost::weak_ptr<UiServices::UiModule> uiModule_;

        /// Do we want to log the chat messages.
        bool logging_;
        /// Log file.
        QFile *logFile_;
    };
}  // end of namespace: TTS

#endif // incl_TtsModule_h
