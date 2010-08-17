#include "StableHeaders.h"
#include "TtsChatProvider.h"
#include "TtsChatSession.h"
#include "TtsModule.h"

namespace TTS
{
	namespace TTSChat
	{
        TtsChatProvider::TtsChatProvider(Foundation::Framework* framework) :
            framework_(framework),
            description_("Tts in-world chat"),
            session_(0)
        {
        }

        TtsChatProvider::~TtsChatProvider()
        {
            SAFE_DELETE(session_);
        }

        void TtsChatProvider::Update(f64 frametime)
        {
        }

		TtsSessionInterface* TtsChatProvider::Session()
        {
			if (session_ && session_->GetState() == TtsSessionInterface::STATE_CLOSED)
                SAFE_DELETE(session_) //! \todo USE SHARED PTR, SOMEONE MIGHT HAVE POINTER TO SESSION OBJECT !!!!

            if (!session_)
            {
                session_ = new TtsChatSession(framework_);
            }
            return session_;
        }

        QString& TtsChatProvider::Description()
        {
            return description_;
        }

        void TtsChatProvider::CloseSession()
        {
			if (session_){
				disconnect(session_);
                session_->Close();
				session_=0;
			}
            emit SessionUnavailable();
        }

		void TtsChatProvider::RegisterToTtsService()
        {
            if (framework_ &&  framework_->GetServiceManager())
            {
				boost::shared_ptr<TTS::TtsServiceInterface> tts = framework_->GetServiceManager()->GetService<TTS::TtsServiceInterface>(Foundation::Service::ST_Tts).lock();
                if (tts.get())
                {
                    tts->RegisterTtsChatProvider(this);
                }
                return;
            }
        }

	}
} // TTS
