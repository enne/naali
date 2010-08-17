#include "StableHeaders.h"
#include "TtsModule.h"
#include "TtsService.h"
#include "Framework.h"
#include "EventManager.h"
#include "TtsChatProvider.h"

namespace TTS
{
	TtsService::TtsService(Foundation::Framework* framework) : 
        framework_(framework),
		tts_chat_provider_(0),
		event_category_(0)
    {
    }
    
    TtsService::~TtsService()
    {
		UnregisterTtsChatProvider();
    }
        
	TTSChat::TtsSessionInterface* TtsService::SessionTtschat()
    {
		if(!tts_chat_provider_)
			return 0;
		else
			return tts_chat_provider_->Session();
    }
	
	bool TtsService::RegisterTtsChatProvider(TTSChat::TtsProviderInterface* tts_provider)
    {
		if (!tts_provider)
        {
            return false;
        }

		tts_chat_provider_=tts_provider;
		connect(tts_chat_provider_, SIGNAL(SessionUnavailable()), SLOT(TtsChatSessionUnavailable()) );
		emit TtsAvailable(); //envía la señal y la recoge communicationwidget
		return true;
    }
    
	bool TtsService::UnregisterTtsChatProvider()
    {
		if (!tts_chat_provider_)
		{
			return false;
		}
		tts_chat_provider_->CloseSession();
		disconnect(tts_chat_provider_);
		return true;
    }    
        
    void TtsService::Update(f64 frametime)
    {
    }

	void TtsService::TtsChatSessionUnavailable()
    {
        emit TtsUnavailable();
    }
}
