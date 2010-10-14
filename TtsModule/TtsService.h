//$ HEADER_NEW_FILE $ 
// For conditions of distribution and use, see copyright notice in license.txt

#ifndef incl_Tts_TtsService_h
#define incl_Tts_TtsService_h

#include "Framework.h"

#include "TtsServiceInterface.h"


namespace Tts
{
	
	class TtsService : public TtsServiceInterface
	{
		Q_OBJECT

    public:

		// Constructor
		TtsService(Foundation::Framework* framework_);

        //! Destructor
        virtual ~TtsService();
        
		virtual void Text2Speech(QString message, Tts::Voice voice);
     	virtual void Text2WAV(QString message, QString pathAndFileName, Tts::Voice voice);
		virtual void Text2PHO(QString message,QString pathAndFileName, Tts::Voice voice);


		virtual void File2Speech(QString pathAndFileName, Voice voice);
        virtual void File2WAV(QString pathAndFileNameIn, QString pathAndFileNameOut, Voice voice);
		virtual void File2PHO(QString pathAndFileNameIn, QString pathAndFileNameOut, Voice voice);


		//virtual const Voice GetVoice();
		//virtual void SetVoice(Voice voice);

    private:
        
        //! Framework we belong to
        Foundation::Framework* framework_;

		//Voice voice_;
		
    };
}
#endif