//$ HEADER_MOD_FILE $
/**
 *  For conditions of distribution and use, see copyright notice in license.txt
 *
 *  @file   UiSceneService.cpp
 *  @brief  Implements UiServiceInterface and provides means of adding widgets to the 
 *          in-world scene and managing different UI scenes.
 *          Basically this class is just a wrapper around InworldSceneController
 *          and UiStateMachine.
 */

#include "StableHeaders.h"
#include "UiSceneService.h"
#include "UiModule.h"
#include "Inworld/InworldSceneController.h"
#include "UiStateMachine.h"
#include "UiProxyWidget.h"
#include "Inworld/Menus/MenuManager.h"
#include "Inworld/NotificationManager.h"

#include <QUiLoader>
#include <QSettings>

#include "Inworld/ControlPanel/SettingsWidget.h"

namespace UiServices
{
    UiSceneService::UiSceneService(UiModule *owner) : owner_(owner)
    {
        connect(owner_->GetUiStateMachine(), SIGNAL(SceneChanged(const QString&, const QString&)),
                this, SIGNAL(SceneChanged(const QString&, const QString&)));
        connect(owner_->GetUiStateMachine(), SIGNAL(SceneChangeComplete()),
                this, SLOT(TranferWidgets()));

        connect(owner_->GetNotificationManager(), SIGNAL(ShowNotificationCalled(const QString&)), this, SIGNAL(Notification(const QString&)));

		//connect(owner_->GetUiStateMachine(), SIGNAL(SceneChanged(const QString&, const QString&)),this,SLOT(HandleTransferToBuild(const QString&, const QString&)));
		//Settings Panel, List of panels send when we enter the world
		//connect(owner_->GetUiStateMachine(), SIGNAL(SceneChanged(const QString&, const QString&)), this,SLOT(SetPanelsList(const QString&, const QString&)));
    }

    UiSceneService::~UiSceneService()
    {
    }

    UiProxyWidget *UiSceneService::AddWidgetToScene(QWidget *widget, bool dockable , bool outside, Qt::WindowFlags flags)
    {
		//QSettings settings("Naali UIExternal2", "UiExternal Settings");
		QSettings settings(QSettings::IniFormat, QSettings::UserScope, APPLICATION_NAME, "configuration/UiExternalSettings");
		QString pos = "vacio";
		if (widget->windowTitle() == "")
			return 0;
		
		pos = settings.value(widget->windowTitle(), QString("vacio")).toString();

		if (pos == "outside")
			external_dockeable_widgets_[widget->windowTitle()] = owner_->GetExternalPanelManager()->AddExternalPanel(widget,widget->windowTitle());
		else if(pos == "inside") {
			internal_widgets_[widget->windowTitle()] = owner_->GetInworldSceneController()->AddWidgetToScene(widget, flags);
			return internal_widgets_[widget->windowTitle()];
		}		
		else
		{
			if (outside && dockable) 
			{
				external_dockeable_widgets_[widget->windowTitle()] = owner_->GetExternalPanelManager()->AddExternalPanel(widget,widget->windowTitle());
				settings.setValue(widget->windowTitle(), "outside");
			}

			else if (outside && !dockable)
				external_nondockeable_widgets_[widget->windowTitle()] = widget;
			else {
				internal_widgets_[widget->windowTitle()] = owner_->GetInworldSceneController()->AddWidgetToScene(widget, flags);
				settings.setValue(widget->windowTitle(), "inside");
				return internal_widgets_[widget->windowTitle()];
			}
		}
		return 0;
    }

    bool UiSceneService::AddWidgetToScene(UiProxyWidget *widget)
    {
		//DEFAULT OUTSIDE = TRUE & MOVEABLE = TRUE. IMPORTANT THING TO HAVE IN MIND!!
		//QSettings settings("Naali UIExternal2", "UiExternal Settings");
		QSettings settings(QSettings::IniFormat, QSettings::UserScope, APPLICATION_NAME, "configuration/UiExternalSettings");
		QString pos = "vacio";
		if (widget->windowTitle() != "")
			pos = settings.value(widget->windowTitle(), QString("vacio")).toString();

		if(pos == "inside")
			if (owner_->GetInworldSceneController()->AddProxyWidget(widget)){
				internal_widgets_[widget->windowTitle()] = widget;
				return true;
			}
		else
			external_dockeable_widgets_[widget->windowTitle()] = owner_->GetExternalPanelManager()->AddExternalPanel(widget->widget(),widget->windowTitle());			
		return false;
    }

