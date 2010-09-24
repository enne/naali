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
		Voice EN3;
		Voice EN4;
		Voice EN5;
		Voice EN6;
		Voice CAT1;
		Voice CAT2;
		Voice FI;
	};

	static const AvailableVoice Voices = {"-ES1","-ES2","-EN1","-EN2","-EN3","-EN4","-EN5","-EN6","-CAT1","-CAT2","-FI"};


	class TTSServiceInterface : public QObject, public Foundation::ServiceInterface
	{
		Q_OBJECT

    public:
        
	    virtual ~TTSServiceInterface() {}

		virtual void text2Speech(QString message, Voice voice) = 0;
        virtual void text2WAV(QString message, QString pathAndFileName, Voice voice) = 0;
		virtual void text2PHO(QString message, QString pathAndFileName, Voice voice) = 0;

		virtual void file2Speech(QString pathAndFileName, Voice voice) = 0;
        virtual void file2WAV(QString pathAndFileNameIn, QString pathAndFileNameOut, Voice voice) = 0;
		virtual void file2PHO(QString pathAndFileNameIn, QString pathAndFileNameOut, Voice voice) = 0;

		//virtual const Voice getVoice() = 0;
		//virtual void setVoice(Voice voice) = 0;
	};

   	typedef boost::shared_ptr<TTSServiceInterface> TTSServicePtr;

}

#endif
 