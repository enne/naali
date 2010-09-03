#include "StableHeaders.h"
#include "TTSChatWidget.h"



namespace Communications //Needed
{
	namespace TTSChat
	{
		TTSChatWidget::TTSChatWidget(QWidget *parent, Qt::WFlags flags)	: QWidget(parent, flags)
		{
			ui.setupUi(this);
			
		}

		TTSChatWidget::~TTSChatWidget()
		{

		}

		void TTSChatWidget::ConfigureInterface(Communications::TTSChat::TTSChatConfig* tts_config)
		{
			tts_config_=tts_config;
			//Configuration, if the chechbox is not enabled, combobox are disabled
			//by default, all disabled.
		   /*ui.ownLangComboBox->setEnabled(false);
		   ui.ownGendComboBox->setEnabled(false);*/


			//if TTSChat enabled, voice can be selected, so ComboBoxes are activated
		   //QObject::connect(ui.ownEnableCheckBox, SIGNAL(clicked(bool)), ui.ownLangComboBox, SLOT(setEnabled(bool)));
		   //QObject::connect(ui.ownEnableCheckBox, SIGNAL(clicked(bool)), ui.ownGendComboBox, SLOT(setEnabled(bool)));

		  // QObject::connect(ui.othersEnableCheckBox, SIGNAL(clicked(bool)), ui.othersLangComboBox, SLOT(setEnabled(bool)));
		   //QObject::connect(ui.othersEnableCheckBox, SIGNAL(clicked(bool)), ui.othersGendComboBox, SLOT(setEnabled(bool)));

			//Hide Button, if pressed, hide QWidget
		   QObject::connect(ui.saveButton, SIGNAL(clicked(bool)), SLOT(hide()));

		   //Save button.
		   QObject::connect(ui.saveButton, SIGNAL(clicked(bool)), SLOT(SaveButtonPressed()));

		 } //End widget configuration.



		//Save button is pressed, so information must be stored
		void TTSChatWidget::SaveButtonPressed()
		{
			//If own voice enabled, save values from combobox
			if(ui.ownEnableCheckBox->isChecked())
			{
				tts_config_->setActiveOwnVoice(true);
				//If index is 0, Selected language is ES
				if(!ui.ownLangComboBox->currentIndex())
				{
					//If index is 0, selected gender is Male
					if(!ui.ownGendComboBox->currentIndex())
						tts_config_->setOwnVoice(TTS::Voices.ES1);
					//Else Female
					else
						tts_config_->setOwnVoice(TTS::Voices.ES2);
				}
				//else EN
				else
				{	
					//Male
					if(!ui.ownGendComboBox->currentIndex())
						tts_config_->setOwnVoice(TTS::Voices.EN1);
					//Female
					else
						tts_config_->setOwnVoice(TTS::Voices.EN2);
				}	
			}
			else
			{
				tts_config_->setActiveOwnVoice(false);
			}
					
			//If others voice enabled, save values from combobox
			//This is exactly the same but with others.
			if(ui.othersEnableCheckBox->isChecked())
			{
				tts_config_->setActiveOthersVoice(true);
			}
			else
			{
				tts_config_->setActiveOthersVoice(false);
			}	
			
		}
		
		//Move window
		void TTSChatWidget::mouseMoveEvent(QMouseEvent *e)
		{
			QPoint pos = e->globalPos();
			QPoint move = pos - mouse_last_pos_;
			this->move(this->pos() +  move);
			mouse_last_pos_ = pos;
			QWidget::mouseMoveEvent(e);
		}

		void TTSChatWidget::mousePressEvent(QMouseEvent *e)
		{
			mouse_last_pos_ = e->globalPos();
			mouse_dragging_ = true;
		}

		void TTSChatWidget::mouseReleaseEvent(QMouseEvent *e)
		{
			mouse_dragging_ = false;
		}

		
		
		
		TTSChatConfig::TTSChatConfig():
			OwnVoice_(TTS::Voices.ES1),
			activeOwnVoice_(0),
			activeOthersVoice_(0)
		{
		}

		const TTS::Voice TTSChatConfig::getOwnVoice()
		{
			return OwnVoice_;
		}

		void TTSChatConfig::setOwnVoice(TTS::Voice voice)
		{
			OwnVoice_=voice;
		}

		void TTSChatConfig::setActiveOwnVoice(bool active)
		{
			activeOwnVoice_=active;
		}

		bool TTSChatConfig::isActiveOwnVoice()
		{
			return activeOwnVoice_;
		}
		void TTSChatConfig::setActiveOthersVoice(bool active)
		{
			activeOthersVoice_=active;
		}

		bool TTSChatConfig::isActiveOthersVoice()
		{
			return activeOthersVoice_;
		}
	
	}

} //End Namespace COmmunications