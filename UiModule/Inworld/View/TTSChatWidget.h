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


namespace Communications //added
{
	namespace TTSChat //added
	{

		class TTSChatWidget : public QWidget
		{
			Q_OBJECT //macro

		public:
			TTSChatWidget(QWidget *parent = 0, Qt::WFlags flags = 0); //constructor
			~TTSChatWidget(); //destructor
			


		public slots:
				//Initializes enable values (ownTTSVoiceStatus,othersTTSVoiceStatus) and voices (ownTTSVoice,othersTTSVoice)
				//Configures and connects SIGNALS with SLOTS
				void ConfigureInterface(TTS::TTSServiceInterface* tts_service); 
		
		private:

			Ui::TTSChatWidgetClass ui;

			//Variables to drag and move TTSChatWidget QWindow
			QPoint mouse_last_pos_;
			bool mouse_dragging_;
			TTS::TTSServiceInterface* tts_service_;
	

		protected:
			//Methods to move the window
			virtual void mouseMoveEvent(QMouseEvent *);
			virtual void mousePressEvent(QMouseEvent *);
			virtual void mouseReleaseEvent(QMouseEvent *);

		private slots:
			//This takes the information from the ComboBoxes and saves into the variables
			// After that, stored information can be obtained with the get channels above.
			void SaveButtonPressed();
		    

		};
	}//End TTSChat namespace

} //End Communications namespace
#endif // TTSCHATWIDGET_H

