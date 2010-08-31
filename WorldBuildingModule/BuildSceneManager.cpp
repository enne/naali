// For conditions of distribution and use, see copyright notice in license.txt

#include "StableHeaders.h"
#include "DebugOperatorNew.h"

#include "BuildSceneManager.h"
#include "BuildScene.h"
#include "AnchorLayout.h"
#include "PropertyEditorHandler.h"
#include "BuildingWidget.h"
#include "UiHelper.h"

#include "ModuleInterface.h"
#include "EC_OpenSimPrim.h"
#include "EC_OgrePlaceable.h"
#include "UiServiceInterface.h"

#include <QPixmap>
#include <QDebug>

#include "MemoryLeakCheck.h"

namespace WorldBuilding
{
    BuildSceneManager::BuildSceneManager(QObject *parent, Foundation::Framework *framework) :
        framework_(framework),
        ui_helper_(new Helpers::UiHelper(parent)),
        inworld_state(false),
        scene_name_("WorldBuilding"),
        world_object_view_(0),
        python_handler_(0),
        property_editor_handler_(0),
        camera_handler_(0),
        prim_selected_(true),
        selected_entity_(0),
        selected_camera_id_(-1)
    {
        setParent(parent);
        InitialseScene();
        ObjectSelected(false);
        object_view_data_.Reset();
    }

    BuildSceneManager::~BuildSceneManager()
    {
        SAFE_DELETE(object_info_widget_);
        SAFE_DELETE(object_manipulations_widget_);
    }

    void BuildSceneManager::InitialseScene()
    {
        scene_ = new BuildScene(this);
        layout_ = new AnchorLayout(this, scene_);

        // Register scene to ui service
        Foundation::UiServiceInterface *ui = framework_->GetService<Foundation::UiServiceInterface>();
        if (ui)
        {
            ui->RegisterScene(scene_name_, scene_);
            connect(ui, SIGNAL(SceneChanged(const QString&, const QString &)),
                SLOT(SceneChangedNotification(const QString&, const QString&)));
        }

        // Init info widget
        object_info_widget_ = new Ui::BuildingWidget(Ui::BuildingWidget::Right);
        object_info_ui.setupUi(object_info_widget_->GetInternal());

        layout_->AddCornerAnchor(object_info_widget_, Qt::TopRightCorner, Qt::TopRightCorner);
        layout_->AddCornerAnchor(object_info_widget_, Qt::BottomRightCorner, Qt::BottomRightCorner);

        object_info_widget_->PrepWidget();
        connect(scene_, SIGNAL(sceneRectChanged(const QRectF&)), object_info_widget_, SLOT(SceneRectChanged(const QRectF&)));

        world_object_view_ = new WorldObjectView();
        world_object_view_->setFixedSize(300,300);
        object_info_ui.viewport_layout->addWidget(world_object_view_);
        object_info_widget_->SetWorldObjectView(world_object_view_);


        // Init manipulations widget
        object_manipulations_widget_ = new Ui::BuildingWidget(Ui::BuildingWidget::Left);
        object_manip_ui.setupUi(object_manipulations_widget_->GetInternal());
        object_manip_ui.rotate_frame->hide();

        connect(object_manip_ui.button_new, SIGNAL(clicked()), SLOT(NewObjectClicked()));
        connect(object_manip_ui.button_clone, SIGNAL(clicked()), SLOT(DuplicateObjectClicked()));
        connect(object_manip_ui.button_delete, SIGNAL(clicked()), SLOT(DeleteObjectClicked()));
        connect(object_manip_ui.button_move, SIGNAL(clicked()), SLOT(ModeToggleMove()));
        connect(object_manip_ui.button_scale, SIGNAL(clicked()), SLOT(ModeToggleScale()));
        connect(object_manip_ui.button_rotate, SIGNAL(clicked()), SLOT(ModeToggleRotate()));

        layout_->AddCornerAnchor(object_manipulations_widget_, Qt::TopLeftCorner, Qt::TopLeftCorner);
        layout_->AddCornerAnchor(object_manipulations_widget_, Qt::BottomLeftCorner, Qt::BottomLeftCorner);

        object_manipulations_widget_->PrepWidget();
        connect(scene_, SIGNAL(sceneRectChanged(const QRectF&)), object_manipulations_widget_, SLOT(SceneRectChanged(const QRectF&)));

        // Init python handler
        python_handler_ = new PythonHandler(this);

        // Init editor handler
        property_editor_handler_ = new PropertyEditorHandler(ui_helper_, this);

        // Init camera handler
        camera_handler_ = new View::CameraHandler(framework_, this);
        connect(world_object_view_, SIGNAL(RotateObject(qreal, qreal)), this, SLOT(RotateObject(qreal, qreal))); 
        connect(world_object_view_, SIGNAL(Zoom(qreal)), this, SLOT(Zoom(qreal))); 
        

        // Setup ui helper
        ui_helper_->SetupRotateControls(&object_manip_ui, python_handler_);
    }

