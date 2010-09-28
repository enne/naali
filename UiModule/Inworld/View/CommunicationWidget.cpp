// For conditions of distribution and use, see copyright notice in license.txt

#include "StableHeaders.h"
#include "DebugOperatorNew.h"

#include "CommunicationWidget.h"

#include "UiProxyWidget.h"
#include "UiModule.h"
#include "ModuleManager.h"
#include "VoiceUsersWidget.h"
#include "VoiceControl.h"
#include "VoiceController.h"
#include "UiServiceInterface.h"

#include <CommunicationsService.h>
#include <QWidget>
#include <QStackedLayout>
#include <QTimer>
#include <QGraphicsSceneMouseEvent>
#include <QApplication>
#include <QGraphicsScene>
#include <QTextBrowser>

#include "DebugOperatorNew.h"

#include "TtsServiceInterface.h"
#include "UiServiceInterface.h"
#include "TtsModule.h"

#include "Entity.h"
#include "ServiceInterface.h"
#include "WorldLogicInterface.h"

#include "RexUUID.h"
#include "SceneManager.h"
#include "EC_OpenSimPresence.h"

namespace
{
    /// HTTP schema indentifier
    const QString &cHttpSchema = "http://";

    /// HTTP schema indentifier
    const QString &cHttpsSchema = "https://";

    /// Hyperlink start tag
    const QString &cLinkStartTag = "<a href=\"";

    /// Hyperlink middle tag
    const QString &cLinkMiddleTag= "\">";

    /// Hyperlink end tag
    const QString &cLinkEndTag= "</a>";

    /// Finds valid hyperlinks in message and generates HTML tags for them
    /// @param message Message to be parsed
    /// @param indentifier Schema indentifier e.g. "http://"
    void GenerateHyperlinks(QString &message, const QString &indentifier)
    {
        QString link;
        int startIndex = 0, endIndex = 0;
        int hyperlinkCount = message.count(indentifier);
        while (hyperlinkCount > 0)
        {
            startIndex = message.indexOf(indentifier, endIndex);
            assert(startIndex != -1);

            endIndex = message.indexOf(' ', startIndex);
            endIndex = endIndex > -1 ? endIndex : message.length();
            assert(endIndex > startIndex);

            link = message.mid(startIndex, endIndex - startIndex);

            message.insert(endIndex, cLinkEndTag);
            message.insert(startIndex, cLinkMiddleTag);
            message.insert(startIndex, link);
            message.insert(startIndex, cLinkStartTag);

            endIndex += link.length();
            --hyperlinkCount;
        }
    }
}

namespace CoreUi
{
    CommunicationWidget::CommunicationWidget(Foundation::Framework* framework) :
        framework_(framework),
        QGraphicsProxyWidget(),
        internal_widget_(new QWidget()),
        im_proxy_(0),
        viewmode_(Normal),
        resizing_horizontal_(false),
        resizing_vertical_(false),
        voice_state_widget_(0),
        voice_users_info_widget_(0),
        voice_users_proxy_widget_(0),
        in_world_chat_session_(0),
        voice_controller_proxy_widget_(0),
        voice_controller_widget_(0),
		Tts_chat_widget(0)
    {
        Initialise();
        ChangeView(viewmode_);
    }

    // Private

