// For conditions of distribution and use, see copyright notice in license.txt

#include "StableHeaders.h"

#include "ProtocolModuleOpenSim.h"
#include "RealXtendWorldSession.h"
#include "Framework.h"
#include "ModuleManager.h"

namespace OpenSimProtocol
{
    RealXtendWorldSession::RealXtendWorldSession(Foundation::Framework *framework) :
        framework_(framework), credentials_(ProtocolUtilities::AT_RealXtend)
    {
        networkOpensim_ = framework_->GetModuleManager()->GetModule<OpenSimProtocol::ProtocolModuleOpenSim>();
    }

    RealXtendWorldSession::~RealXtendWorldSession()
    {
    }

    bool RealXtendWorldSession::StartSession(const LoginCredentials &credentials, const QUrl &serverEntryPointUrl)
    {
        bool success = false;
        if (credentials.GetType() == ProtocolUtilities::AT_RealXtend)
        {
            // Set Url and Credentials
            serverEntryPointUrl_ = ValidateUrl(serverEntryPointUrl.toString(), WorldSessionInterface::OpenSimServer);
            credentials_ = credentials;
            credentials_.SetAuthenticationUrl( ValidateUrl(credentials_.GetAuthenticationUrl().toString(), WorldSessionInterface::RealXtendAuthenticationServer) );

            // Try do RealXtend auth based login with ProtocolModuleOpenSim
            success = LoginToServer(
                credentials_.GetPassword(),
                serverEntryPointUrl_.host(),
                QString::number(serverEntryPointUrl_.port()),
                credentials_.GetAuthenticationUrl().host(),
                QString::number(credentials_.GetAuthenticationUrl().port()),
                credentials_.GetIdentity(),
                credentials_.GetStartLocation(),
                GetConnectionThreadState());
        }
        else
        {
            ProtocolModuleOpenSim::LogInfo("Invalid credential type, must be RealXtend for RealXtendWorldSession");
            success = false;
        }

        return success;
    }

    bool RealXtendWorldSession::LoginToServer(
        const QString& password,
        const QString& address,
        const QString& port,
        const QString& auth_server_address_noport,
        const QString& auth_server_port,
        const QString& auth_login,
        const QString& start_location,
        ProtocolUtilities::ConnectionThreadState *thread_state)
    {
        boost::shared_ptr<OpenSimProtocol::ProtocolModuleOpenSim> spOpenSim = networkOpensim_.lock();
        if (spOpenSim.get())
        {
            spOpenSim->SetAuthenticationType(ProtocolUtilities::AT_RealXtend);
            OpenSimLoginThread *loginWorker = spOpenSim->GetLoginWorker();
            if (!loginWorker)
                return false;
            if (loginWorker->GetState() == ProtocolUtilities::Connection::STATE_CONNECTED)
            {
                ProtocolModuleOpenSim::LogInfo("RealXtendWorldSession::LoginToServer() - Already in connected state. Something went bad on diconnect before you got here!");
                return false;
            }
            loginWorker->PrepareRealXtendLogin(password, address, port, thread_state, auth_login, 
                auth_server_address_noport, auth_server_port, start_location);
            connect(loginWorker, SIGNAL(LoginStateChanged(int)), SLOT(HandleLoginStateChange(int)), Qt::UniqueConnection);

            // Start the login thread.
            boost::thread(boost::ref(*loginWorker));
        }
        else
        {
            ProtocolModuleOpenSim::LogInfo("Could not lock ProtocolModuleOpenSim");
            return false;
        }

        return true;
    }

    QUrl RealXtendWorldSession::ValidateUrl(const QString &urlString, const UrlType urlType)
    {
        QUrl returnUrl(urlString);
        switch(urlType)
        {
        case WorldSessionInterface::OpenSimServer:
            if (returnUrl.port() == -1)
            {
                returnUrl.setPort(9000);
                ProtocolModuleOpenSim::LogInfo("OpenSimServer url had no port, using default 9000");
            }
            break;

        case WorldSessionInterface::OpenSimGridServer:
            if (returnUrl.port() == -1)
            {
                returnUrl.setPort(8002);
                ProtocolModuleOpenSim::LogInfo("OpenSimGridServer url had no port, using default 8002");
            }
            break;

        case WorldSessionInterface::RealXtendAuthenticationServer:
            if (returnUrl.port() == -1)
            {
                returnUrl.setPort(10001);
                ProtocolModuleOpenSim::LogInfo("RealXtendAuthenticationServer url had no port, using default 10001");
            }
            break;
        default:
            break;
        }

        if (returnUrl.isValid())
            return returnUrl;
        else
        {
            ProtocolModuleOpenSim::LogInfo("Invalid connection url");
            return QUrl();
        }
    }

    LoginCredentials RealXtendWorldSession::GetCredentials() const
    {
        return credentials_;
    }

    QUrl RealXtendWorldSession::GetServerEntryPointUrl() const
    {
        return serverEntryPointUrl_;
    }

    void RealXtendWorldSession::GetWorldStream() const
    {
    }

    void RealXtendWorldSession::SetCredentials(const LoginCredentials &credentials)
    {
        if (credentials.GetType() == ProtocolUtilities::AT_RealXtend)
            credentials_ = credentials;
        else
            ProtocolModuleOpenSim::LogInfo("Could not set credentials, invalid type. Must be RealXtend for RealXtendWorldSession");
    }

    void RealXtendWorldSession::SetServerEntryPointUrl(const QUrl &newUrl)
    {
        serverEntryPointUrl_ = newUrl;
    }

    void RealXtendWorldSession::HandleLoginStateChange(int state)
    {
        ProtocolUtilities::Connection::State loginState = (ProtocolUtilities::Connection::State)state;
        ProtocolModuleOpenSim::LogDebug("RexAuth login in process: " + NetworkStateToString(loginState));
        if (loginState == ProtocolUtilities::Connection::STATE_LOGIN_FAILED)
        {
            // Dont emit this if we are already connected, the state change might be false from
            // another worker thread "instance" or thread run of the constuctor. 
            // Caused crashes after teleports without the check!
            if (networkOpensim_.lock()->GetLoginWorker()->GetState() != ProtocolUtilities::Connection::STATE_CONNECTED)
                emit LoginFailed(networkOpensim_.lock()->GetLoginWorker()->GetErrorMessage().c_str());
        }
        else if (loginState == ProtocolUtilities::Connection::STATE_CONNECTED)
        {
            emit LoginSuccessful();
        }
    }
}
