// For conditions of distribution and use, see copyright notice in license.txt

#ifndef incl_TtsModule_Session_h
#define incl_TtsModule_Session_h

#include "ModuleLoggingFunctions.h"
#include "TtsServiceInterface.h"
#include "TtsChatConfiguration.h"

#include <QMap>

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

            TtsChatSession(Foundation::Framework* framework);
            virtual ~TtsChatSession();

            virtual void Close();
			virtual const State GetState();
            virtual const QString Description();
			virtual void SpeakTextMessage(bool self_sent_message, QString sender, QString timestamp, QString message);
			virtual const Voice GetOwnVoice();
			virtual const Voice GetOthersVoice();
			virtual void SetOwnVoice(Voice voice);
			virtual void SetOthersVoice(Voice voice);

			//! Logging
		    MODULE_LOGGING_FUNCTIONS
	        static const std::string &NameStatic() { return type_name_static_; }
			void Update(f64 frametime);


        private:

            bool GetOwnAvatarPosition(Vector3df& position, Vector3df& direction);
            QString OwnAvatarId();
            QString GetAvatarFullName(QString uuid) const;

            static std::string type_name_static_;
            Foundation::Framework* framework_;
            TtsSessionInterface::State state_;
            QString description_;
			TtsChatConfigurationInterface* configuration_;

		};
	}
} // TTS

#endif // incl_TtsModule_Session_h
