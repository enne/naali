/**
 *  For conditions of distribution and use, see copyright notice in license.txt
 *
 *  @file   EC_TtsVoice.h
 *  @brief  EC_TtsVoice Tts voice component wich allows use Tts function on entity.
*/

#ifndef incl_TTS_EC_TtsVoice_h
#define incl_TTS_EC_TtsVoice_h

#include "ComponentInterface.h"
#include "Declare_EC.h"
#include "TtsServiceInterface.h"
#include "AttributeInterface.h"

#include <QString>

class EC_TtsVoice : public Foundation::ComponentInterface
{
	DECLARE_EC(EC_TtsVoice);

public:
    /// Destructor.
    ~EC_TtsVoice();

    /// Sets voice for the entity.
    /// @param voice Voice.
	void SetMyVoice(const TTS::Voice voice);

	/// Gets the voice of entity.
	TTS::Voice GetMyVoice() const;

	/// Return true if the component is serializable in XML
	virtual bool IsSerializable() const { return true; }

    /// Send the message to tts service to play it
    /// @param msg Message to be shown.
	/// @param voice The Voice to play
    /// @note If there is not param voice, plays the own voice.
	void SpeakMessage(const QString msg, TTS::Voice voice);
	void SpeakMessage(const QString msg);
	void SpeakMessage();

	/// Attributes
	Foundation::Attribute<std::string> voice_;
	Foundation::Attribute<std::string> message_;

private:

    /// Constuctor.
    /// @param module Owner module.
    explicit EC_TtsVoice(Foundation::ModuleInterface *module);

    /// Tts pointer.
	TTS::TTSServiceInterface* ttsService_;
};

#endif
