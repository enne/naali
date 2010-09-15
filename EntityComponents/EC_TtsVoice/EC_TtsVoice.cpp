/**
 *  For conditions of distribution and use, see copyright notice in license.txt
 *
 *  @file   EC_TtsVoice.cpp
 *  @brief  EC_TtsVoice Tts voice component wich allows use Tts function on entity.
*/

#include "StableHeaders.h"

#include "EC_TtsVoice.h"
#include "ModuleInterface.h"
#include "Entity.h"


EC_TtsVoice::EC_TtsVoice(Foundation::ModuleInterface *module) :
	Foundation::ComponentInterface(module->GetFramework()),
	voice_(this, "voice", TTS::Voices.ES1),
    message_(this, "message", "")
{
	// Get TTS service
	ttsService_ = framework_->GetService<TTS::TTSServiceInterface>();
	//voice_=TTS::Voices.ES1;
}

EC_TtsVoice::~EC_TtsVoice()
{
}

void EC_TtsVoice::SetMyVoice(const TTS::Voice voice)
{
	voice_.Set(voice,AttributeChange::Local);
	//voice_=voice;
	ComponentChanged(AttributeChange::Local);
	//ComponentChanged(AttributeChange::Network);
}

TTS::Voice EC_TtsVoice::GetMyVoice() const
{
	return voice_.Get();
}

bool EC_TtsVoice::IsActive() const
{
	return true;
}

void EC_TtsVoice::SpeakMessage(const QString msg, TTS::Voice voice)
{
	if(!ttsService_)
		ttsService_ = framework_->GetService<TTS::TTSServiceInterface>();
	
	ttsService_->text2Speech(msg,voice);
}

void EC_TtsVoice::SpeakMessage(const QString msg)
{
	if(!ttsService_)
		ttsService_ = framework_->GetService<TTS::TTSServiceInterface>();
	
	ttsService_->text2Speech(msg,voice_.Get());
}

void EC_TtsVoice::SpeakMessage()
{
	if(!ttsService_)
		ttsService_ = framework_->GetService<TTS::TTSServiceInterface>();

	ttsService_->text2Speech(message_.Get().c_str(),voice_.Get());
}