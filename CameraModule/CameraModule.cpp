/**
 *  For conditions of distribution and use, see copyright notice in license.txt
 *
 *  @file   CameraModule.cpp
 */

#include "StableHeaders.h"

#include "CameraModule.h"
#include "EventManager.h"
#include "SceneEvents.h"
#include "EC_Placeable.h"
#include "SceneAPI.h"
#include "UiAPI.h"
#include "UiMainWindow.h"

#include <QColor>


namespace Camera
{

	const std::string CameraModule::module_name_ = std::string("CameraModule");

	CameraModule::CameraModule() :
        IModule(module_name_),
		scene_event_category_(0),
        network_state_event_category_(0),
        scene_(0),
        viewport_poller_(new QTimer(this)),
        click_distance_percent_(10)
	{


	}

	CameraModule::~CameraModule()
	{
	}

	void CameraModule::Load()
	{
	}

	void CameraModule::UnLoad()
	{
	}

	void CameraModule::Initialize()
	{		
        if (framework_->IsHeadless())
            return;

        //Get Scene and Network event Category 
        scene_event_category_ = framework_->GetEventManager()->QueryEventCategory("Scene");
		connect(framework_->Scene(), SIGNAL(DefaultWorldSceneChanged(Scene::SceneManager *)), SLOT(DefaultWorldSceneChanged(Scene::SceneManager *)));        

        framework_->Ui()->RegisterUiWidgetFactory(UiWidgetFactoryPtr(this));
       
	}
	void CameraModule::PostInitialize()
	{
        if (framework_->IsHeadless())
            return;

        QAction *action = framework_->Ui()->MainWindow()->AddMenuAction("&View", "New Camera View");
        connect(action, SIGNAL(triggered()), SLOT(CreateNewCamera()));
        
        connect(viewport_poller_, SIGNAL(timeout()), SLOT(UpdateObjectViewport()));  

        //Generate widgets
        ReadConfig();
	}    

	void CameraModule::Uninitialize()
	{
        if (framework_->IsHeadless())
            return;
        //Save widget status to .ini file
        SaveConfig();
	}

	void CameraModule::Update(f64 frametime)
	{
        //delete one widget from dirty widgets set
        if(!dirty_widgets_.isEmpty())        
            DeleteCameraWidget(dirty_widgets_.takeFirst());

	}

	bool CameraModule::HandleEvent(event_category_id_t category_id, event_id_t event_id, IEventData* data)
	{
        bool handled = false;

        if (category_id == scene_event_category_)
        { 
            switch(event_id)
            {
                case Scene::Events::EVENT_ENTITY_CLICKED:
                {           
                    //focus cameras to entity
                    Scene::Events::EntityClickedData *entity_data = checked_static_cast<Scene::Events::EntityClickedData*>(data);
                    if (entity_data)
                    {
                        QMapIterator<CameraWidget*,CameraHandler*> i(controller_view_handlers_);
                        while (i.hasNext()) {
                            i.next();
                            if (i.key()->isVisible())                                                            
                                i.value()->FocusToEntity(entity_data->entity);                            
                        }
                        handled = true;
                        viewport_poller_->start(200);
                    }
                    break;
                }            
                case Scene::Events::EVENT_ENTITY_NONE_CLICKED:
                {
                    viewport_poller_->stop();
                    handled = true;
                    break;
                }

            }
        }
        return handled;
	}

    void CameraModule::DefaultWorldSceneChanged(Scene::SceneManager *scene)
    {
        //create camera entity in all camera handlers
       scene_ = scene;
       QMapIterator<CameraWidget*,CameraHandler*> i(controller_view_handlers_);
        while (i.hasNext()) {
            i.next();
            i.value()->CreateCamera(scene);
        }
    }

    void CameraModule::UpdateObjectViewport()
    {
        //update renderer label
        QMapIterator<CameraWidget*,CameraHandler*> i(controller_view_handlers_);
        while (i.hasNext()) {
            i.next();
            if (i.key()->isVisible())
                i.key()->GetRenderer()->setPixmap(i.value()->RenderCamera(i.key()->widgetRenderer->size()));
        }
    }

