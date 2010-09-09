// For conditions of distribution and use, see copyright notice in license.txt

#include "StableHeaders.h"
#include "DebugOperatorNew.h"

#include "MessageNotification.h"

#include <QPlainTextEdit>

#include "MemoryLeakCheck.h"

//H7
#include "TTSServiceInterface.h"

namespace UiServices
{
    MessageNotification::MessageNotification(const QString &message, int hide_in_msec) :
        CoreUi::NotificationBaseWidget(hide_in_msec, message)
    {
        // Init Ui
        QPlainTextEdit *message_box = new QPlainTextEdit(message);
        message_box->setReadOnly(true);
        message_box->setFrameShape(QFrame::NoFrame);
        message_box->setStyleSheet("background-color: transparent;");

        QFontMetrics metric(message_box->font());
        QRect text_rect = metric.boundingRect(QRect(0,0,160,400), Qt::AlignLeft|Qt::TextWordWrap, message);
        message_box->setMaximumHeight(text_rect.height() + metric.height());
        message_box->setMinimumHeight(text_rect.height() + metric.height());

        // Layout
        QWidget *content_widget = new QWidget();
        QVBoxLayout *v_layout = new QVBoxLayout();
        v_layout->addWidget(message_box);

        content_widget->setLayout(v_layout);
        SetCentralWidget(content_widget);
		/*
		//H7
		//TTS the message
		Foundation::Framework *framework_ = GetFramework();
		//Foundation::Framework *framework_ = Foundation::ModuleInterface::GetFramework();
		TTS::TTSServiceInterface* tts_service_ = framework_->GetService<TTS::TTSServiceInterface>();
		if (tts_service_)
		{
			tts_service_->setVoice(TTS::Voices.ES1);
			tts_service_->text2Speech(message);
		}
		*/
    }
}