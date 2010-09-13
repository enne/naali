/********************************************************************
/*********                  TTSChatWidget                ************
/*																    *
/*  This QWidget is used to configure InWorldChat TTSChat translation   *
/*																    *
/*		It's necessary to use Communications:TTSChat namespace.			*										
/*		The class is TTSChatWidget (QWidget)							*
/********************************************************************/


#ifndef TTSCHATWIDGET_H
#define TTSCHATWIDGET_H

#include <QtGui/QMainWindow>
#include "ui_TTSChatWidget.h"
#include "TTSServiceInterface.h"

#include <phonon>


namespace Communications //added
{
	namespace TTSChat //added
	{
		class TTSChatConfig
		{
		public:
			TTSChatConfig();
			virtual const TTS::Voice getOwnVoice();
			virtual void setOwnVoice(TTS::Voice voice);
			// Set the voice of the others
			virtual void setActiveOwnVoice(bool active);
			// return true if the own voce is active
			virtual bool isActiveOwnVoice();
			// Active the others voice
			virtual void setActiveOthersVoice(bool active);
			// return true if the others voce is active
			virtual bool isActiveOthersVoice();	

		private:
			TTS::Voice OwnVoice_;
			bool activeOwnVoice_,activeOthersVoice_;
		};

		class TTSChatWidget : public QWidget
		{
			Q_OBJECT //macro

		public:
			TTSChatWidget(QWidget *parent = 0, Qt::WFlags flags = 0); //constructor
			~TTSChatWidget(); //destructor
			


		public slots:
				//Initializes enable values (ownTTSVoiceStatus,othersTTSVoiceStatus) and voices (ownTTSVoice,othersTTSVoice)
				//Configures and connects SIGNALS with SLOTS
				void ConfigureInterface(Communications::TTSChat::TTSChatConfig* tts_config);
				void reloadItems();
		
		private:

			Ui::TTSChatWidgetClass ui;

			//Variables to drag and move TTSChatWidget QWindow
			QPoint mouse_last_pos_;
			bool mouse_dragging_;
			Communications::TTSChat::TTSChatConfig* tts_config_;
			Phonon::MediaObject *media_object_;
			Phonon::AudioOutput *audio_output_;

			QString fileName;

		 signals:
				void TTSstateChanged();
				void TTSVoiceChanged(TTS::Voice voice);

		protected:
			//Methods to move the window
			virtual void mouseMoveEvent(QMouseEvent *);
			virtual void mousePressEvent(QMouseEvent *);
			virtual void mouseReleaseEvent(QMouseEvent *);

		private slots:
			//This takes the information from the ComboBoxes and saves into the variables
			// After that, stored information can be obtained with the get channels above.
			void saveChanges();
		    void demoButtonPressed();
			void sendTTSStateChanged();

		};
	}//End TTSChat namespace

} //End Communications namespace

namespace Phonon
{
    class MediaObject;
	class AudioOutput;
}

#endif // TTSCHATWIDGET_H