    void BuildSceneManager::KeyPressed(KeyEvent *key)
    {
        if (key->IsRepeat())
            return;

        // Ctrl + B to toggle build scene
        if (key->HasCtrlModifier() && key->keyCode == Qt::Key_B)
        {
            ToggleBuildScene();
            return;
        }

        if (!scene_->isActive() || !property_editor_handler_->HasCurrentPrim() || !prim_selected_)
            return;

        PythonParams::ManipulationMode mode = PythonParams::MANIP_NONE;
        if (key->HasCtrlModifier())
        {
            mode = PythonParams::MANIP_MOVE;
            if (key->HasAltModifier() && (python_handler_->GetCurrentManipulationMode() != PythonParams::MANIP_ROTATE))
                mode = PythonParams::MANIP_SCALE;
            if (key->HasShiftModifier() && (python_handler_->GetCurrentManipulationMode() != PythonParams::MANIP_SCALE))
                mode = PythonParams::MANIP_ROTATE;
        }
        if (mode != PythonParams::MANIP_NONE)
            ManipModeChanged(mode);
    }

    void BuildSceneManager::KeyReleased(KeyEvent *key)
    {
        if (!scene_->isActive() || !property_editor_handler_->HasCurrentPrim() || !prim_selected_)
            return;
        if (key->IsRepeat())
            return;

        PythonParams::ManipulationMode mode = PythonParams::MANIP_NONE;
        if (key->keyCode == Qt::Key_Control && (python_handler_->GetCurrentManipulationMode() == PythonParams::MANIP_MOVE))
            mode = PythonParams::MANIP_FREEMOVE;
        if (key->keyCode == Qt::Key_Alt && (python_handler_->GetCurrentManipulationMode() == PythonParams::MANIP_SCALE))
        {
            if (key->HasCtrlModifier())
                mode = PythonParams::MANIP_MOVE;
            else
                mode = PythonParams::MANIP_FREEMOVE;
        }
        if (key->keyCode == Qt::Key_Shift && (python_handler_->GetCurrentManipulationMode() == PythonParams::MANIP_ROTATE))
        {
            if (key->HasCtrlModifier())
                mode = PythonParams::MANIP_MOVE;
            else
                mode = PythonParams::MANIP_FREEMOVE;
        }
        if (mode != PythonParams::MANIP_NONE)
            ManipModeChanged(mode);
    }

    void BuildSceneManager::ModeToggleMove()
    {
        if (!scene_->isActive() || !property_editor_handler_->HasCurrentPrim() || !prim_selected_)
            return;

        if (python_handler_->GetCurrentManipulationMode() == PythonParams::MANIP_MOVE)
            ManipModeChanged(PythonParams::MANIP_FREEMOVE);
        else
            ManipModeChanged(PythonParams::MANIP_MOVE);
    }

    void BuildSceneManager::ModeToggleScale()
    {
        if (!scene_->isActive() || !property_editor_handler_->HasCurrentPrim() || !prim_selected_)
            return;

        if (python_handler_->GetCurrentManipulationMode() == PythonParams::MANIP_SCALE)
            ManipModeChanged(PythonParams::MANIP_FREEMOVE);
        else
            ManipModeChanged(PythonParams::MANIP_SCALE);
    }

    void BuildSceneManager::ModeToggleRotate()
    {
        if (!scene_->isActive() || !property_editor_handler_->HasCurrentPrim() || !prim_selected_)
            return;

        if (python_handler_->GetCurrentManipulationMode() == PythonParams::MANIP_ROTATE)
            ManipModeChanged(PythonParams::MANIP_FREEMOVE);
        else
            ManipModeChanged(PythonParams::MANIP_ROTATE);
    }

    void BuildSceneManager::NewObjectClicked()
    {
        python_handler_->EmitObjectAction(PythonParams::OBJ_NEW);
    }

    void BuildSceneManager::DuplicateObjectClicked()
    {
        python_handler_->EmitObjectAction(PythonParams::OBJ_CLONE);
    }

    void BuildSceneManager::DeleteObjectClicked()
    {
        python_handler_->EmitObjectAction(PythonParams::OBJ_DELETE);
        ObjectSelected(false);
    }

