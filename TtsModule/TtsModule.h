/**
 *  For conditions of distribution and use, see copyright notice in license.txt
 *
 *  @file   TtsModule.h
 *  @brief  Simple OpenSim TTS module. receives the chat message and plays it 
 *			using the Festival TTS wuth the configuration established in the current session.
 */

#ifndef incl_TtsModule_h
#define incl_TtsModule_h

#include "TtsModuleApi.h"
#include "ModuleInterface.h"
#include "ModuleLoggingFunctions.h"
#include "Core.h"

#include "TtsServiceInterface.h"
#include "TtsService.h"
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
    typedef boost::weak_ptr<ProtocolModuleInterface> ProtocolWeakPtr;
}

namespace TTS
{

    class TTS_MODULE_API TtsModule :  public QObject, public Foundation::ModuleInterface
    {
        Q_OBJECT

    public:
        // Default constructor.
        TtsModule();

        // Destructor 
        virtual ~TtsModule();
        // Load function,if it has any component, are loaded here. It is not use now
        void Load();
		// Unload function. It is not use now
		void UnLoad();
		// Initializes and registers the service and emits signal of service available
        void Initialize();
		// PostInitialize function. It is not use now
        void PostInitialize();
		// Unregisters and destroys the service. 
        void Uninitialize();
		// It is not use now
        void Update(f64 frametime);
		// Initializes and registers the chat provider when the world is ready
        bool HandleEvent(event_category_id_t category_id, event_id_t event_id, Foundation::EventDataInterface* data);
        /// Returns name of this module. Needed for logging.
        static const std::string &NameStatic() { return moduleName_; } 

		//! Logging
        MODULE_LOGGING_FUNCTIONS

	signals:

		// Signal of Service available
		//void ServiceTtsAvailable();

    private:
        Q_DISABLE_COPY(TtsModule);

        /// Name of this module.
        static const std::string moduleName_;
		/// Servicio del tts
		TtsServicePtr tts_service_;
		/// Provider del Tts
		TTSChat::TtsProviderInterface* chat_tts_provider_;


        /// NetworkState event category.
        event_category_id_t networkStateEventCategory_;
        /// NetworkIn event category.
        event_category_id_t networkInEventCategory_;
        /// Framework event category
        event_category_id_t frameworkEventCategory_;
    };
}  // end of namespace: TTS

#endif // incl_TtsModule_h