    void CameraModule::CreateNewCamera(QString title, bool restored, int camera_type, int projection_type, bool wireframe)
    {
        //Create camera view
        //Create camera handler
        ConnectViewToHandler(CreateCameraWidget(title, restored), CreateCameraHandler(),camera_type, projection_type, wireframe);

    }
    CameraWidget* CameraModule::CreateCameraWidget(QString title, bool restored)
    {           
        GenerateValidWidgetTitle(title);

        CameraWidget* camera_view = new CameraWidget(title);
        UiWidget* camera_widget = framework_->Ui()->AddWidgetToWindow(camera_view);
        camera_widget->show();

        //set qdoc features to flotable and movable, but not closable
        if (camera_widget->parent())
        {
            QDockWidget *doc = dynamic_cast<QDockWidget*>(camera_widget->parent());           
            if (doc)
                doc->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
        }

		connect(camera_widget, SIGNAL(visibilityChanged(bool)), this, SLOT(OnCameraWidgetVisibilityChanged(bool)));
        //insert title to camera view titles
        camera_view_titles_.insert(title);
        return camera_view;
    }

    CameraHandler* CameraModule::CreateCameraHandler()
    {        
        CameraHandler *camera_handler = new CameraHandler(GetFramework());        
        if (scene_ != 0)
            camera_handler->CreateCamera(scene_);
        return camera_handler;
    }

    void CameraModule::ConnectViewToHandler(CameraWidget *camera_view, CameraHandler *camera_handler, int camera_type, int projection_type, bool wireframe)
    {
		//Set dynamic properties for dynamic widget
		camera_view->parentWidget()->setProperty("dynamic",QVariant::fromValue(true));
		camera_view->parentWidget()->setProperty("type",QVariant("CameraWidget"));
		camera_view->parentWidget()->setProperty("DP_Camera",QVariant::fromValue(camera_type));
		camera_view->parentWidget()->setProperty("DP_Projection",QVariant::fromValue(projection_type));
		camera_view->parentWidget()->setProperty("DP_Wireframe",QVariant::fromValue(wireframe));

        //init camera view
        camera_view->comboBoxCameras->addItem("Pivot");
        camera_view->comboBoxCameras->addItem("Front");
        camera_view->comboBoxCameras->addItem("Back");
        camera_view->comboBoxCameras->addItem("Left");
        camera_view->comboBoxCameras->addItem("Rigth");
        camera_view->comboBoxCameras->addItem("Top");
        camera_view->comboBoxCameras->addItem("Bottom");

        camera_view->comboBoxProjection->addItem("Ortho");
        camera_view->comboBoxProjection->addItem("Persp");

        camera_view->comboBoxCameras->setCurrentIndex(camera_type);        
        camera_view->comboBoxProjection->setCurrentIndex(projection_type);
        camera_view->checkBoxWireframe->setChecked(wireframe);

        //init camera handler
        camera_handler->SetCameraType(camera_type);
        camera_handler->SetCameraProjection(projection_type);
        camera_handler->SetCameraWireframe(wireframe);

        //connect signals
         //new camera signal
        connect(camera_view->buttonNewCamera, SIGNAL(released()),this,  SLOT(OnNewButtonClicked())); 

        //close camera signal
		connect(camera_view->buttonDeleteCamera, SIGNAL(released()),this,  SLOT(OnDeleteButtonClicked())); 
        
        //movement signals
        connect(camera_view->GetRenderer(), SIGNAL(Move(qreal, qreal)),camera_handler,  SLOT(Move(qreal, qreal))); 
        connect(camera_view->GetRenderer(), SIGNAL(Zoom(qreal,Qt::KeyboardModifiers)),camera_handler,  SLOT(Zoom(qreal,Qt::KeyboardModifiers))); 
       
        //camera type signal
        connect(camera_view->comboBoxCameras, SIGNAL(currentIndexChanged(int)),camera_handler,  SLOT(SetCameraType(int))); 

        //projection type signal
        connect(camera_view->comboBoxProjection, SIGNAL(currentIndexChanged(int)),camera_handler,  SLOT(SetCameraProjection(int)));
        //wireframe signal
        connect(camera_view->checkBoxWireframe, SIGNAL(stateChanged(int)),this, SLOT(OnWireframeCheckBoxChanged(int))); 

        //clip distance buttons
        connect(camera_view->pushButtonNearPlus, SIGNAL(clicked(bool)), this, SLOT(OnNearPlusButtonClicked(bool)));
        connect(camera_view->pushButtonNearMinus, SIGNAL(clicked(bool)), this, SLOT(OnNearMinusButtonClicked(bool)));
        connect(camera_view->pushButtonFarPlus, SIGNAL(clicked(bool)), this, SLOT(OnFarPlusButtonClicked(bool)));
        connect(camera_view->pushButtonFarMinus, SIGNAL(clicked(bool)), this, SLOT(OnFarMinusButtonClicked(bool)));
        
        //save in map
        controller_view_handlers_.insert(camera_view, camera_handler);       

    }