    void CommunicationWidget::Initialise()
    {
        setupUi(internal_widget_);
        setWidget(internal_widget_);

        // Hide IM contenat are (and button) by default. Shown when UpdateImWidget is called.
        imContentWidget->hide();

        // Stacked layout
        stacked_layout_ = new QStackedLayout();
        stacked_layout_->setMargin(0);
        contentContainerLayout->addLayout(stacked_layout_);

        // History view mode
        history_view_text_edit_ = new QTextBrowser(chatContentWidget);
        history_view_text_edit_->setOpenExternalLinks(true);
        history_view_text_edit_->setObjectName("historyViewTextEdit");
        history_view_text_edit_->setStyleSheet(
            "QTextBrowser#historyViewTextEdit {"
                "background-color: rgba(34,34,34,191);"
                "border-radius: 7px; border: 1px solid rgba(255,255,255,50);"
            "}");
        history_view_text_edit_->setFont(QFont("Calibri", 11));
        stacked_layout_->addWidget(history_view_text_edit_);

        // Slim view mode
        normal_view_widget_ = new NormalChatViewWidget(chatContentWidget);
        stacked_layout_->addWidget(normal_view_widget_);

        stacked_layout_->setCurrentWidget(normal_view_widget_);

        connect(viewModeButton, SIGNAL( clicked() ), SLOT( ChangeViewPressed() ));
        connect(imButton, SIGNAL( clicked() ), SLOT( ToggleImWidget() ));
        connect(chatLineEdit, SIGNAL( returnPressed() ), SLOT( SendMessageRequested() ));

        HideVoiceControls();

        HideTtsChatControls();

        // Initialize In-World Voice
        if (framework_ &&  framework_->GetServiceManager())
        {
            Communications::ServiceInterface *comm = framework_->GetService<Communications::ServiceInterface>();
            if (comm)
            {
                connect(comm, SIGNAL(InWorldVoiceAvailable()), SLOT(InitializeInWorldVoice()) );
                connect(comm, SIGNAL(InWorldChatAvailable()), SLOT(InitializeInWorldChat()) );
                connect(comm, SIGNAL(InWorldChatUnavailable()), SLOT(InitializeInWorldChat()) );

				connect(comm, SIGNAL(InWorldChatAvailable()), SLOT(InitializeInWorldTts()));
            }
        }

    }

    void CommunicationWidget::ShowVoiceControls()
    {
        this->voiceLabel->show();
        this->voiceLeftBorder->show();
        if (voice_state_widget_)
            voice_state_widget_->show();
        if (voice_users_info_widget_)
            voice_users_info_widget_->show();
    }

    void CommunicationWidget::HideVoiceControls()
    {
        this->voiceLabel->hide();
        this->voiceLeftBorder->hide();
        if (voice_state_widget_)
            voice_state_widget_->hide();
        if (voice_users_info_widget_)
            voice_users_info_widget_->hide();
    }

	void CommunicationWidget::ShowTtsChatControls()
    {
		this->ttsContentWidget->show();
		this->ttsButton->show();
    }

    void CommunicationWidget::HideTtsChatControls()
    {
		this->ttsContentWidget->hide();
		this->ttsButton->hide();
    }

    void CommunicationWidget::ChangeViewPressed()
    {
        switch (viewmode_)
        {
            case Normal:
                ChangeView(History);
                break;
            case History:
                ChangeView(Normal);
                break;
        }
    }

    void CommunicationWidget::ChangeView(ViewMode new_mode)
    {
        viewmode_ = new_mode; 
        switch (viewmode_)
        {
            case Normal:
                chatContentWidget->setStyleSheet("QWidget#chatContentWidget { background-color: transparent; border-top-left-radius: 0px; border-top-right-radius: 0px; }");
                viewModeButton->setStyleSheet("QPushButton#viewModeButton { background-image: url('./data/ui/images/chat/uibutton_HISTORY_normal.png'); }");
                stacked_layout_->setCurrentWidget(normal_view_widget_);
                break;
            case History:
                chatContentWidget->setStyleSheet("QWidget#chatContentWidget { background-color: rgba(34,34,34,191); border-top-left-radius: 7px; border-top-right-radius: 7px; }");
                viewModeButton->setStyleSheet("QPushButton#viewModeButton { background-image: url('./data/ui/images/chat/uibutton_HISTORY_click.png'); }");
                stacked_layout_->setCurrentWidget(history_view_text_edit_);
                break;
        }
    }

	void CommunicationWidget::ToggleTtsChatWidget()
    {
		if(Tts_chat_widget)
		{
		   if(!tts_proxy_->isVisible())
			   tts_proxy_->show();
		   else
			   tts_proxy_->AnimatedHide();
		}
   }

