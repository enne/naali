/**
 *  For conditions of distribution and use, see copyright notice in license.txt
 *
 *  @file   NotificationTTS.cpp
 *  @brief  Uses TTSModule for the notification messages
 *			
 */

#include "StableHeaders.h"

#include "NotificationTTS.h"
//
//#include "ConsoleCommandServiceInterface.h"
//#include "WorldStream.h"
//#include "WorldLogicInterface.h"
//#include "Entity.h"
//#include "SceneManager.h"
//#include "EventManager.h"
//#include "ModuleManager.h"
//#include "RealXtend/RexProtocolMsgIDs.h"
//#include "NetworkMessages/NetInMessage.h"
//#include "CoreStringUtils.h"
//#include "GenericMessageUtils.h"
//#include "ConfigurationManager.h"
//#include "EC_OpenSimPresence.h"
//#include "EC_OpenSimPrim.h"
//#include "EC_OgrePlaceable.h"
//#include "MemoryLeakCheck.h"
//#include "SceneEvents.h"

#include "TtsServiceInterface.h"
#include "UiServiceInterface.h" 


#include <QColor>


namespace NotifiTTS
{

	const std::string NotificationTTS::moduleName_ = std::string("NotificationTTS");

	NotificationTTS::NotificationTTS() :
		ModuleInterface(moduleName_)
	{

	}

	NotificationTTS::~NotificationTTS()
	{
	}

	void NotificationTTS::Load()
	{
		//Si hay algún componente, cargar aquí.
	}

	void NotificationTTS::UnLoad()
	{
	}

	void NotificationTTS::Initialize()
	{
		if (framework_ &&  framework_->GetServiceManager())
        {
			Foundation::UiServiceInterface *ui = framework_->GetService<Foundation::UiServiceInterface>();
            if (ui)
				connect(ui, SIGNAL(Notification(const QString&)), SLOT(Notification2Speech(const QString&)));
		}

	}
	void NotificationTTS::PostInitialize()
	{
	}


	void NotificationTTS::Uninitialize()
	{

	}
	
	void NotificationTTS::Notification2Speech(const QString &message)
	{
		tts_service_ = framework_->GetService<TTS::TTSServiceInterface>();
		if (!tts_service_)
			return;
		tts_service_->text2Speech(message, TTS::Voices.EN4);
	}
	
} // end of namespace: Namespace


extern "C" void POCO_LIBRARY_API SetProfiler(Foundation::Profiler *profiler);
void SetProfiler(Foundation::Profiler *profiler)
{
    Foundation::ProfilerSection::SetProfiler(profiler);
}


using namespace NotifiTTS;

POCO_BEGIN_MANIFEST(Foundation::ModuleInterface)
    POCO_EXPORT_CLASS(NotificationTTS)
POCO_END_MANIFEST
