// For conditions of distribution and use, see copyright notice in license.txt

#ifndef incl_TTS_TTSService_h
#define incl_TTS_TTSService_h

#include "TTSServiceInterface.h"
#include "Framework.h"

namespace TTS
{
	
	class TTSService : public TTSServiceInterface
	{
		Q_OBJECT

    public:

		// Constructor
		TTSService(Foundation::Framework* framework_);

        //! Destructor
        virtual ~TTSService();
        
		// Plays the message using the Festival TTS with the current configuration if the voice is active
		virtual void text2Speech(QString message, TTS::Voice voice);


		virtual void text2WAV(QString message, QString pathAndFileName, TTS::Voice voice);

		virtual void text2PHO(QString message,QString pathAndFileName, TTS::Voice voice);

		// Return the voice
		//virtual const Voice getVoice();
		// Set the voice
		//irtual void setVoice(Voice voice);


		//??
        void Update(f64 frametime);

    private:
        
        //! Framework we belong to
        Foundation::Framework* framework_;

		// Flag of voices (active or not)
		//Por defecto desactivado
		//Voice voice_;
		
    };
}
#endif