    void UiSceneService::AddWidgetToMenu(QWidget *widget)
    {
		//Save initial values
		if (!panels_menus_list_.contains(widget->windowTitle()))
			panels_menus_list_[widget->windowTitle()]=menusPair("", "");

		if (external_dockeable_widgets_.contains(widget->windowTitle()))
           owner_->GetExternalMenuManager()->AddExternalMenuPanel(external_dockeable_widgets_[widget->windowTitle()],widget->windowTitle(),"Panels");
		else if  (external_nondockeable_widgets_.contains(widget->windowTitle()))
			owner_->GetExternalMenuManager()->AddExternalMenuPanel(external_nondockeable_widgets_[widget->windowTitle()],widget->windowTitle(),"Panels");
        else if  (internal_widgets_.contains(widget->windowTitle()))
			owner_->GetInworldSceneController()->AddWidgetToMenu(external_nondockeable_widgets_[widget->windowTitle()], widget->windowTitle(), "", "");
    }

    void UiSceneService::AddWidgetToMenu(QWidget *widget, const QString &entry, const QString &menu, const QString &icon)
    {
		//Save initial values
		if (!panels_menus_list_.contains(widget->windowTitle()))
			panels_menus_list_[widget->windowTitle()]=menusPair(menu, icon);

		if (external_dockeable_widgets_.contains(widget->windowTitle()))
           owner_->GetExternalMenuManager()->AddExternalMenuPanel(external_dockeable_widgets_[widget->windowTitle()],entry,menu);
		else if  (external_nondockeable_widgets_.contains(widget->windowTitle()))
			owner_->GetExternalMenuManager()->AddExternalMenuPanel(external_nondockeable_widgets_[widget->windowTitle()],entry,menu);
        else if  (internal_widgets_.contains(widget->windowTitle()))
            owner_->GetInworldSceneController()->AddWidgetToMenu(widget, entry, menu, icon);
    }

    void UiSceneService::AddWidgetToMenu(UiProxyWidget *widget, const QString &entry, const QString &menu, const QString &icon)
    {
		//Save initial values
		if (!panels_menus_list_.contains(widget->windowTitle()))
			panels_menus_list_[widget->windowTitle()]=menusPair(menu, icon);

		if (internal_widgets_.contains(widget->windowTitle()))
			owner_->GetInworldSceneController()->AddWidgetToMenu(widget->widget(), entry, menu, icon);
		else if (external_dockeable_widgets_.contains(widget->windowTitle()))
			owner_->GetExternalMenuManager()->AddExternalMenuPanel(external_dockeable_widgets_[widget->windowTitle()],entry,menu);
		else if (external_nondockeable_widgets_.contains(widget->windowTitle()))
			owner_->GetExternalMenuManager()->AddExternalMenuPanel(external_nondockeable_widgets_[widget->windowTitle()],entry,menu);
    }

    void UiSceneService::RemoveWidgetFromMenu(QWidget *widget)
    {
		owner_->GetExternalMenuManager()->RemoveExternalMenuPanel(widget);
		owner_->GetInworldSceneController()->RemoveWidgetFromMenu(widget);
    }

    void UiSceneService::RemoveWidgetFromMenu(QGraphicsProxyWidget *widget)
    {        
		if (external_dockeable_widgets_.contains(widget->windowTitle()))
			owner_->GetExternalMenuManager()->RemoveExternalMenuPanel(external_dockeable_widgets_[widget->windowTitle()]);
		else if (external_nondockeable_widgets_.contains(widget->windowTitle()))
			owner_->GetExternalMenuManager()->RemoveExternalMenuPanel(external_nondockeable_widgets_[widget->windowTitle()]);
		else 
			owner_->GetInworldSceneController()->RemoveWidgetFromMenu(widget->widget());
    }

