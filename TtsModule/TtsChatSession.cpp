#include "StableHeaders.h"

#include "TtsChatSession.h"

namespace TTS
{
	namespace TTSChat
	{

		std::string TtsChatSession::type_name_static_ = "TtsChat";

        TtsChatSession::TtsChatSession(Foundation::Framework* framework) : 
            state_(STATE_CONNECTING),
            description_("Esta es la sesión del tts"),
			activeOwnVoice_(false),
			activeOthersVoice_(false)
        {
			configuration_=new TtsChatConfiguration();
        }

        TtsChatSession::~TtsChatSession()
        {
        }

        void TtsChatSession::Close()
        {
            if (state_ != STATE_CLOSED && state_ != STATE_ERROR)
            {
                state_ = STATE_CLOSED;
            }
        }

        const TtsSessionInterface::State TtsChatSession::GetState() 
        {
            return state_;
        }

        const QString TtsChatSession::Description()
        {
            return description_;
        }

        void TtsChatSession::Update(f64 frametime)
        {
        }
		void TtsChatSession::SpeakTextMessage(bool self_sent_message, QString sender, QString timestamp, QString message)
		{
			std::string msg;
			std::stringstream commandoss;
			std::string commandos;
			commandoss << "start /B festival.exe --libdir \"festival/lib\" "; 
			if(self_sent_message)
				commandoss << configuration_->getOwnVoice();
			else
				commandoss << configuration_->getOthersVoice();
			commandoss << " -A -T \"";
			msg=message.toStdString();
			std::replace_if(msg.begin(),msg.end(),boost::is_any_of("\""),', ');
			commandoss << msg;
			commandoss << "\"";
			commandos = commandoss.str();
			system(commandos.c_str());	
		}
		const Voice TtsChatSession::GetOwnVoice()
		{
			return configuration_->getOwnVoice();
		}
		const Voice TtsChatSession::GetOthersVoice()
		{
			return configuration_->getOthersVoice();
		}
		void TtsChatSession::SetOwnVoice(Voice voice)
		{
			configuration_->setOwnVoice(voice);
		}
		void TtsChatSession::SetOthersVoice(Voice voice)
		{
			configuration_->setOthersVoice(voice);
		}
		void TtsChatSession::SetActiveOwnVoice(bool active)
		{
			activeOwnVoice_=active;
		}
		bool TtsChatSession::IsActiveOwnVoice()
		{
			return activeOwnVoice_;
		}
		void TtsChatSession::SetActiveOthersVoice(bool active)
		{
			activeOthersVoice_=active;
		}
		bool TtsChatSession::IsActiveOthersVoice()
		{
			return activeOthersVoice_;
		}

	}
} // TTS