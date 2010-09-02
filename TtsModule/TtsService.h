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
		virtual void SpeakTextMessage(QString message);
		// Return the own voice
		virtual const Voice getVoice();
		// Set the own voice
		virtual void setVoice(Voice voice);
		// Set the voice of the others
		virtual void setActiveOwnVoice(bool active);
		// return true if the own voce is active
		virtual bool isActiveOwnVoice();
		// Active the others voice
		virtual void setActiveOthersVoice(bool active);
		// return true if the others voce is active
		virtual bool isActiveOthersVoice();
		//??
        void Update(f64 frametime);

    private:
        
        //! Framework we belong to
        Foundation::Framework* framework_;

		// Flag of voices (active or not)
		//Por defecto desactivado
		bool activeOwnVoice_,activeOthersVoice_;
		Voice voice_;

    };
}
#endif