    void CommunicationWidget::ToggleImWidget()
    {
        if (im_proxy_)
        {
            if (!im_proxy_->isVisible())
            {
                im_proxy_->show();
                // \todo Find a proper solution to the problem
                // IM widget doesn't get input without main frame resisizing for unknow reason.
                // HACK begin
                im_proxy_->moveBy(1,1);
                im_proxy_->moveBy(-1,-1);
                // HACK end
            }
            else
                im_proxy_->AnimatedHide();
        }
    }

    void CommunicationWidget::ToggleVoice()
    {
        if (!in_world_voice_session_)
            return;

        if (voice_controller_proxy_widget_)
        {
            if (voice_controller_proxy_widget_->isVisible())
                voice_controller_proxy_widget_->AnimatedHide();
            else
            {
                voice_controller_proxy_widget_->show();
                voice_controller_proxy_widget_->moveBy(1,1);
                voice_controller_proxy_widget_->moveBy(-1,-1);
            }
        }
    }

    void CommunicationWidget::ShowIncomingMessage(bool self_sent_message, QString sender, QString timestamp, QString message)
    {
        // History view
        timestamp = timestamp.midRef(timestamp.indexOf(" ")+1).toString(); // Cut the fat from timestamp for now
        QString htmlcontent("<span style='color:grey'>[");
        htmlcontent.append(timestamp);
        if (!self_sent_message)
            htmlcontent.append("]</span> <span style='color:#0099FF;'>");
        else
            htmlcontent.append("]</span> <span style='color:#FF3330;'>");
        htmlcontent.append(sender);
        htmlcontent.append(": </span><span style='color:#EFEFEF;'>");

        // If the message contains hyperlinks, make HTML tags for them.
        if (message.contains(cHttpSchema))
            GenerateHyperlinks(message, cHttpSchema);
        if (message.contains(cHttpsSchema))
            GenerateHyperlinks(message, cHttpsSchema);

        htmlcontent.append(message);
        htmlcontent.append("</span>");

        history_view_text_edit_->append(htmlcontent);

        // Normal view
        if (!self_sent_message)
            normal_view_widget_->ShowChatMessage(self_sent_message, QString("%1: %2").arg(sender, message));
        else
            normal_view_widget_->ShowChatMessage(self_sent_message, QString("Me: %1").arg(message));
    }

    void CommunicationWidget::SendMessageRequested()
    {
        if (chatLineEdit->text().isEmpty())
            return;

        QString message = chatLineEdit->text();
        chatLineEdit->clear();

		if (tts_service_)
		{
			//QString voice;
			//Tts::Voice ownVoice_ = tts_config_->getOwnVoice();
			//QString oVoice_ = ownVoice_.c_str();

			if(!avatar_voice_)
				GetAvatarVoiceComponent();
			
			QString voice;
			Tts::Voice ownVoice_ = avatar_voice_->GetMyVoice();

			QTextStream(&voice) << "<voice>" << ownVoice_.c_str() << "</voice>";
			message =voice+message;
		}
		
        if (in_world_chat_session_)
            in_world_chat_session_->SendTextMessage(message);
    }