    void CameraModule::OnNewButtonClicked()
    {
        CreateNewCamera();
    }

    void CameraModule::OnDeleteButtonClicked()
    {
        UiWidget* camera_view = qobject_cast<UiWidget*>(sender()->parent()->parent()->parent());
		if (!camera_view)
            return;
		camera_view->hide();
    }

    void CameraModule::OnCameraWidgetVisibilityChanged(bool visible)
    {
        UiWidget* camera_view = qobject_cast<UiWidget*>(sender());
		if (!camera_view)
            return;

        if (!visible && camera_view->parent())
        {
            QDockWidget *doc = dynamic_cast<QDockWidget*>(camera_view->parent());           
            if (doc && doc->isVisible())
                return;
        }
        
        for (int i = 0; i < dirty_widgets_.size(); ++i) {
            if (dirty_widgets_[i] == camera_view)
            {
                if (visible)
                {
                    dirty_widgets_.removeAt(i);
                }
                return;
            }
        }
        if (!visible)
		    dirty_widgets_.append(camera_view);
    }

    void CameraModule::DeleteCameraWidget(UiWidget *widget)
    {	
        if(!widget->widget())
            return;

        CameraWidget* camera_view = qobject_cast<CameraWidget*>(widget->widget());
		if (!camera_view)
            return;

        QSettings settings(QSettings::IniFormat, QSettings::UserScope, APPLICATION_NAME, "configuration/CameraModuleSettings");
		settings.remove(camera_view->windowTitle());

		camera_view->blockSignals(true);
        
        //remove widget and handler from controller map
        QMap<CameraWidget*,CameraHandler*>::const_iterator i = controller_view_handlers_.find(camera_view);
        while (i != controller_view_handlers_.end() && i.key() == camera_view) {
            CameraWidget *camera = i.key();
            CameraHandler *handler = i.value();            
            camera_view_titles_.remove(camera->windowTitle());
            controller_view_handlers_.remove(camera);
            delete camera;
            delete handler;
        } 

        //remove widget from window
        framework_->Ui()->RemoveWidgetFromWindow(widget);

    }
    void CameraModule::ReadConfig()
    {
        // Init config file if file/segments doesnt exist
        QSettings camera_config(QSettings::IniFormat, QSettings::UserScope, APPLICATION_NAME, "configuration/CameraModuleSettings");

        QStringList cameras = camera_config.childGroups();

        for (int i = 0; i< cameras.length(); i++)
        {
			if (cameras[i].startsWith("CameraExt"))
			{
				camera_config.beginGroup(cameras[i]);
				QVariant camera_type = camera_config.value("CameraType");
				QVariant projection_type = camera_config.value("ProjectionType");
				QVariant wireframe = camera_config.value("Wireframe");
				CreateNewCamera(cameras[i], true, camera_type.toInt(), projection_type.toInt(), wireframe.toBool());
				camera_config.endGroup();
			}
        }
        
    }

    void CameraModule::SaveConfig()
    {
        // Init config file if file/segments doesnt exist
        QSettings camera_config(QSettings::IniFormat, QSettings::UserScope, APPLICATION_NAME, "configuration/CameraModuleSettings");
        //delete previous file
        //camera_config.clear();
        
        QMapIterator<CameraWidget*,CameraHandler*> i(controller_view_handlers_);
        while (i.hasNext()) {
            i.next();
            camera_config.beginGroup(i.key()->windowTitle());
            camera_config.setValue("CameraType", i.value()->GetCameraType());
            camera_config.setValue("ProjectionType", i.value()->GetProjectionType());
            camera_config.setValue("Wireframe", i.value()->IsWireframeEnabled());
            camera_config.endGroup();
        }
    }

