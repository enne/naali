// For conditions of distribution and use, see copyright notice in license.txt

#ifndef incl_Interfaces_TtsServiceInterface_h
#define incl_Interfaces_TtsServiceInterface_h

#include "ServiceInterface.h"

#include <QObject>
#include <Vector3D.h>
#include <QList>
#include <QByteArray>
#include <QString>
#include <QDateTime>

namespace TTS
{

	namespace TTSChat
	{
		typedef std::string Voice;

		struct AvailableVoice
		{
			Voice ES1;
			Voice ES2;
			Voice EN1;
			Voice EN2;
		};

		static const AvailableVoice Voices = {"-ES1","-ES2","-EN1","-EN2"};

		class TtsChatConfigurationInterface
		{

        public:

			virtual const Voice getOwnVoice() = 0;
			virtual const Voice getOthersVoice() = 0;
			virtual void setOwnVoice(Voice voice) = 0;
			virtual void setOthersVoice(Voice voice) = 0;
		};

		class TtsSessionInterface : public QObject
        {
            Q_OBJECT

        public:

			enum State { STATE_CONNECTING, STATE_AUTHENTICATING, STATE_OPEN, STATE_CLOSED, STATE_ERROR };

			virtual ~TtsSessionInterface(){};
            virtual void Close() = 0;
			virtual const State GetState() = 0;
            virtual const QString Description() = 0;
			virtual void SpeakTextMessage(bool self_sent_message, QString sender, QString timestamp, QString message) = 0;
        
			virtual const Voice GetOwnVoice() = 0;
			virtual const Voice GetOthersVoice() = 0;
			virtual void SetOwnVoice(Voice voice) = 0;
			virtual void SetOthersVoice(Voice voice) = 0;
			virtual void SetActiveOwnVoice(bool active) = 0;
			virtual bool IsActiveOwnVoice() = 0;
			virtual void SetActiveOthersVoice(bool active) = 0;
			virtual bool IsActiveOthersVoice() = 0;
		};
	typedef boost::shared_ptr<TtsSessionInterface> TtsSessionPtr;  


	class TtsProviderInterface : public QObject
        {
            Q_OBJECT

        public:

			virtual ~TtsProviderInterface(){};
            virtual TtsSessionInterface* Session() = 0;
            virtual QString& Description() = 0;
  			virtual void RegisterToTtsService() = 0; 
			virtual void CloseSession() = 0;

		signals:

			void SessionUnavailable();
        };

		typedef boost::shared_ptr<TtsProviderInterface> TtsProviderPtr;  

	}





	class TtsServiceInterface : public QObject, public Foundation::ServiceInterface
	{
		Q_OBJECT

    public:
        
	    virtual ~TtsServiceInterface() {}

		virtual TTSChat::TtsSessionInterface* SessionTtschat()=0;

		virtual bool RegisterTtsChatProvider(TTSChat::TtsProviderInterface* tts_provider) = 0;

        virtual bool UnregisterTtsChatProvider() = 0;


	signals:

		void TtsAvailable();
		void TtsUnavailable();

	};

   	typedef boost::shared_ptr<TtsServiceInterface> TtsServicePtr;

}

#endif
