/**
 *  For conditions of distribution and use, see copyright notice in license.txt
 *
 *  @file   NotificationTTS.h
 *  @brief  Esto es un m�dulo vac�o que sirve como basa para desarrollar cualquier m�dulo.
 *			
 */

#ifndef incl_NotificationTTS_h
#define incl_NotificationTTS_h


#include "NotificationTTSApi.h"
#include "ModuleInterface.h"
#include "ModuleLoggingFunctions.h"
#include "Core.h"

#include <QObject>


class RexUUID;

namespace Foundation
{
    class EventDataInterface;
	class UiServiceInterface;
}

namespace ProtocolUtilities
{
    class ProtocolModuleInterface;
    typedef boost::weak_ptr<ProtocolModuleInterface> ProtocolWeakPtr;
}

namespace TTS
{
	class TTSServiceInterface;
	
}

namespace NotifiTTS
{

    class NOTIFICATIONTTS_API NotificationTTS :  public QObject, public Foundation::ModuleInterface
    {
        Q_OBJECT

    public:
        // Constructor.
        NotificationTTS();
        // Destructor 
        virtual ~NotificationTTS();
        // Funci�n de carga. Si hay componentes se cargan aqu�.
        void Load();
		// Funcio�n de descarga
		void UnLoad();
		// Inicializa y registra el servicio/interfaz
        void Initialize();
		// Funci�n de postinicializaci�n
        void PostInitialize();
		// Elimina el servicio del registro
        void Uninitialize();
    
        // Devuelve el nombre del m�dulo, utilizado para logging
        static const std::string &NameStatic() { return moduleName_; } 

		//! Logging
        MODULE_LOGGING_FUNCTIONS

	
    private:
        Q_DISABLE_COPY(NotificationTTS);

        // Nombre del m�dulo
        static const std::string moduleName_;
		TTS::TTSServiceInterface* tts_service_;
	
	private slots:
		void Notification2Speech(const QString &message);
    };
}  // end of namespace: Namespace

#endif // incl_NotificationTTS_h
