// For conditions of distribution and use, see copyright notice in license.txt

#ifndef incl_Tts_TtsService_h
#define incl_Tts_TtsService_h

#include "TtsServiceInterface.h"

#include "Framework.h"

namespace TTS
{
	
	class TtsService : public TtsServiceInterface
	{
		Q_OBJECT

    public:

		// Constructor
		TtsService(Foundation::Framework* framework_);

        //! Destructor
        virtual ~TtsService();
        
        //! Registers an tts provider
        /*! \param tts_provider Provider to register
            \return true if successfully registered
         */
		virtual bool RegisterTtsChatProvider(TTSChat::TtsProviderInterface* tts_provider);
        
        //! Unregisters an tts provider
        /*! \param tts_provider Provider to unregister
            \return true if successfully unregistered
         */
		virtual bool UnregisterTtsChatProvider();

		//! Return the ttschatsession of the provider
		virtual TTSChat::TtsSessionInterface* SessionTtschat(); 

		//! Performs time-based update
        /*! Calls update function of all registered asset providers, and of cache
            \param frametime Seconds since last frame
         */
        void Update(f64 frametime);

    private:
        
        //! Framework we belong to
        Foundation::Framework* framework_;
        
		//! Vector of Providers which are registered on the service
		TTSChat::TtsProviderInterface* tts_chat_provider_;

	private slots:

		void TtsChatSessionUnavailable();

    };
}
#endif