#include "StableHeaders.h"
#include "TtsModule.h"
#include "TtsService.h"
#include "Framework.h"
#include "EventManager.h"

namespace TTS
{
	TtsService::TtsService(Foundation::Framework* framework) : 
        framework_(framework),
		providers_(0),
		event_category_(0)
    {
    }
    
    TtsService::~TtsService()
    {
		providers_.clear();
    }
        
	TtsSessionInterface* TtsService::SessionTtschat()
    {
		TtsProviderVector::iterator i = providers_.begin();
        while (i != providers_.end())
        {
			if ((*i)->getType()==TtsProviderInterface::Type::CHAT)
            {
                return (*i)->Session();
            }            
            ++i;
        }      
        return 0;
    }
	
	bool TtsService::RegisterTtsProvider(TtsProviderInterface* tts_provider)
    {
		if (!tts_provider)
        {
            return false;
        }

 		TtsProviderVector::iterator i = providers_.begin();
        while (i != providers_.end())
        {
            if ((*i) == tts_provider)
            {
                return false;
            }            
            ++i;
        }        
        
        providers_.push_back(tts_provider);  
		connect(tts_provider, SIGNAL(SessionUnavailable()), SLOT(TtsChatSessionUnavailable()) );
		emit TtsAvailable(); //envía la señal y la recoge communicationwidget
		return true;
    }
    
	bool TtsService::UnregisterTtsProvider(TtsProviderInterface* tts_provider)
    {
		if (!tts_provider)
		{
			return false;
		}
		
		TtsProviderVector::iterator i = providers_.begin();
		while (i != providers_.end())
		{
			if ((*i) == tts_provider)
			{
				tts_provider->CloseSession();
				disconnect(tts_provider);
				providers_.erase(i);
				return true;
			}            
			++i;
		}  
		return false;
    }    
        
    void TtsService::Update(f64 frametime)
    {
    }

	void TtsService::TtsChatSessionUnavailable()
    {
        emit TtsUnavailable();
    }
}
