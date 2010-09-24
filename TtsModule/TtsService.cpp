#include "StableHeaders.h"

#include "TTSService.h"


namespace TTS
{
	TTSService::TTSService(Foundation::Framework* framework) : 
        framework_(framework)
			//voice_(Voices.ES1)
    {

    }
		
    TTSService::~TTSService()
    {

    }
    

	void TTSService::text2Speech(QString message, Voice voice)
	{

		std::stringstream commandoss;
		std::string commandos,msg;
		commandoss << "start /B festival.exe --libdir \"festival/lib\" "; 

		
		commandoss << voice;

		commandoss << " -A -T \"";

		msg=message.toStdString();
		std::replace_if(msg.begin(),msg.end(),boost::is_any_of("\""),', ');
		commandoss << msg;
		commandoss << "\"";
		commandos = commandoss.str();

		system(commandos.c_str());	
	}

	void TTSService::text2WAV(QString message, QString pathAndFileName, Voice voice)
	{

		std::string msg;
		msg=message.toStdString();

		std::stringstream commandoss;
		std::string commandos;
		commandoss << "start /B festival.exe --libdir \"festival/lib\" "; 
		commandoss << voice;
		commandoss << " -W ";
		commandoss << pathAndFileName.toStdString();
		commandoss << " -T \"";

		std::replace_if(msg.begin(),msg.end(),boost::is_any_of("\""),', ');
		commandoss << msg;
		commandoss << "\"";
		commandos = commandoss.str();

		system(commandos.c_str());	

	}

	void TTSService::text2PHO(QString message, QString pathAndFileName, Voice voice)
	{

		std::string msg;
		msg=message.toStdString();

		std::stringstream commandoss;
		std::string commandos;
		commandoss << "start /B festival.exe --libdir \"festival/lib\" "; 
		commandoss << voice;
		commandoss << " -P ";
		commandoss << pathAndFileName.toStdString();
		commandoss << " -T \"";

		std::replace_if(msg.begin(),msg.end(),boost::is_any_of("\""),', ');
		commandoss << msg;
		commandoss << "\"";
		commandos = commandoss.str();

		system(commandos.c_str());	

	}
	
	void TTSService::file2Speech(QString pathAndFileName, Voice voice)
	{
		std::stringstream commandoss;
		std::string commandos,file;
		commandoss << "start /B festival.exe --libdir \"festival/lib\" "; 

		
		commandoss << voice;

		commandoss << " -A -F \"";

		file=pathAndFileName.toStdString();

		commandoss << file;
		commandoss << "\"";
		commandos = commandoss.str();

		system(commandos.c_str());	
	}
	void TTSService::file2WAV(QString pathAndFileNameIn, QString pathAndFileNameOut, Voice voice)
	{
		std::string fileIn;
		fileIn=pathAndFileNameIn.toStdString();

		std::stringstream commandoss;
		std::string commandos;
		commandoss << "start /B festival.exe --libdir \"festival/lib\" "; 
		commandoss << voice;
		commandoss << " -W ";
		commandoss << pathAndFileNameOut.toStdString();
		commandoss << " -F \"";

		commandoss << fileIn;
		commandoss << "\"";
		commandos = commandoss.str();

		system(commandos.c_str());	

	}
	void TTSService::file2PHO(QString pathAndFileNameIn, QString pathAndFileNameOut, Voice voice)
	{
		std::string fileIn;
		fileIn=pathAndFileNameIn.toStdString();

		std::stringstream commandoss;
		std::string commandos;
		commandoss << "start /B festival.exe --libdir \"festival/lib\" "; 
		commandoss << voice;
		commandoss << " -P ";
		commandoss << pathAndFileNameOut.toStdString();
		commandoss << " -F \"";

		commandoss << fileIn;
		commandoss << "\"";
		commandos = commandoss.str();

		system(commandos.c_str());	
	}



	/*const Voice TTSService::getVoice()
	{
		return voice_;
	}

	void TTSService::setVoice(Voice voice)
	{
		voice_=voice;
	}*/



    void TTSService::Update(f64 frametime)
    {
    }


}
