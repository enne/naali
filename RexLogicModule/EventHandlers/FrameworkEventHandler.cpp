// For conditions of distribution and use, see copyright notice in license.txt

#include "StableHeaders.h"
#include "RexLogicModule.h"
#include "EventHandlers/FrameworkEventHandler.h"
#include "EventHandlers/LoginHandler.h"

#include "WorldStream.h"
#include "ScriptServiceInterface.h"
#include "ServiceManager.h"

#include <QMap>

namespace RexLogic
{

bool FrameworkEventHandler::HandleFrameworkEvent(event_id_t event_id, IEventData* data)
{
    if (event_id == Foundation::PROGRAM_OPTIONS)
    {
        QMap<int, QString> map;
        QString command, parameter;
        Foundation::ProgramOptionsEvent *po_event = static_cast<Foundation::ProgramOptionsEvent*>(data);

        for( int count = 0; count < po_event->argc; count++ )
            map[count] = QString(po_event->argv[count]);

        command = map[1];
        parameter = map[2];

        if (!command.isEmpty())
        {
            if (command == "-loginuri" && !parameter.isEmpty() && parameter.startsWith("http://", Qt::CaseInsensitive))
            {
                // Fix this to work again later (taiga login from command line)
                //CoreUi::LoginContainer *login_ui = rexLogic_->GetLogin();
                //if (login_ui)
                //    login_ui->StartParameterLoginTaiga(parameter);
                //else
                //    rexLogic_->LogInfo("Could not do parameter login due login path now awailable"); // Fix after refactor
            }

            if (command == "-python" || command == "-p")
            {
                if (!parameter.isEmpty())
                {
                    boost::shared_ptr<Foundation::ScriptServiceInterface> pyservice =
                        rexLogic_->GetFramework()->GetServiceManager()->GetService<Foundation::ScriptServiceInterface>(Foundation::Service::ST_PythonScripting).lock();
                    if (pyservice)
                        pyservice->RunScript(parameter);
                    else
                        RexLogicModule::LogError("Python scripting service not available.");
                }
                else
                    RexLogicModule::LogError("Expected script filename as parameter.");
            }
        }
    }

    return false;
}

}
