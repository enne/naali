// For conditions of distribution and use, see copyright notice in license.txt

#ifndef incl_TtsModule_Session_h
#define incl_TtsModule_Session_h

#include "ModuleLoggingFunctions.h"
#include "TtsService.h"
#include "TtsSessionInterface.h"
#include <QMap>

namespace Foundation
{
    class Framework;
}

namespace TTS
{

	class TtsChatSession : public TtsSessionInterface
        {
            Q_OBJECT

        public:

            TtsChatSession(Foundation::Framework* framework);
            virtual ~TtsChatSession();

            virtual void Close();
			virtual TtsSessionInterface::State GetState() const;
            virtual QString Description() const;
			virtual void SpeakTextMessage(bool self_sent_message, QString sender, QString timestamp, QString message);
        
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
		};

} // TTS

#endif // incl_TtsModule_Session_h
