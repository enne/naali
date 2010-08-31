/**
 *  For conditions of distribution and use, see copyright notice in license.txt
 *
 *  @file   TtsModule.cpp
 *  @brief  Simple OpenSim TTS module. receives the chat message and plays it 
 *			using the Festival TTS wuth the configuration established in the current session.
 */

#include "StableHeaders.h"

#include "TtsModule.h"

#include "ConsoleCommandServiceInterface.h"
#include "WorldStream.h"
#include "WorldLogicInterface.h"
#include "Entity.h"
#include "SceneManager.h"
#include "EventManager.h"
#include "ModuleManager.h"
#include "RealXtend/RexProtocolMsgIDs.h"
#include "NetworkMessages/NetInMessage.h"
#include "CoreStringUtils.h"
#include "GenericMessageUtils.h"
#include "ConfigurationManager.h"
#include "EC_OpenSimPresence.h"
#include "EC_OpenSimPrim.h"
#include "EC_OgrePlaceable.h"
#include "MemoryLeakCheck.h"
#include "SceneEvents.h"

#include <QColor>


namespace TTS
{

	const std::string TtsModule::moduleName_ = std::string("TtsModule");

	TtsModule::TtsModule() :
		ModuleInterface(moduleName_),
		networkStateEventCategory_(0),
		networkInEventCategory_(0),
		frameworkEventCategory_(0)
	{

	}

	TtsModule::~TtsModule()
	{
	}

	void TtsModule::Load()
	{
		//Si hay algún componente, cargar aquí.
	}

	void TtsModule::UnLoad()
	{
	}

	void TtsModule::Initialize()
	{
		//Registra el servicio
		tts_service_ = TtsServicePtr(new TtsService(framework_));
		framework_->GetServiceManager()->RegisterService(Foundation::Service::ST_Tts, tts_service_);

		//emit ServiceTtsAvailable();

		// Recoge los eventos del framework
		frameworkEventCategory_ = framework_->GetEventManager()->QueryEventCategory("Framework");

	}
	void TtsModule::PostInitialize()
	{
	}


	void TtsModule::Uninitialize()
	{
		tts_service_->UnregisterTtsChatProvider();

		framework_->GetServiceManager()->UnregisterService(tts_service_);
		tts_service_.reset();
	}

	void TtsModule::Update(f64 frametime)
	{
		//if (chat_tts_provider_)
		//	chat_tts_provider_->Update(frametime);
	}

	bool TtsModule::HandleEvent(event_category_id_t category_id, event_id_t event_id, Foundation::EventDataInterface *data)
	{
		if (category_id == frameworkEventCategory_)
		{
			if(event_id == Foundation::WORLD_STREAM_READY)
			{
				// Get settings.
				chat_tts_provider_ = new TTSChat::TtsChatProvider(framework_);
				if (chat_tts_provider_)
					chat_tts_provider_->RegisterToTtsService();
				return false;
			}
		}
		return false;
	}

} // end of namespace: TTS


extern "C" void POCO_LIBRARY_API SetProfiler(Foundation::Profiler *profiler);
void SetProfiler(Foundation::Profiler *profiler)
{
    Foundation::ProfilerSection::SetProfiler(profiler);
}


using namespace TTS;

POCO_BEGIN_MANIFEST(Foundation::ModuleInterface)
    POCO_EXPORT_CLASS(TtsModule)
POCO_END_MANIFEST
