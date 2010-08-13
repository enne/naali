#include "StableHeaders.h"
#include "TTSChatWidget.h"



namespace Communications //Needed
{

	TTSChat::TTSChatWidget::TTSChatWidget(QWidget *parent, Qt::WFlags flags)	: QWidget(parent, flags)
	{
		ui.setupUi(this);
		
	}

	TTSChat::TTSChatWidget::~TTSChatWidget()
	{

	}

	void TTSChat::TTSChatWidget::ConfigureInterface()
	{

		//Initializations
		ownTTSChatVoice=ES1;
		othersTTSChatVoice=ES2;
		ownTTSChatStatus=FALSE;
		othersTTSChatStatus=FALSE;



		//Configuration, if the chechbox is not enabled, combobox are disabled
		//by default, all disabled.
	   ui.ownLangComboBox->setEnabled(false);
	   ui.ownGendComboBox->setEnabled(false);
	   ui.othersLangComboBox->setEnabled(false);
	   ui.othersGendComboBox->setEnabled(false);

		//if TTSChat enabled, voice can be selected, so ComboBoxes are activated
	   QObject::connect(ui.ownEnableCheckBox, SIGNAL(clicked(bool)), ui.ownLangComboBox, SLOT(setEnabled(bool)));
	   QObject::connect(ui.ownEnableCheckBox, SIGNAL(clicked(bool)), ui.ownGendComboBox, SLOT(setEnabled(bool)));

	   QObject::connect(ui.othersEnableCheckBox, SIGNAL(clicked(bool)), ui.othersLangComboBox, SLOT(setEnabled(bool)));
	   QObject::connect(ui.othersEnableCheckBox, SIGNAL(clicked(bool)), ui.othersGendComboBox, SLOT(setEnabled(bool)));

		//Hide Button, if pressed, hide QWidget
	   QObject::connect(ui.hideButton, SIGNAL(clicked(bool)), SLOT(hide()));

	   //Save button.
	   QObject::connect(ui.saveButton, SIGNAL(clicked(bool)), SLOT(SaveButtonPressed()));

	 } //End widget configuration.



	//Save button is pressed, so information must be stored
	void TTSChat::TTSChatWidget::SaveButtonPressed()
	{
		//If own voice enabled, save values from combobox
		if(ui.ownEnableCheckBox->isChecked())
		{
			//If index is 0, Selected language is ES
			if(!ui.ownLangComboBox->currentIndex())
			{
				//If index is 0, selected gender is Male
				if(!ui.ownGendComboBox->currentIndex())
					ownTTSChatVoice=ES1;
				//Else Female
				else
					ownTTSChatVoice=ES2;
			}
			//else EN
			else
			{	
				//Male
				if(!ui.ownGendComboBox->currentIndex())
					ownTTSChatVoice=EN1;
				//Female
				else
					ownTTSChatVoice=EN2;
			}
			//TODO: It should EMIT a signal to TTSModule, to update configuration.
			
		}
		
		
		//If others voice enabled, save values from combobox
		//This is exactly the same but with others.
		if(ui.othersEnableCheckBox->isChecked())
		{
			//
			if(!ui.othersLangComboBox->currentIndex())
			{
				if(!ui.othersGendComboBox->currentIndex())
					othersTTSChatVoice=ES1;
				else
					othersTTSChatVoice=ES2;
			}
			else
			{
				if(!ui.othersGendComboBox->currentIndex())
					othersTTSChatVoice=EN1;
				else
					othersTTSChatVoice=EN2;
			}
		}
		
		
	}
	
	//Returns if ownTTS is enabled or not
	bool TTSChat::TTSChatWidget::ownTTSChatEnabled()
	{
		 return ownTTSChatStatus;   
	}
	//Returns if othersTTS is enabled or not
	bool TTSChat::TTSChatWidget::othersTTSChatEnabled()
	{
		   return othersTTSChatStatus; 
	}
	//Returns own voice
	//it should be called if own TTS is enabled
	int TTSChat::TTSChatWidget::getOwnTTSChatVoice()
	{
			return ownTTSChatVoice;
	}
	//Returns others voice
	//it should be called if others TTS is enabled
	int TTSChat::TTSChatWidget::getOthersTTSChatVoice()
	{
			return othersTTSChatVoice;
	}

	//Move window
	void TTSChat::TTSChatWidget::mouseMoveEvent(QMouseEvent *e)
    {
        QPoint pos = e->globalPos();
        QPoint move = pos - mouse_last_pos_;
        this->move(this->pos() +  move);
        mouse_last_pos_ = pos;
        QWidget::mouseMoveEvent(e);
    }

    void TTSChat::TTSChatWidget::mousePressEvent(QMouseEvent *e)
    {
        mouse_last_pos_ = e->globalPos();
        mouse_dragging_ = true;
    }

    void TTSChat::TTSChatWidget::mouseReleaseEvent(QMouseEvent *e)
    {
        mouse_dragging_ = false;
    }

} //End Namespace COmmunications