    void BuildSceneManager::ManipModeChanged(PythonParams::ManipulationMode mode)
    {
        python_handler_->EmitManipulationModeChange(mode);

        bool show_rotate_controls = false;
        QString selected_style = "background-color: qlineargradient(spread:pad, x1:0, y1:0.165, x2:0, y2:0.864, stop:0 rgba(248, 248, 248, 255), stop:1 rgba(232, 232, 232, 255));"
                                 "border: 1px solid grey; border-radius: 0px; color: black; font-weight: bold; padding-top: 5px; padding-bottom: 4px;";
        switch (mode)
        {
            case PythonParams::MANIP_MOVE:
                object_manip_ui.button_move->setStyleSheet(selected_style);
                object_manip_ui.button_scale->setStyleSheet("");
                object_manip_ui.button_rotate->setStyleSheet("");
                break;
            case PythonParams::MANIP_SCALE:
                object_manip_ui.button_scale->setStyleSheet(selected_style);
                object_manip_ui.button_move->setStyleSheet("");
                object_manip_ui.button_rotate->setStyleSheet("");
                break;
            case PythonParams::MANIP_ROTATE:
                object_manip_ui.button_rotate->setStyleSheet(selected_style);
                object_manip_ui.button_move->setStyleSheet("");
                object_manip_ui.button_scale->setStyleSheet("");
                show_rotate_controls = true;
                break;
            case PythonParams::MANIP_FREEMOVE:
                object_manip_ui.button_move->setStyleSheet("");
                object_manip_ui.button_scale->setStyleSheet("");
                object_manip_ui.button_rotate->setStyleSheet("");
                break;
            default:
                break;
        }
        object_manip_ui.rotate_frame->setVisible(show_rotate_controls);
    }

    QObject *BuildSceneManager::GetPythonHandler() const
    {
        return python_handler_; 
    }

    void BuildSceneManager::ToggleBuildScene()
    {
        if (!scene_->isActive())
            ShowBuildScene();
        else
            HideBuildScene();
    }

    void BuildSceneManager::ShowBuildScene()
    {
        if (!inworld_state)
            return;

        Foundation::UiServiceInterface *ui = framework_->GetService<Foundation::UiServiceInterface>();
        if (ui)
        {
            ui->SwitchToScene(scene_name_);

            object_info_widget_->CheckSize();
            object_manipulations_widget_->CheckSize();

            python_handler_->EmitEditingActivated(true);
        }
    }

    void BuildSceneManager::HideBuildScene()
    {
        Foundation::UiServiceInterface *ui = framework_->GetService<Foundation::UiServiceInterface>();
        if (ui)
            if (inworld_state)
                ui->SwitchToScene("Inworld");
            else
                ui->SwitchToScene("Ether");
    }

    void BuildSceneManager::SceneChangedNotification(const QString &old_name, const QString &new_name)
    {
        if (new_name == scene_name_)
        {
            object_info_widget_->CheckSize();
            object_manipulations_widget_->CheckSize();
            python_handler_->EmitEditingActivated(true);
        }
        else if (old_name == scene_name_)
        {
            ObjectSelected(false);
            python_handler_->EmitEditingActivated(false);
        }
    }

    void BuildSceneManager::CreateCamera()
    {
        if (selected_camera_id_ == -1 && camera_handler_)
            selected_camera_id_ = camera_handler_->CreateCustomCamera();
    }

    void BuildSceneManager::ResetCamera()
    {
        if (selected_camera_id_ != -1 && camera_handler_)
        {
            camera_handler_->DestroyCamera(selected_camera_id_);
            selected_camera_id_ = -1;
        }
    }

    void BuildSceneManager::ResetEditing()
    {
        ObjectSelected(false);
        property_editor_handler_->ClearCurrentPrim();
    }
    
    void BuildSceneManager::ObjectDeselected()
    {
        ObjectSelected(false);
        python_handler_->EmitRemoveHightlight();
    }

