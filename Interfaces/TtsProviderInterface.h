// For conditions of distribution and use, see copyright notice in license.txt

#ifndef incl_Interfaces_TtsProviderInterface_h
#define incl_Interfaces_TtsProviderInterface_h

#include "Framework.h"
#include "EventManager.h"
#include "TtsSessionInterface.h"

#include <QObject>
#include <Vector3D.h>
#include <QList>
#include <QByteArray>
#include <QString>
#include <QDateTime>

namespace TTS
{

	class TtsProviderInterface;
	typedef boost::shared_ptr<TtsProviderInterface> TtsProviderPtr;  

	class TtsProviderInterface : public QObject
    {
		Q_OBJECT

    public:
        
		const enum Type {CHAT, LEARNING};
        virtual ~TtsProviderInterface() {};
		virtual void Update(f64 frametime){};
		virtual TtsSessionInterface* Session() = 0;
		virtual Type getType() = 0;
		virtual void CloseSession(){};

	signals:

		void SessionUnavailable();
    };



} //end Foundation

#endif // end incl_Interfaces_TtsProviderInterface_h
