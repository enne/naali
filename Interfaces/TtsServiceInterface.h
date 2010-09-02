// For conditions of distribution and use, see copyright notice in license.txt

#ifndef incl_Interfaces_TTSServiceInterface_h
#define incl_Interfaces_TTSServiceInterface_h

#include "ServiceInterface.h"

#include <QObject>
#include <Vector3D.h>
#include <QList>
#include <QByteArray>
#include <QString>
#include <QDateTime>

namespace TTS
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


	class TTSServiceInterface : public QObject, public Foundation::ServiceInterface
	{
		Q_OBJECT

    public:
        
	    virtual ~TTSServiceInterface() {}

		virtual void SpeakTextMessage(QString message) = 0;
        
		virtual const Voice getVoice() = 0;
		virtual void setVoice(Voice voice) = 0;
		virtual void setActiveOwnVoice(bool active) = 0;
		virtual bool isActiveOwnVoice() = 0;
		virtual void setActiveOthersVoice(bool active) = 0;
		virtual bool isActiveOthersVoice() = 0;

	};

   	typedef boost::shared_ptr<TTSServiceInterface> TTSServicePtr;

}

#endif
