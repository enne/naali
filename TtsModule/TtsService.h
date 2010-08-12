// For conditions of distribution and use, see copyright notice in license.txt

#ifndef incl_Tts_TtsService_h
#define incl_Tts_TtsService_h

#include "TtsServiceInterface.h"

#include "NetworkEvents.h"
#include "Framework.h"
#include "EventManager.h"

namespace Foundation
{
    class Framework;
}

namespace TTS
{
	class TtsService : public TtsServiceInterface
	{
		Q_OBJECT

    public:

		TtsService(Foundation::Framework* framework_);

        //! Destructor
        virtual ~TtsService();
        
        //! Registers an tts provider
        /*! \param asset_provider Provider to register
            \return true if successfully registered
         */
		virtual bool RegisterTtsProvider(TtsProviderInterface* tts_provider);
        
        //! Unregisters an asset provider
        /*! \param asset_provider Provider to unregister
            \return true if successfully unregistered
         */
		virtual bool UnregisterTtsProvider(TtsProviderInterface* tts_provider);
        
        //! Performs time-based update
        /*! Calls update function of all registered asset providers, and of cache
            \param frametime Seconds since last frame
         */
		virtual TtsSessionInterface* SessionTtschat(); 
		//virtual TTS::TtsChatSession* SessionTtsChat() const;

        void Update(f64 frametime);

    private:
        
        //! Framework we belong to
        Foundation::Framework* framework_;
        
        //! Asset event category
        event_category_id_t event_category_;

		typedef std::vector<TtsProviderInterface*> TtsProviderVector;
        TtsProviderVector providers_;

	private slots:

		void TtsChatSessionUnavailable();

    };
}
#endif