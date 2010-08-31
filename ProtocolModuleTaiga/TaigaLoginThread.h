/**
 *  For conditions of distribution and use, see copyright notice in license.txt
 *
 *  @file TaigaLoginThread.h
 *  @brief Taiga XML-RPC login worker.
 */

#ifndef incl_ProtocolModuleTaiga_TaigaLoginThread_h
#define incl_ProtocolModuleTaiga_TaigaLoginThread_h

#include "NetworkEvents.h"
#include <QString>

namespace Foundation
{
    class Framework;
}

namespace TaigaProtocol
{
    /// Taiga XML-RPC login worker.
    class TaigaLoginThread : public QObject
    {
        Q_OBJECT

    public:
        /// Default constructor.
        TaigaLoginThread();

        /// Destructor.
        virtual ~TaigaLoginThread();

        /// Set framework
        void SetFramework(Foundation::Framework* framework) { framework_  = framework; }

        /// Thread entry point.
        void operator()();

        /**
         * Sets up the XML-RPC login procedure using authentication server or direct rexserver depending of 
         * given @p callMethod param and @p authentication param. All data which will be got from authentication of
         * login into world server is saved in @p ConnectionThreadState struct. 
         *
         * @param worldAddress is a address of world (sim) server without port value. 
         * @param worldPort is a port of world (sim) server. 
         */
        void PrepareTaigaLogin(
            const QString &worldAddress,
            const QString &worldPort,
            ProtocolUtilities::ConnectionThreadState *thread_state);

        /// Performs the actual XML-RPC login procedure.
        ///@return true if login (or authentication) was successful.
        bool PerformXMLRPCLogin();

        /// Change the state of the XML-RPC worker.
        void SetConnectionState(ProtocolUtilities::Connection::State state)
        {
            threadState_->state = state;
            emit LoginStateChanged((int)state);
        }

        ///@return State of connection.
        volatile ProtocolUtilities::Connection::State GetState() const;

        ///@return error message as std::string
        std::string &GetErrorMessage() const;

        ///@return The client parameters retreived from the XML-RPC reply.
        const ProtocolUtilities::ClientParameters &GetClientParameters() const { return threadState_->parameters; }

        ///@return True, if the XML-RPC worker is ready.
        const bool IsReady() const { return ready_; }

        const std::string GetUsername() const { return ""; }
        const std::string GetPassword() const { return ""; }

    signals:
        void LoginStateChanged(int state);

    private:
        Q_DISABLE_COPY(TaigaLoginThread);

        /// Triggers the XML-RPC login procedure.
        bool start_login_;

        /// Indicates that the XML-RPC worker is ready to perform.
        bool ready_;

        /// Information which is received via the XML-RPC reply from the server.
        ProtocolUtilities::ConnectionThreadState *threadState_;

        /// Framework pointer
        Foundation::Framework* framework_;

        /// Information needed for the XML-RPC login procedure.
        std::string worldAddress_;
        std::string worldPort_;

        static std::string LOGIN_TO_SIMULATOR;
        static std::string OPTIONS;
    };
}

#endif
