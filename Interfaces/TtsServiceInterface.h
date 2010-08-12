// For conditions of distribution and use, see copyright notice in license.txt

#ifndef incl_Interfaces_TtsServiceInterface_h
#define incl_Interfaces_TtsServiceInterface_h

#include "ServiceInterface.h"
#include "TtsProviderInterface.h"
#include "TtsSessionInterface.h"

#include <QObject>
#include <Vector3D.h>
#include <QList>
#include <QByteArray>
#include <QString>
#include <QDateTime>

namespace TTS
{

	class TtsServiceInterface : public QObject, public Foundation::ServiceInterface
	{
		Q_OBJECT

    public:
        
	    virtual ~TtsServiceInterface() {}

		//virtual TtsChatSession* SessionTtsChat() const = 0;
		virtual TtsSessionInterface* SessionTtschat() = 0;

		virtual bool RegisterTtsProvider(TtsProviderInterface* tts_provider) = 0;

        virtual bool UnregisterTtsProvider(TtsProviderInterface* tts_provider) = 0;

	signals:

		void TtsAvailable();
		void TtsUnavailable();

	};

   	typedef boost::shared_ptr<TtsServiceInterface> TtsServicePtr;

}

#endif