	void CommunicationWidget::SpeakIncomingMessage(const Communications::InWorldChat::TextMessageInterface &message, const QString& from_uuid)
	{
		if((message.IsOwnMessage() && tts_config_->isActiveOwnVoice()) || (!message.IsOwnMessage() && tts_config_->isActiveOthersVoice()))
		{		
			QString hour_str = QString::number(message.TimeStamp().time().hour());
			QString minute_str = QString::number(message.TimeStamp().time().minute());
			QString time_stamp_str = QString("%1:%2").arg(hour_str, 2, QChar('0')).arg(minute_str, 2, QChar('0'));

			//Splits voice and message
			QString msg;
			QRegExp rxlen("^<voice>(.*)</voice>(.*)$");
			int pos = rxlen.indexIn(message.Text());
			QString Qvoice;
			Tts::Voice voice;

			if (pos > -1) 
			{
				Qvoice = rxlen.cap(1); 
				msg = rxlen.cap(2);
			}
			
			voice=Qvoice.toStdString();
			
			if(!avatar_voice_)
				GetAvatarVoiceComponent();

			avatar_voice_->SpeakMessage(msg,voice);

			//Scene::ScenePtr current_scene = framework_->GetDefaultWorldScene();
			//boost::shared_ptr<EC_TtsVoice> other_voice;
			//RexUUID* uuid=new RexUUID(from_uuid);
			//EC_OpenSimPresence *presence_component;

			//if (current_scene.get())
			//{
			//	for(Scene::SceneManager::iterator iter = current_scene->begin(); iter != current_scene->end(); ++iter)
			//	{
			//		Scene::Entity &entity = **iter;
			//		presence_component = entity.GetComponent<EC_OpenSimPresence>().get();
			//		if (presence_component)
			//			if (presence_component->agentId.ToQString() == uuid->ToQString())
			//				other_voice = entity.GetComponent<EC_TtsVoice>();
			//	}
			//}

			//other_voice->ComponentChanged(AttributeChange::Network);

			//if(other_voice)
			//	avatar_voice_->SpeakMessage(msg,other_voice->GetMyVoice());
			//else
			//	avatar_voice_->SpeakMessage(msg);

			//tts_service_->text2Speech(msg, voice);
			//QString fileName1("\"./festival/audio\"");
			//QString fileName2("\"./festival/phonetic\"");
			//tts_service_->text2WAV(msg, fileName1,voice);
			//tts_service_->text2PHO(msg, fileName2,voice);
		}
	}

	void CommunicationWidget::GetAvatarVoiceComponent()
	{
		// Pick up the EC_TtsVoice of the avatar entity
		boost::shared_ptr<Foundation::WorldLogicInterface> world_logic = framework_->GetServiceManager()->GetService<Foundation::WorldLogicInterface>(Foundation::Service::ST_WorldLogic).lock();
			if(world_logic)
			{
				Scene::EntityPtr user_avatar = world_logic->GetUserAvatarEntity();
				if(user_avatar)
					avatar_voice_=user_avatar->GetComponent<EC_TtsVoice>();
			}
	}

	void CommunicationWidget::UpdateAvatarVoice(Tts::Voice voice)
	{
		if(!avatar_voice_)
				GetAvatarVoiceComponent();
		avatar_voice_->SetMyVoice(voice);
	}
	//
    void CommunicationWidget::hoverMoveEvent(QGraphicsSceneHoverEvent *mouse_hover_move_event)
    {
        if (stacked_layout_->currentWidget() == history_view_text_edit_)
        {
            qreal widget_press_pos_x = chatContentWidget->width() - mouse_hover_move_event->scenePos().x();
            qreal widget_press_pos_y = scene()->sceneRect().size().height() - mouse_hover_move_event->scenePos().y() - rect().height();
            if (widget_press_pos_x >= 0 && widget_press_pos_x <= 6)
            {
                if (!QApplication::overrideCursor())
                    QApplication::setOverrideCursor(QCursor(Qt::SizeHorCursor));
            }
            else if (widget_press_pos_y <= 0 && widget_press_pos_y >= -6 && widget_press_pos_x >= 0)
            {
                if (!QApplication::overrideCursor())
                    QApplication::setOverrideCursor(QCursor(Qt::SizeVerCursor));
            }
            else if (QApplication::overrideCursor())
                QApplication::restoreOverrideCursor();
        }
        else if (QApplication::overrideCursor())
            QApplication::restoreOverrideCursor();

        QGraphicsProxyWidget::hoverMoveEvent(mouse_hover_move_event);
    }

    void CommunicationWidget::hoverLeaveEvent(QGraphicsSceneHoverEvent *mouse_hover_leave_event)
    {
        if (QApplication::overrideCursor())
            QApplication::restoreOverrideCursor();
        QGraphicsProxyWidget::hoverLeaveEvent(mouse_hover_leave_event);
    }

