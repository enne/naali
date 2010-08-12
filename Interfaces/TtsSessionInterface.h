// For conditions of distribution and use, see copyright notice in license.txt

#ifndef incl_Interfaces_TtsSessionInterface_h
#define incl_Interfaces_TtsSessionInterface_h

#include <QObject>
#include <Vector3D.h>
#include <QList>
#include <QByteArray>
#include <QString>
#include <QDateTime>


namespace TTS
{
        class TtsSessionInterface : public QObject
        {
			Q_OBJECT

        public:
			
			enum State { STATE_CONNECTING, STATE_AUTHENTICATING, STATE_OPEN, STATE_CLOSED, STATE_ERROR };

			virtual ~TtsSessionInterface(){}

			virtual TtsSessionInterface::State GetState() const = 0;

			virtual void SpeakTextMessage(bool self_sent_message, QString sender, QString timestamp, QString message) = 0;

			virtual void Update(f64 frametime) = 0;

			virtual void Close() = 0;


		};

		typedef boost::shared_ptr<TtsSessionInterface> TtsSessionPtr;  
} // Foundation

#endif // incl_Interfaces_TtsSessionInterface_h
