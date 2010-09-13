#include "StableHeaders.h"
#include "TTSChatWidget.h"

#include <Phonon/MediaObject>
#include <Phonon/MediaSource>

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
			
			//load items
			reloadItems();
			saveChanges();

			//Hide Button, if pressed, hide QWidget
		   QObject::connect(ui.closeButton, SIGNAL(clicked(bool)), SLOT(hide()));

		   //Save
		   QObject::connect(ui.ownGendComboBox, SIGNAL(currentIndexChanged(int)), SLOT(saveChanges()));
		   QObject::connect(ui.ownEnableCheckBox, SIGNAL(stateChanged(int)),SLOT(saveChanges()));
		   QObject::connect(ui.othersEnableCheckBox, SIGNAL(stateChanged(int)),SLOT(saveChanges()));
		   
		   QObject::connect(ui.ownEnableCheckBox, SIGNAL(stateChanged(int)),SLOT(sendTTSStateChanged()));
		   QObject::connect(ui.othersEnableCheckBox, SIGNAL(stateChanged(int)),SLOT(sendTTSStateChanged()));
			
		    
			//Load itemes depending on language
		   QObject::connect(ui.ownLangComboBox, SIGNAL(currentIndexChanged(int)), SLOT(reloadItems()));

		   //Demo button.
		   QObject::connect(ui.demoButton, SIGNAL(clicked(bool)), SLOT(demoButtonPressed()));

		 } //End widget configuration.


		//Save button is pressed, so information must be stored
		void TTSChatWidget::saveChanges()
		{
			QString currentLanguage,currentGender;
     		currentLanguage=ui.ownLangComboBox->currentText();
			currentGender=ui.ownGendComboBox->currentText();

			if (currentLanguage=="Spanish")
			{
				if (currentGender=="Male")
				{
					tts_config_->setOwnVoice(TTS::Voices.ES1);
					fileName ="./festival/demo/DemoES1.wav";
				}
				if (currentGender=="Female")
				{
					tts_config_->setOwnVoice(TTS::Voices.ES2);
					fileName ="./festival/demo/DemoES2.wav";
				}
			}
			if (currentLanguage=="English")
			{
				if (currentGender=="Male 1")
				{
					tts_config_->setOwnVoice(TTS::Voices.EN1);
					fileName ="./festival/demo/DemoEN1.wav";
				}
				if (currentGender=="Female 1")
				{
					tts_config_->setOwnVoice(TTS::Voices.EN2);
					fileName ="./festival/demo/DemoEN2.wav";
				}
				if (currentGender=="Male 2")
				{
					tts_config_->setOwnVoice(TTS::Voices.EN3);
					fileName ="./festival/demo/DemoEN3.wav";
				}
				if (currentGender=="Female 2")
				{
					tts_config_->setOwnVoice(TTS::Voices.EN4);
					fileName ="./festival/demo/DemoEN4.wav";
				}
				if (currentGender=="Male 3")
				{
					tts_config_->setOwnVoice(TTS::Voices.EN5);
					fileName ="./festival/demo/DemoEN5.wav";
				}
				if (currentGender=="Male 4")
				{
					tts_config_->setOwnVoice(TTS::Voices.EN6);
					fileName ="./festival/demo/DemoEN6.wav";
				}
			}
			if (currentLanguage=="Catalan")
			{
				if (currentGender=="Male")
				{
					tts_config_->setOwnVoice(TTS::Voices.CAT1);
					fileName ="./festival/demo/DemoCAT1.wav";
				}
				if (currentGender=="Female")
				{
					tts_config_->setOwnVoice(TTS::Voices.CAT2);
					fileName ="./festival/demo/DemoCAT2.wav";
				}
			}			
			if (currentLanguage=="Finnish")
			{
				if (currentGender=="Male")
				{
					tts_config_->setOwnVoice(TTS::Voices.FI);
					fileName ="./festival/demo/DemoFI.wav";
				}
			}	
			
			
			if(ui.ownEnableCheckBox->isChecked())
				tts_config_->setActiveOwnVoice(true);
			else
				tts_config_->setActiveOwnVoice(false);
							
			//If others voice enabled, save values from combobox
			//This is exactly the same but with others.
			if(ui.othersEnableCheckBox->isChecked())
				tts_config_->setActiveOthersVoice(true);
			else
				tts_config_->setActiveOthersVoice(false);

			emit TTSVoiceChanged(tts_config_->getOwnVoice());
		}
		void TTSChatWidget::reloadItems()
		{
			QString currentLanguage;
			currentLanguage=ui.ownLangComboBox->currentText();
			
			//Be carefull, if item text is translated
			//Do it with index
			if (currentLanguage=="Spanish")
			{
				ui.ownGendComboBox->clear();
				ui.ownGendComboBox->addItem("Male");
				ui.ownGendComboBox->addItem("Female");
			}
			if (currentLanguage=="English")
			{
				ui.ownGendComboBox->clear();
				ui.ownGendComboBox->addItem("Male 1");
				ui.ownGendComboBox->addItem("Female 1");
				ui.ownGendComboBox->addItem("Male 2");
				ui.ownGendComboBox->addItem("Female 2");
				ui.ownGendComboBox->addItem("Male 3");
				ui.ownGendComboBox->addItem("Male 4");
			}
			if (currentLanguage=="Finnish")
			{
				ui.ownGendComboBox->clear();
				ui.ownGendComboBox->addItem("Male");
			}
			if (currentLanguage=="Catalan")
			{
				ui.ownGendComboBox->clear();
				ui.ownGendComboBox->addItem("Male");
				ui.ownGendComboBox->addItem("Female");
			}
		}

		void TTSChatWidget::demoButtonPressed()
		{
			media_object_ = new Phonon::MediaObject(this);
			audio_output_ = new Phonon::AudioOutput(Phonon::MusicCategory, this);
			
		
			media_object_->setCurrentSource(fileName);
			
			Phonon::createPath(media_object_, audio_output_);

			media_object_->play();

		}
		void TTSChatWidget::sendTTSStateChanged()
		{
			emit TTSstateChanged();
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