    void CommunicationWidget::mousePressEvent(QGraphicsSceneMouseEvent *mouse_press_event)
    {   
        QWidget *w = widget()->childAt(mouse_press_event->pos().toPoint());
        if (w != chatContentWidget)
            scene()->clearFocus();

        resizing_horizontal_ = false;
        resizing_vertical_ = false;
        if (stacked_layout_->currentWidget() == history_view_text_edit_)
        {
            qreal widget_press_pos_x = chatContentWidget->width() - mouse_press_event->scenePos().x();
            qreal widget_press_pos_y = scene()->sceneRect().size().height() - mouse_press_event->scenePos().y() - rect().height();
            if (widget_press_pos_x >= 0 && widget_press_pos_x <= 6)
            {
                mouse_press_event->accept();
                QApplication::setOverrideCursor(QCursor(Qt::SizeHorCursor));
                resizing_horizontal_ = true;
                return;
            }
            else if (widget_press_pos_y <= 0 && widget_press_pos_y >= -6 && widget_press_pos_x >= 0)
            {
                mouse_press_event->accept();
                QApplication::setOverrideCursor(QCursor(Qt::SizeVerCursor));
                resizing_vertical_ = true;
                return;
            }
        }
        QGraphicsProxyWidget::mousePressEvent(mouse_press_event);
    }

    void CommunicationWidget::mouseMoveEvent(QGraphicsSceneMouseEvent *mouse_move_event)
    {
        if (resizing_horizontal_)
            chatContentWidget->setMinimumWidth(mouse_move_event->scenePos().x());
        else if (resizing_vertical_)
            chatContentWidget->setMinimumHeight(scene()->sceneRect().size().height() - mouse_move_event->scenePos().y() - chatControlsWidget->height());
        QGraphicsProxyWidget::mouseMoveEvent(mouse_move_event);
    }

