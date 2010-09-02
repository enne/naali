/**
 *  For conditions of distribution and use, see copyright notice in license.txt
 *
 *  @file   TTSModule.cpp
 *  @brief  Simple OpenSim TTS module. receives the chat message and plays it 
 *			using the Festival TTS wuth the configuration established in the current session.
 */

#include "StableHeaders.h"

#include "TTSModule.h"

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

	const std::string TTSModule::moduleName_ = std::string("TTSModule");

	TTSModule::TTSModule() :
		ModuleInterface(moduleName_),
		networkStateEventCategory_(0),
		networkInEventCategory_(0),
		frameworkEventCategory_(0)
	{

	}

	TTSModule::~TTSModule()
	{
	}

	void TTSModule::Load()
	{
		//Si hay algún componente, cargar aquí.
	}

	void TTSModule::UnLoad()
	{
	}

	void TTSModule::Initialize()
	{
		//Registra el servicio
		tts_service_ = TTSServicePtr(new TTSService(framework_));
		framework_->GetServiceManager()->RegisterService(Foundation::Service::ST_TTS, tts_service_);

		// Recoge los eventos del framework
		//?
		frameworkEventCategory_ = framework_->GetEventManager()->QueryEventCategory("Framework");

	}
	void TTSModule::PostInitialize()
	{
	}


	void TTSModule::Uninitialize()
	{

		framework_->GetServiceManager()->UnregisterService(tts_service_);
		tts_service_.reset();
	}

	void TTSModule::Update(f64 frametime)
	{

	}

	bool TTSModule::HandleEvent(event_category_id_t category_id, event_id_t event_id, Foundation::EventDataInterface *data)
	{
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
    POCO_EXPORT_CLASS(TTSModule)
POCO_END_MANIFEST