    void UiSceneService::RemoveWidgetFromScene(QWidget *widget)
    {
		if (internal_widgets_.contains(widget->windowTitle()))
			owner_->GetInworldSceneController()->RemoveProxyWidgetFromScene(widget);
		else if (external_dockeable_widgets_.contains(widget->windowTitle()))
			owner_->GetExternalPanelManager()->RemoveExternalPanel(external_dockeable_widgets_[widget->windowTitle()]);
		else if (external_nondockeable_widgets_.contains(widget->windowTitle()))
			SAFE_DELETE(widget);
    }

    void UiSceneService::RemoveWidgetFromScene(QGraphicsProxyWidget *widget)
    {
		if (internal_widgets_.contains(widget->windowTitle()))
			owner_->GetInworldSceneController()->RemoveProxyWidgetFromScene(widget);
		else if (external_dockeable_widgets_.contains(widget->windowTitle()))
			owner_->GetExternalPanelManager()->RemoveExternalPanel(external_dockeable_widgets_[widget->windowTitle()]->widget());
		else if (external_nondockeable_widgets_.contains(widget->windowTitle()))
			SAFE_DELETE(widget);
    }

    void UiSceneService::ShowWidget(QWidget *widget) const
    {
		if (internal_widgets_.contains(widget->windowTitle()))
			owner_->GetInworldSceneController()->ShowProxyForWidget(widget);
		else if (external_dockeable_widgets_.contains(widget->windowTitle()))
			owner_->GetExternalPanelManager()->ShowWidget(external_dockeable_widgets_[widget->windowTitle()]);
		else if (external_nondockeable_widgets_.contains(widget->windowTitle()))
			widget->show();
    }

    void UiSceneService::HideWidget(QWidget *widget) const
    {
		if (internal_widgets_.contains(widget->windowTitle()))
			owner_->GetInworldSceneController()->HideProxyForWidget(widget);
		else if (external_dockeable_widgets_.contains(widget->windowTitle()))
			owner_->GetExternalPanelManager()->HideWidget(widget);
		else if (external_nondockeable_widgets_.contains(widget->windowTitle()))
			widget->hide();
    }

    void UiSceneService::BringWidgetToFront(QWidget *widget) const
    {
		if (internal_widgets_.contains(widget->windowTitle()))
			owner_->GetInworldSceneController()->BringProxyToFront(widget);
		else if (external_dockeable_widgets_.contains(widget->windowTitle()))
			owner_->GetExternalPanelManager()->ShowWidget(widget);
		else if (external_nondockeable_widgets_.contains(widget->windowTitle()))
			widget->show();
    }

	void UiSceneService::BringWidgetToFront(QString widget)
    {
		if (internal_widgets_.contains(widget))
			owner_->GetInworldSceneController()->BringProxyToFront(internal_widgets_[widget]);
		else if (external_dockeable_widgets_.contains(widget))
			owner_->GetExternalPanelManager()->ShowWidget(external_dockeable_widgets_[widget]);
		else if (external_nondockeable_widgets_.contains(widget))
			external_nondockeable_widgets_[widget]->show();
    }

    void UiSceneService::BringWidgetToFront(QGraphicsProxyWidget *widget) const
    {
		if (internal_widgets_.contains(widget->windowTitle()))
			owner_->GetInworldSceneController()->BringProxyToFront(widget);
		else if (external_dockeable_widgets_.contains(widget->windowTitle()))
			owner_->GetExternalPanelManager()->ShowWidget(external_dockeable_widgets_[widget->windowTitle()]);
		else if (external_nondockeable_widgets_.contains(widget->windowTitle()))
			external_nondockeable_widgets_[widget->windowTitle()]->show();
    }

//$ END_MOD $
    bool UiSceneService::AddSettingsWidget(QWidget *widget, const QString &name) const
    {
        return owner_->GetInworldSceneController()->AddSettingsWidget(widget, name);
    }

