#ifndef incl_TtsModule_TtsChatconfiguration_h
#define incl_TtsModule_TtsChatconfiguration_h

#include "TtsServiceInterface.h"

namespace TTS
{
	namespace TTSChat
	{
		class TtsChatConfiguration : public TtsChatConfigurationInterface
		{

        public:

			TtsChatConfiguration();
			virtual const Voice getOwnVoice();
			virtual const Voice getOthersVoice();
			virtual void setOwnVoice(Voice voice);
			virtual void setOthersVoice(Voice voice);
		
		private:

			Voice OwnVoice_,OthersVoice_;
		};
	}
}

#endif