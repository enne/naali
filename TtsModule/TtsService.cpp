#include "StableHeaders.h"

#include "TTSService.h"


namespace TTS
{
	TTSService::TTSService(Foundation::Framework* framework) : 
        framework_(framework),
			voice_(Voices.ES1)
    {
    }
		
    TTSService::~TTSService()
    {

    }
    

	void TTSService::SpeakTextMessage(QString message)
	{
		std::string msg;
		std::stringstream commandoss;
		std::string commandos;
		commandoss << "start /B festival.exe --libdir \"festival/lib\" "; 

		//H5 Aquí coge la voz del.
	
		QRegExp rxlen("^<voice>(.*)</voice>(.*)$");
		int pos = rxlen.indexIn(message);
		QString voice;
		
		if (pos > -1) 
		{
			voice = rxlen.cap(1); 
			msg = rxlen.cap(2).toStdString();
		}
		commandoss << voice.toStdString();
		commandoss << " -A -T \"";

		std::replace_if(msg.begin(),msg.end(),boost::is_any_of("\""),', ');
		commandoss << msg;
		commandoss << "\"";
		commandos = commandoss.str();

		system(commandos.c_str());	
	}

	
	
	const Voice TTSService::getVoice()
	{
		return voice_;
	}

	void TTSService::setVoice(Voice voice)
	{
		voice_=voice;
	}

	void TTSService::setActiveOwnVoice(bool active)
	{
		activeOwnVoice_=active;
	}

	bool TTSService::isActiveOwnVoice()
	{
		return activeOwnVoice_;
	}
	void TTSService::setActiveOthersVoice(bool active)
	{
		activeOthersVoice_=active;
	}

	bool TTSService::isActiveOthersVoice()
	{
		return activeOthersVoice_;
	}

    void TTSService::Update(f64 frametime)
    {
    }


}
