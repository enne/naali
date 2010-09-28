// For conditions of distribution and use, see copyright notice in license.txt

#ifndef incl_UiModule_CommunicationWidget_h
#define incl_UiModule_CommunicationWidget_h

#include <QGraphicsProxyWidget>
#include "ui_CommunicationWidget.h"
#include "InputServiceInterface.h"


#include "TtsChatWidget.h"
#include "EC_TtsVoice.h"



class QStackedLayout;
class QTextBrowser;
class QGraphicsSceneMouseEvent;

class UiProxyWidget;

namespace Communications
{
    namespace InWorldVoice
    {
        class SessionInterface;
        class ParticipantInterface;
    }

    namespace InWorldChat
    {
        class SessionInterface;
        class TextMessageInterface;
    }

	namespace TtsChat
	{
		class TtsChatWidget;
		class TtsChatConfig;
	}

}

namespace CommUI
{
    class VoiceUsersInfoWidget;
    class VoiceStateWidget;
    class VoiceControllerWidget;
}

namespace Foundation
{
    class Framework;
}

namespace CoreUi
{
    class NormalChatViewWidget;
    class ChatLabel;

    //! Provide communications functionalities to end user
    //! CommunicationWidget is located to bottom left corner view.
    class CommunicationWidget : public QGraphicsProxyWidget, private Ui::CommunicationWidget
    {
        Q_OBJECT

    public:
        CommunicationWidget(Foundation::Framework* framework);
        enum ViewMode { Normal, History };

    public slots:
        void UpdateImWidget(UiProxyWidget *im_proxy);
        void SetFocusToChat();
        
    protected:
        void hoverMoveEvent(QGraphicsSceneHoverEvent *mouse_hover_move_event);
        void hoverLeaveEvent(QGraphicsSceneHoverEvent *mouse_hover_leave_event);
        void mousePressEvent(QGraphicsSceneMouseEvent *mouse_press_event);
        void mouseMoveEvent(QGraphicsSceneMouseEvent *mouse_move_event);
        void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouse_release_event);

    private slots:
        void Initialise();
        void ChangeViewPressed();
        void ChangeView(ViewMode new_mode);
        void ToggleImWidget();
        void ToggleVoice();

        void ShowIncomingMessage(bool self_sent_message, QString sender, QString timestamp, QString message);
		/// Takes the message from chat line and adds voice information if TTS available
        void SendMessageRequested();
        void InitializeInWorldVoice();
        void InitializeInWorldChat();
        void UninitializeInWorldVoice();
        void UpdateInWorldVoiceIndicator();
        void ShowVoiceControls();
        void HideVoiceControls();
		/// Updates chat view removing voice info first
        void UpdateInWorldChatView(const Communications::InWorldChat::TextMessageInterface &message, const QString& uuid);
        void ConnectParticipantVoiceAvticitySignals(Communications::InWorldVoice::ParticipantInterface* p);

		/// Shows or hides TTSCHatWidget configuration window
		void ToggleTtsChatWidget();
		/// Shows TTS Button in the communication widget
		void ShowTtsChatControls();
		/// Hides TTS Button in the communication widget
        void HideTtsChatControls();
	    /// Update TTS Button color to yellow or green changing style sheet
		void UpdateTtsChatControls();
		/// Initializes all TTS system showing buttons, creating widgets and with default configuration
		void InitializeInWorldTts();
		/** Message parser (voice+msg) calls TTS Service
		\param message Message directly from network with time stamp and so on.
		\param from_uuid
		\todo Redefine with components. */
		void SpeakIncomingMessage(const Communications::InWorldChat::TextMessageInterface &message, const QString& from_uuid);
		void GetAvatarVoiceComponent();
		void UpdateAvatarVoice(Tts::Voice voice);
	


    private:
        Foundation::Framework* framework_;
        ViewMode viewmode_;

        QWidget *internal_widget_;
        QStackedLayout *stacked_layout_;
        QTextBrowser *history_view_text_edit_;
        NormalChatViewWidget *normal_view_widget_;
        UiProxyWidget *im_proxy_,*tts_proxy_;
        Communications::InWorldVoice::SessionInterface* in_world_voice_session_;
        Communications::InWorldChat::SessionInterface* in_world_chat_session_;


        QPointF press_pos_;
        QPointF release_pos_;
        bool resizing_vertical_;
        bool resizing_horizontal_;

        // in-world voice
        CommUI::VoiceStateWidget* voice_state_widget_;
        CommUI::VoiceUsersInfoWidget* voice_users_info_widget_;
        CommUI::VoiceControllerWidget* voice_controller_widget_;
        UiProxyWidget* voice_users_proxy_widget_;
        UiProxyWidget* voice_controller_proxy_widget_;

        InputContextPtr input_context_;


		Communications::TtsChat::TtsChatWidget* Tts_chat_widget;
		Tts::TtsServiceInterface* tts_service_;
		Communications::TtsChat::TtsChatConfig* tts_config_;
		bool ownVoiceOn,othersVoiceOn;
		boost::shared_ptr<EC_TtsVoice> avatar_voice_;

    signals:
        void SendMessageToServer(const QString &message);
    };

    class NormalChatViewWidget : public QWidget
    {
        Q_OBJECT

    public:
        NormalChatViewWidget(QWidget *parent);

    public slots:
        void ShowChatMessage(bool own_message, QString message);

    private slots:
        void RemoveChatLabel(ChatLabel *label);
    };

    class ChatLabel : public QLabel
    {
        Q_OBJECT

    public:
        ChatLabel(bool own_message, QString message);

    private slots:
        void TimeOut();

    signals:
        void DestroyMe(ChatLabel *);
    };
}

#endif // incl_UiModule_CommunicationWidget_h