    void CommunicationWidget::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouse_release_event)
    {
        if (resizing_horizontal_ || resizing_vertical_)
        {
            resizing_horizontal_ = false;
            resizing_vertical_ = false;
            QApplication::restoreOverrideCursor();
        }
        QGraphicsProxyWidget::mouseReleaseEvent(mouse_release_event);
    }

    void CommunicationWidget::UpdateImWidget(UiProxyWidget *im_proxy)
    {
        im_proxy_ = im_proxy;
        imContentWidget->show();
    }

    void CommunicationWidget::SetFocusToChat()
    {
        chatLineEdit->setFocus(Qt::MouseFocusReason);
    }

    void CommunicationWidget::InitializeInWorldChat()
    {
        if (framework_ &&  framework_->GetServiceManager())
        {
            Communications::ServiceInterface *comm = framework_->GetService<Communications::ServiceInterface>();
            if (comm)
            {
                if (in_world_chat_session_)
                {
                    disconnect(in_world_chat_session_);
                    in_world_chat_session_ = 0;
                    history_view_text_edit_->clear();
                }

				in_world_chat_session_ = comm->InWorldChatSession();
                if (!in_world_chat_session_)
                    return;

                connect(in_world_chat_session_, SIGNAL(TextMessageReceived(const Communications::InWorldChat::TextMessageInterface&,const QString&)),
                    SLOT(UpdateInWorldChatView(const Communications::InWorldChat::TextMessageInterface&,const QString&)) );
			
            }
        }
    }

    void CommunicationWidget::InitializeInWorldVoice()
    {
        if (framework_ &&  framework_->GetServiceManager())
        {
            Communications::ServiceInterface *comm = framework_->GetService<Communications::ServiceInterface>();
            if (comm)
            {
                in_world_voice_session_ = comm->InWorldVoiceSession();
                connect(in_world_voice_session_, SIGNAL(StartSendingAudio()), SLOT(UpdateInWorldVoiceIndicator()) );
                connect(in_world_voice_session_, SIGNAL(StopSendingAudio()), SLOT(UpdateInWorldVoiceIndicator()) );
                connect(in_world_voice_session_, SIGNAL(StateChanged(Communications::InWorldVoice::SessionInterface::State)), SLOT(UpdateInWorldVoiceIndicator()) );
                connect(in_world_voice_session_, SIGNAL(ParticipantJoined(Communications::InWorldVoice::ParticipantInterface*)), SLOT(UpdateInWorldVoiceIndicator()) );
                connect(in_world_voice_session_, SIGNAL(ParticipantJoined(Communications::InWorldVoice::ParticipantInterface*)), SLOT(ConnectParticipantVoiceAvticitySignals(Communications::InWorldVoice::ParticipantInterface*)) );
                connect(in_world_voice_session_, SIGNAL(ParticipantLeft(Communications::InWorldVoice::ParticipantInterface*)), SLOT(UpdateInWorldVoiceIndicator()) );
                connect(in_world_voice_session_, SIGNAL(destroyed()), SLOT(UninitializeInWorldVoice()));
                connect(in_world_voice_session_, SIGNAL(SpeakerVoiceActivityChanged(double)), SLOT(UpdateInWorldVoiceIndicator()));
                connect(in_world_voice_session_, SIGNAL(StateChanged(Communications::InWorldVoice::SessionInterface::State)), SLOT(UpdateInWorldVoiceIndicator()));
            }
        }

        if (voice_state_widget_)
        {
            this->voiceLayoutH->removeWidget(voice_state_widget_);
            SAFE_DELETE(voice_state_widget_);
        }
        voice_state_widget_ = new CommUI::VoiceStateWidget(0);
        connect(voice_state_widget_, SIGNAL( clicked() ), SLOT(ToggleVoice() ) );
        this->voiceLayoutH->addWidget(voice_state_widget_);
        voice_state_widget_->show();

        if (voice_users_info_widget_)
        {
            this->voiceLayoutH->removeWidget(voice_users_info_widget_);
            SAFE_DELETE(voice_users_info_widget_);
        }
        voice_users_info_widget_ = new CommUI::VoiceUsersInfoWidget(0);
        connect(voice_users_info_widget_, SIGNAL( clicked() ), SLOT(ToggleVoice() ) );
        this->voiceLayoutH->addWidget(voice_users_info_widget_);
        voice_users_info_widget_->show();

        UpdateInWorldVoiceIndicator();

        Foundation::UiServiceInterface* ui_service = framework_->GetService<Foundation::UiServiceInterface>();
        if (ui_service)
        {
            if (voice_controller_widget_)
                SAFE_DELETE(voice_controller_widget_);

            voice_controller_widget_ = new CommUI::VoiceControllerWidget(in_world_voice_session_);

            voice_controller_proxy_widget_ = ui_service->AddWidgetToScene(voice_controller_widget_);
            voice_controller_proxy_widget_->setWindowTitle("In-world voice");
            voice_controller_proxy_widget_->hide();

            if (framework_)
            {
                input_context_ = framework_->Input().RegisterInputContext("CommunicationWidget", 90);
                connect(input_context_.get(), SIGNAL(MouseMiddlePressed(MouseEvent*)), voice_controller_widget_, SLOT(SetPushToTalkOn()));
                connect(input_context_.get(), SIGNAL(MouseMiddleReleased(MouseEvent*)),voice_controller_widget_, SLOT(SetPushToTalkOff()));
                connect(input_context_.get(), SIGNAL(MouseMiddlePressed(MouseEvent*)), voice_controller_widget_, SLOT(Toggle()));
            }
        }
    }
	void CommunicationWidget::InitializeInWorldTts()
	{
		tts_service_ = framework_->GetService<Tts::TtsServiceInterface>();
		if (!tts_service_)
			return;
		connect(in_world_chat_session_, SIGNAL(TextMessageReceived(const Communications::InWorldChat::TextMessageInterface&,const QString&)), SLOT(SpeakIncomingMessage(const Communications::InWorldChat::TextMessageInterface&,const QString&)) );
		
		// Pick up the EC_TtsVoice of the avatar entity
		GetAvatarVoiceComponent();

		//Inicialización de la ventana gráfica del Tts
		ShowTtsChatControls();

		if (Tts_chat_widget)
			SAFE_DELETE(Tts_chat_widget);
		
		Tts_chat_widget = new Communications::TtsChat::TtsChatWidget(); 
		
		tts_config_ = new Communications::TtsChat::TtsChatConfig();
		Tts_chat_widget->ConfigureInterface(tts_config_);
		connect(ttsButton, SIGNAL(clicked()), SLOT(ToggleTtsChatWidget()));
		connect(Tts_chat_widget, SIGNAL(TtsstateChanged()),SLOT(UpdateTtsChatControls()));
		connect(Tts_chat_widget,SIGNAL(TtsVoiceChanged(Tts::Voice)),SLOT(UpdateAvatarVoice(Tts::Voice)));

		Foundation::UiServiceInterface *ui = framework_->GetService<Foundation::UiServiceInterface>();
		if (ui)
		{
			tts_proxy_ = ui->AddWidgetToScene(Tts_chat_widget);
			tts_proxy_->AnimatedHide();
		}
	}
    void CommunicationWidget::ConnectParticipantVoiceAvticitySignals(Communications::InWorldVoice::ParticipantInterface* p)
    {
        /// @todo Move all voice control widget related code to separate class
        ///       and possible add to UiServiceInterdace methods to conrol tool widgets.
        connect(p, SIGNAL(StartSpeaking()), this, SLOT(UpdateInWorldVoiceIndicator()));
        connect(p, SIGNAL(StopSpeaking()), this, SLOT(UpdateInWorldVoiceIndicator()));
    }
    void CommunicationWidget::UpdateInWorldVoiceIndicator()
    {
        if (!in_world_voice_session_)
            return;

        if (in_world_voice_session_->GetState() != Communications::InWorldVoice::SessionInterface::STATE_OPEN)
        {
            HideVoiceControls();
            return;
        }
        ShowVoiceControls();

        if (in_world_voice_session_->IsAudioSendingEnabled())
        {
            if (voice_state_widget_)
            {
                voice_state_widget_->setState(CommUI::VoiceStateWidget::STATE_ONLINE);
                voice_state_widget_->SetVoiceActivity(in_world_voice_session_->SpeakerVoiceActivity());
            }
        }
        else
        {
            if (voice_state_widget_)
                voice_state_widget_->setState(CommUI::VoiceStateWidget::STATE_OFFLINE);
        }

        if (voice_users_info_widget_)
        {
            double channel_voice_activity = 0;
            QList<Communications::InWorldVoice::ParticipantInterface*> list = in_world_voice_session_->Participants();
            foreach(Communications::InWorldVoice::ParticipantInterface* p, list)
            {
                if (p->IsSpeaking())
                {
                    channel_voice_activity = 1;
                    break;
                }
            }
            voice_users_info_widget_->SetVoiceActivity(channel_voice_activity);
            voice_users_info_widget_->SetUsersCount(in_world_voice_session_->Participants().count());
        }
    }

    void CommunicationWidget::UpdateInWorldChatView(const Communications::InWorldChat::TextMessageInterface &message,const QString& uuid)
    {
        QString hour_str = QString::number(message.TimeStamp().time().hour());
        QString minute_str = QString::number(message.TimeStamp().time().minute());
        QString time_stamp_str = QString("%1:%2").arg(hour_str, 2, QChar('0')).arg(minute_str, 2, QChar('0'));
      
		if(!message.Text().contains("</voice>", Qt::CaseInsensitive))
			ShowIncomingMessage(message.IsOwnMessage(), message.Author(), time_stamp_str, message.Text());
		else
		{
			// Deletes voice info
			QString	chatText_=message.Text();
			const QString labelClose = "</voice>";
			const int labelCloseSize = labelClose.size();
			const int closePosition = chatText_.lastIndexOf(labelClose);
			chatText_ = chatText_.remove(0, closePosition + labelCloseSize);
			
			ShowIncomingMessage(message.IsOwnMessage(), message.Author(), time_stamp_str, chatText_);
		}
    }

    void CommunicationWidget::UninitializeInWorldVoice()
    {
        //! \todo: set indicator status -> disabled (hide voice controls)
        in_world_voice_session_ = 0;
    }
	

	void CommunicationWidget::UpdateTtsChatControls()
    {
		ownVoiceOn =tts_config_->isActiveOwnVoice();
		othersVoiceOn =tts_config_->isActiveOthersVoice();;
		if (ownVoiceOn || othersVoiceOn)
		{
			this->ttsButton->setStyleSheet("QPushButton#ttsButton {border: 0px;background-color: transparent;background-image: url('./data/ui/images/chat/uibutton_TTS_semi.png');background-position: top left;background-repeat: no-repeat;} QPushButton#ttsButton::hover {border: 0px;background-image: url('./data/ui/images/chat/uibutton_TTS_hover.png');} QPushButton#ttsButton::pressed {border: 0px; background-image: url('./data/ui/images/chat/uibutton_TTS_click.png');}");
		}

		if (ownVoiceOn && othersVoiceOn)
		{
			this->ttsButton->setStyleSheet("QPushButton#ttsButton {border: 0px;background-color: transparent;background-image: url('./data/ui/images/chat/uibutton_TTS_total.png');background-position: top left;background-repeat: no-repeat;} QPushButton#ttsButton::hover {border: 0px;background-image: url('./data/ui/images/chat/uibutton_TTS_hover.png');} QPushButton#ttsButton::pressed {border: 0px; background-image: url('./data/ui/images/chat/uibutton_TTS_click.png');}");
		}
		if (!(ownVoiceOn || othersVoiceOn))
		{
			this->ttsButton->setStyleSheet("QPushButton#ttsButton {border: 0px;background-color: transparent;background-image: url('./data/ui/images/chat/uibutton_TTS_normal.png');background-position: top left;background-repeat: no-repeat;} QPushButton#ttsButton::hover {border: 0px;background-image: url('./data/ui/images/chat/uibutton_TTS_hover.png');} QPushButton#ttsButton::pressed {border: 0px; background-image: url('./data/ui/images/chat/uibutton_TTS_click.png');}");
		}
    }

    // NormalChatViewWidget : QWidget

    NormalChatViewWidget::NormalChatViewWidget(QWidget *parent) :
        QWidget(parent)
    {
        setObjectName("normalChatViewWidget");
        setStyleSheet("QWidget#normalChatViewWidget { background-color: transparent; }");

        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->setMargin(0);
        layout->setSpacing(3);
        layout->addSpacerItem(new QSpacerItem(1,1, QSizePolicy::Fixed, QSizePolicy::Expanding));
        setLayout(layout);
    }

    void NormalChatViewWidget::ShowChatMessage(bool own_message, QString message)
    {
        ChatLabel *chat_label = new ChatLabel(own_message, message);
        layout()->addWidget(chat_label);
        connect(chat_label, SIGNAL( DestroyMe(ChatLabel*) ), SLOT( RemoveChatLabel(ChatLabel*) ));
    }

    void NormalChatViewWidget::RemoveChatLabel(ChatLabel *label)
    {
        int index = layout()->indexOf(label);
        if (index != -1)
        {
            layout()->removeItem(layout()->itemAt(index));
            SAFE_DELETE(label);
            if (layout()->count() < 4)
                updateGeometry();
        }
    }

    // ChatLabel

    ChatLabel::ChatLabel(bool own_message, QString message) :
        QLabel(message)
    {
        setFont(QFont("Arial", 12));
        if (own_message)
            setStyleSheet("background-color: rgba(34,34,34,191); color: white; border-radius: 5px; padding: 3px;");
        else
            setStyleSheet("background-color: rgba(34,34,34,150); color: white; border-radius: 5px; padding: 3px;");
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        QTimer::singleShot(10000, this, SLOT(TimeOut()));
    }

    void ChatLabel::TimeOut()
    {
        emit DestroyMe(this);
    }
}