    void CameraModule::OnWireframeCheckBoxChanged(int state)
    {
        //capture checkbox state and call the handler
        //Qt::CheckState: 0: unchecked, 1: partial checked, 2: checked
        CameraWidget* camera_view = qobject_cast<CameraWidget*>(sender()->parent()->parent()->parent());
        if (!camera_view)
            return;

        QMap<CameraWidget*,CameraHandler*>::const_iterator i = controller_view_handlers_.find(camera_view);
        while (i != controller_view_handlers_.end() && i.key() == camera_view) {            
            if (state == 0)
                i.value()->SetCameraWireframe(false);
            else
                i.value()->SetCameraWireframe(true);
            i++;
        }
    }

    void CameraModule::GenerateValidWidgetTitle(QString &title)
    {       
        if (title.isNull() || title=="")
        {            
            title = QString("CameraExt").append(QString::number(rand()));
        }
        bool stop = false;
        while (stop == false)
        {
            if (!camera_view_titles_.contains(title))
                stop = true;
            else
                title = QString("CameraExt").append(QString::number(rand()));
        }

    }

    void CameraModule::OnNearPlusButtonClicked(bool checked)
    {
        //capture botton clicked widget and call the handler
        CameraWidget* camera_view = qobject_cast<CameraWidget*>(sender()->parent()->parent()->parent());
        if (!camera_view)
            return;
        QMap<CameraWidget*,CameraHandler*>::const_iterator i = controller_view_handlers_.find(camera_view);
        while (i != controller_view_handlers_.end() && i.key() == camera_view) {            
            i.value()->SetNearClipPercent(click_distance_percent_);
            i++;
        }
    }

    void CameraModule::OnNearMinusButtonClicked(bool checked)
    {
        //capture botton clicked widget and call the handler        
        CameraWidget* camera_view = qobject_cast<CameraWidget*>(sender()->parent()->parent()->parent());
        if (!camera_view)
            return;
        QMap<CameraWidget*,CameraHandler*>::const_iterator i = controller_view_handlers_.find(camera_view);
        while (i != controller_view_handlers_.end() && i.key() == camera_view) {            
            i.value()->SetNearClipPercent(-click_distance_percent_);
            i++;
        }
    }

    void CameraModule::OnFarPlusButtonClicked(bool checked)
    {
        //capture botton clicked widget and call the handler
        CameraWidget* camera_view = qobject_cast<CameraWidget*>(sender()->parent()->parent()->parent());
        if (!camera_view)
            return;
        QMap<CameraWidget*,CameraHandler*>::const_iterator i = controller_view_handlers_.find(camera_view);
        while (i != controller_view_handlers_.end() && i.key() == camera_view) {            
            i.value()->SetFarClipPercent(click_distance_percent_);
            i++;
        }
    }

    void CameraModule::OnFarMinusButtonClicked(bool checked)
    {
        //capture botton clicked widgetand call the handler
        CameraWidget* camera_view = qobject_cast<CameraWidget*>(sender()->parent()->parent()->parent());
        if (!camera_view)
            return;
        QMap<CameraWidget*,CameraHandler*>::const_iterator i = controller_view_handlers_.find(camera_view);
        while (i != controller_view_handlers_.end() && i.key() == camera_view) {            
            i.value()->SetFarClipPercent(-click_distance_percent_);
            i++;
        }
    }

    QWidget* CameraModule::CreateWidget(const QString &name, const QStringList &params)
    {
        int camera_type = 0;
        int projection_type = 1;
        bool wireframe = false;

        int i = 1;
        while (i < params.size())
        {
            if (params.at(i-1).compare(QString("DP_Camera")) == 0) {
                camera_type = params.at(i).toInt();
            }
            else if (params.at(i-1).compare(QString("DP_Projection")) == 0) {
                projection_type = params.at(i).toInt();
            }
            else if (params.at(i-1).compare(QString("DP_Wireframe")) == 0) {
                wireframe = (params.at(i).compare(QString("true"), Qt::CaseInsensitive) == 0);
            }
            i += 2;
        }
        
        CameraWidget* cameraWidget = CreateCameraWidget(name, true);
        
        ConnectViewToHandler(cameraWidget, CreateCameraHandler(),camera_type, projection_type, wireframe);

        return cameraWidget;
    }

}


extern "C" void POCO_LIBRARY_API SetProfiler(Foundation::Profiler *profiler);
void SetProfiler(Foundation::Profiler *profiler)
{
    Foundation::ProfilerSection::SetProfiler(profiler);
}


using namespace Camera;

POCO_BEGIN_MANIFEST(IModule)
    POCO_EXPORT_CLASS(CameraModule)
POCO_END_MANIFEST

