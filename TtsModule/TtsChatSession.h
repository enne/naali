/**
 *  For conditions of distribution and use, see copyright notice in license.txt
 *
 *  @file   TtsChatSession.h
 *  @brief  The session of TTS Chat is the interface with the Festival and it keeps the configuration
 *			voice (if is active or not and the TtsChatConfiguration)
 *			
 */

#ifndef incl_TtsModule_Session_h
#define incl_TtsModule_Session_h

#include "ModuleLoggingFunctions.h"
#include "TtsServiceInterface.h"
#include "TtsChatConfiguration.h"

namespace Foundation
{
    class Framework;
}

namespace TTS
{

	namespace TTSChat
	{

	class TtsChatSession : public TtsSessionInterface
        {
            Q_OBJECT

        public:

			// Constructor
            TtsChatSession(Foundation::Framework* framework);
			// Destructor
            virtual ~TtsChatSession();
			// Change the state to closed
            virtual void Close();
			// Return the current state of session
			virtual const State GetState();
			// Return a description of session
            virtual const QString Description();
			// Plays the message using the Festival TTS with the current configuration if the voice is active
			virtual void SpeakTextMessage(bool self_sent_message, QString sender, QString timestamp, QString message);
			// Return the own voice
			virtual const Voice GetOwnVoice();
			// Return the voice of the others
			virtual const Voice GetOthersVoice();
			// Set the own voice
			virtual void SetOwnVoice(Voice voice);
			// Set the voice of the others
			virtual void SetOthersVoice(Voice voice);
			// Active the own voice
			virtual void SetActiveOwnVoice(bool active);
			// return true if the own voce is active
			virtual bool IsActiveOwnVoice();
			// Active the others voice
			virtual void SetActiveOthersVoice(bool active);
			// return true if the others voce is active
			virtual bool IsActiveOthersVoice();

			//! Logging
		    MODULE_LOGGING_FUNCTIONS

			// Returns name of this session. Needed for logging.
	        static const std::string &NameStatic() { return type_name_static_; }
			// It is not use now
			void Update(f64 frametime);

        private:
			// Name of this session.
            static std::string type_name_static_;
			// State of session
            TtsSessionInterface::State state_;
			// Description of session
            QString description_;
			// Configuration of voice
			TtsChatConfigurationInterface* configuration_;
			// Flag of voices (active or not)
			bool activeOwnVoice_,activeOthersVoice_;
		};
	}
} // TTS

#endif // incl_TtsModule_Session_h
