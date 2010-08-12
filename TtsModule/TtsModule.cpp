/**
 *  For conditions of distribution and use, see copyright notice in license.txt
 *
 *  @file   TtsChatModule.cpp
 *  @brief  Simple OpenSim world chat module. Listens for ChatFromSimulator packets and shows the chat on the UI.
 *          Outgoing chat sent using ChatFromViewer packets. Manages EC_ChatBubbles, EC_Billboards, chat logging etc.
 */

#include "StableHeaders.h"

#include "TtsModule.h"
#include "TtsChatProvider.h"
#include "ConsoleCommandServiceInterface.h"
#include "WorldStream.h"
#include "WorldLogicInterface.h"
#include "Entity.h"
#include "SceneManager.h"
#include "EventManager.h"
#include "ModuleManager.h"
#include "RealXtend/RexProtocolMsgIDs.h"
#include "NetworkMessages/NetInMessage.h"
#include "CoreStringUtils.h"
#include "GenericMessageUtils.h"
#include "ConfigurationManager.h"
#include "EC_OpenSimPresence.h"
#include "EC_OpenSimPrim.h"
#include "EC_OgrePlaceable.h"
#include "TtsChatProvider.h"
#include "MemoryLeakCheck.h"

#include <QColor>


namespace TTS
{

const std::string TtsModule::moduleName_ = std::string("TtsModule");

TtsModule::TtsModule() :
    ModuleInterface(moduleName_),
    networkStateEventCategory_(0),
    networkInEventCategory_(0),
    frameworkEventCategory_(0),
    logging_(false),
    logFile_(0)
{

}

TtsModule::~TtsModule()
{
    //SAFE_DELETE(logFile_);
}

void TtsModule::Load()
{
	//Si hay algún componente, cargar aquí.
}

void TtsModule::UnLoad()
{
	 //SAFE_DELETE(logFile_);
}
void TtsModule::Initialize()
{
	//Registra el servicio
	tts_service_ = TtsServicePtr(new TtsService(framework_));
    framework_->GetServiceManager()->RegisterService(Foundation::Service::ST_Tts, tts_service_);

	emit ServiceTtsAvailable();

	// Recoge los eventos del framework
    frameworkEventCategory_ = framework_->GetEventManager()->QueryEventCategory("Framework");

}
void TtsModule::PostInitialize()
{
}


void TtsModule::Uninitialize()
{
	tts_service_->UnregisterTtsProvider(chat_tts_provider_);

	framework_->GetServiceManager()->UnregisterService(tts_service_);
    tts_service_.reset();
}

void TtsModule::Update(f64 frametime)
{
	//if (chat_tts_provider_)
	//	chat_tts_provider_->Update(frametime);
}

bool TtsModule::HandleEvent(event_category_id_t category_id, event_id_t event_id, Foundation::EventDataInterface *data)
{
    if (category_id == frameworkEventCategory_)
    {
		if (event_id == Foundation::WORLD_STREAM_READY)
        {
			// Get settings.
			chat_tts_provider_ = new TtsChatProvider(framework_);
			if (chat_tts_provider_)
				chat_tts_provider_->RegisterToTtsService();
			return false;
        }
    }
    return false;
}

Scene::Entity *TtsModule::GetEntityWithId(const RexUUID &id)
{
    Scene::ScenePtr scene = GetFramework()->GetDefaultWorldScene();
    for(Scene::SceneManager::iterator iter = scene->begin(); iter != scene->end(); ++iter)
    {
        Scene::Entity &entity = **iter;

        boost::shared_ptr<EC_OpenSimPresence> ec_presence = entity.GetComponent<EC_OpenSimPresence>();
        boost::shared_ptr<EC_OpenSimPrim> ec_prim = entity.GetComponent<EC_OpenSimPrim>();

        if (ec_presence)
        {
            if (ec_presence->agentId == id)
                return &entity;
        }
        else if (ec_prim)
        {
            if (ec_prim->FullId == id)
                return &entity;
        }
    }

    return 0;
}

bool TtsModule::CreateLogFile()
{
    // Create filename. Format: "<server_name>_yyyy_dd_MM_<counter>.log"
    // Use QSettings for getting the application settings home dir
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, APPLICATION_NAME, "chatlogs/");
    QString path = settings.fileName();
    path.replace(".ini", "");
    QDir dir(path);
    if (!dir.exists())
        dir.mkpath(path);

    QString filename = path;
    QString server = currentWorldStream_->GetSimName().c_str();

    // Protection against invalid characters and possible evil filename injections
    server.replace('.', '_');
    server.replace(' ', '_');
    server.replace('\\', '_');
    server.replace('/', '_');
    server.replace(':', '_');
    server.replace('*', '_');
    server.replace('?', '_');
    server.replace('\"', '_');
    server.replace('<', '_');
    server.replace('>', '_');
    server.replace('|', '_');

    filename.append(server);
    filename.append('_');
    filename.append(QDate::currentDate().toString("yyyy-MM-dd"));

    // Create file
    int fileSuffixCounter = 1;
    logFile_ = new QFile;
    while(!logFile_->isOpen() && fileSuffixCounter < 100)
    {
        QString file = filename + "_" + QString("%1.log").arg(fileSuffixCounter++);
        if (!QFile::exists(file))
        {
            SAFE_DELETE(logFile_);
            logFile_ = new QFile(file);
            logFile_->open(QIODevice::WriteOnly | QIODevice::Text);
        }
    }

    if (!logFile_->isOpen())
    {
        LogError("Could not create log file for chat logging.");
        SAFE_DELETE(logFile_);
        return false;
    }

    QTextStream log(logFile_);
    QString entry = tr("Chat log created ") + QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss");
    log << entry << "\n";
    LogDebug(entry.toStdString());
    return true;
}

} // end of namespace: TTS


extern "C" void POCO_LIBRARY_API SetProfiler(Foundation::Profiler *profiler);
void SetProfiler(Foundation::Profiler *profiler)
{
    Foundation::ProfilerSection::SetProfiler(profiler);
}


using namespace TTS;

POCO_BEGIN_MANIFEST(Foundation::ModuleInterface)
    POCO_EXPORT_CLASS(TtsModule)
POCO_END_MANIFEST
