#include "StableHeaders.h"

#include "TtsChatConfiguration.h"

namespace TTS
{
	namespace TTSChat
	{

		TtsChatConfiguration::TtsChatConfiguration():
		OwnVoice_(Voices.ES1),
		OthersVoice_(Voices.ES2)
		{
		}
		const TTS::TTSChat::Voice TtsChatConfiguration::getOwnVoice()
		{
			return OwnVoice_;
		}
		const TTS::TTSChat::Voice TtsChatConfiguration::getOthersVoice()
		{
			return OthersVoice_;
		}
		void TtsChatConfiguration::setOwnVoice(TTS::TTSChat::Voice voice)
		{
			OwnVoice_=voice;
		}
		void TtsChatConfiguration::setOthersVoice(TTS::TTSChat::Voice voice)
		{
			OthersVoice_=voice;
		}
	}
}