    void BuildSceneManager::ObjectSelected(bool selected)
    {
        // Update ui elements
        if (selected)
            ManipModeChanged(python_handler_->GetCurrentManipulationMode());
        else
            ManipModeChanged(PythonParams::MANIP_FREEMOVE);

         // Update ui visibility if the mode changed from previous known one
        if (prim_selected_ == selected)
            return;
        prim_selected_ = selected;

        object_info_ui.status_label->setVisible(!selected);
        world_object_view_->setVisible(selected);
        object_info_ui.server_id_title->setVisible(selected);
        object_info_ui.server_id_value->setVisible(selected);
        object_info_ui.local_id_title->setVisible(selected);
        object_info_ui.local_id_value->setVisible(selected);

        object_manip_ui.button_clone->setEnabled(selected);
        object_manip_ui.button_delete->setEnabled(selected);
        object_manip_ui.button_scale->setEnabled(selected);
        object_manip_ui.button_move->setEnabled(selected);
        object_manip_ui.button_rotate->setEnabled(selected);

        property_editor_handler_->SetEditorVisible(selected);
    }

    void BuildSceneManager::ObjectSelected(Scene::Entity *entity)
    {
        if (!entity || !scene_->isActive())
            return;

        EC_OpenSimPrim *prim = entity->GetComponent<EC_OpenSimPrim>().get();
        if (!prim)
        {
            // Manipulators wont have prim, but will have mesh
            if (entity->HasComponent("EC_OgreMesh"))
                return;
            ObjectSelected(false);
            python_handler_->EmitRemoveHightlight();
            return;
        }

        // Update our widgets UI
        object_info_ui.server_id_value->setText(ui_helper_->CheckUiValue(prim->getFullId()));
        object_info_ui.local_id_value->setText(ui_helper_->CheckUiValue(prim->getLocalId()));

        // Update entity viewport UI
        if (camera_handler_->FocusToEntity(selected_camera_id_, entity))
        {
            //qDebug() << "ObjectViewData: " << object_view_data_.x << " - " << object_view_data_.y << " : " << object_view_data_.delta;
            world_object_view_->setPixmap(camera_handler_->RenderCamera(selected_camera_id_, world_object_view_->size()));
            if (!world_object_view_->text().isEmpty())
                world_object_view_->setText("");
        }
        else
        {
            QPixmap disabled_pixmap(world_object_view_->size());
            disabled_pixmap.fill(Qt::gray);
            world_object_view_->setPixmap(disabled_pixmap);
            world_object_view_->setText("Could not focus to object");
        }

        // Update the property editor
        if (!property_editor_handler_->HasCurrentPrim())
            property_editor_handler_->CreatePropertyBrowser(object_info_widget_->GetInternal(), object_info_ui.property_browser_layout, prim);
        else
            property_editor_handler_->PrimSelected(prim);
        ObjectSelected(true);
        selected_entity_ =  entity;
    }

    void BuildSceneManager::Zoom(qreal delta)
    {
        if (selected_entity_)
        {
            OgreRenderer::EC_OgrePlaceable *entity_ec_placable = selected_entity_->GetComponent<OgreRenderer::EC_OgrePlaceable>().get();
            if (entity_ec_placable)
            {
                qreal acceleration = 0.01;
                if (camera_handler_->ZoomRelativeToPoint(entity_ec_placable->GetPosition(),selected_camera_id_, delta*acceleration))
                    world_object_view_->setPixmap(camera_handler_->RenderCamera(selected_camera_id_, world_object_view_->size()));
                
                // Manaluusua: Fill this with needed data to remember zoom/rotation/pos of camera when you change object
                // prolly will be vectors etc not these simple x,y as they mean nothing to the camera
                object_view_data_.delta = delta;
                object_view_data_.acceleration_delta = acceleration;
                //qDebug() << "Zoom: " << delta << " with accel " << acceleration;
            }
        }
    }

    void BuildSceneManager::RotateObject(qreal x, qreal y)
    {
        if (selected_entity_)
        {
            OgreRenderer::EC_OgrePlaceable *entity_ec_placable = selected_entity_->GetComponent<OgreRenderer::EC_OgrePlaceable>().get();
            if (entity_ec_placable)
            {
                qreal acceleration_x = 1;
                qreal acceleration_y = 1;
                camera_handler_->RotateCamera(entity_ec_placable->GetPosition(),selected_camera_id_,x*acceleration_x,y*acceleration_y);
                world_object_view_->setPixmap(camera_handler_->RenderCamera(selected_camera_id_, world_object_view_->size()));

                // Manaluusua: Fill this with needed data to remember zoom/rotation/pos of camera when you change object
                // prolly will be vectors etc not these simple x,y as they mean nothing to the camera
                object_view_data_.x = x;
                object_view_data_.y = y;
                object_view_data_.acceleration_x = acceleration_x;
                object_view_data_.acceleration_y = acceleration_y;
                //qDebug() << "Rotate: " << x << " - " << y << " with accel " << acceleration_x << " - " << acceleration_y;
            }
        }
    }
}