    QGraphicsScene *UiSceneService::GetScene(const QString &name) const
    {
        return owner_->GetUiStateMachine()->GetScene(name);
    }

    void UiSceneService::RegisterScene(const QString &name, QGraphicsScene *scene)
    {
        owner_->GetUiStateMachine()->RegisterScene(name, scene);
    }

    bool UiSceneService::UnregisterScene(const QString &name)
    {
        return owner_->GetUiStateMachine()->UnregisterScene(name);
    }

    bool UiSceneService::SwitchToScene(const QString &name)
    {
       return owner_->GetUiStateMachine()->SwitchToScene(name);
    }

    void UiSceneService::RegisterUniversalWidget(const QString &name, QGraphicsProxyWidget *widget)
    {
        return owner_->GetUiStateMachine()->RegisterUniversalWidget(name, widget);
    }

    void UiSceneService::ShowNotification(CoreUi::NotificationBaseWidget *notification_widget)
    {
        owner_->GetNotificationManager()->ShowNotification(notification_widget);
    }

    QWidget *UiSceneService::LoadFromFile(const QString &file_path, bool add_to_scene, QWidget *parent)
    {
        QWidget *widget = 0;
        QUiLoader loader;
        QFile file(file_path); 
        file.open(QFile::ReadOnly);
        widget = loader.load(&file, parent);
        if(add_to_scene && widget)
            AddWidgetToScene(widget);
        return widget;
    }

    void UiSceneService::TranferWidgets()
    {
        CoreUi::UniversalWidgetMap universal_widgets = owner_->GetUiStateMachine()->GetUniversalWidgets();
        foreach(QString widget_name, universal_widgets.keys())
        {
            QGraphicsProxyWidget *widget = universal_widgets[widget_name];
            if (!widget)
                continue;
            emit TransferRequest(widget_name, widget);
        }
    }
//$ BEGIN_MOD $
	bool UiSceneService::AddExternalMenuAction(QAction *action, const QString &name, const QString &menu, const QString &icon){
		return owner_->GetExternalMenuManager()->AddExternalMenuAction(action,name,menu,icon);
	}

	bool UiSceneService::AddExternalToolbar(QToolBar *toolbar, const QString &name){
		return owner_->GetExternalToolBarManager()->AddExternalToolbar(toolbar, name);
    }

	bool UiSceneService::RemoveExternalToolbar(QString name){
		return owner_->GetExternalToolBarManager()->RemoveExternalToolbar(name);
    }

	bool UiSceneService::ShowExternalToolbar(QString name){
		return owner_->GetExternalToolBarManager()->ShowExternalToolbar(name);
    }

	bool UiSceneService::HideExternalToolbar(QString name){
		return owner_->GetExternalToolBarManager()->HideExternalToolbar(name);
    }

	bool UiSceneService::EnableExternalToolbar(QString name){
		return owner_->GetExternalToolBarManager()->EnableExternalToolbar(name);
    }

	bool UiSceneService::DisableExternalToolbar(QString name){
		return owner_->GetExternalToolBarManager()->DisableExternalToolbar(name);
    }

	QToolBar* UiSceneService::GetExternalToolbar(QString name){
		return owner_->GetExternalToolBarManager()->GetExternalToolbar(name);
    }

	//TO MANAGE MENU SETTINGS
	void UiSceneService::CreateSettingsPanel(){
		CoreUi::SettingsWidget *settings_widget_ = dynamic_cast<CoreUi::SettingsWidget *>(owner_->GetInworldSceneController()->GetSettingsObject());
		QWidget *internal_wid = settings_widget_->GetInternalWidget();//
		if (internal_wid)
			AddWidgetToScene(internal_wid, true, true);
	}

	bool UiSceneService::IsMenuInside(QString name){
		if (internal_widgets_.contains(name))
			return true;
		else
			return false;
	}

	bool UiSceneService::IsMenuMoveable(QString name){
		if (external_dockeable_widgets_.contains(name))
			return true;
		else
			return false;
	}	
//$ END_MOD $
}