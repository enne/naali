//$ HEADER_NEW_FILE $

#include "StableHeaders.h"
#include "CameraView.h"

#include <QMouseEvent>
#include <QDebug>
#include <QCursor>

#define PI 3.14159265

namespace Camera
{
    CameraWidget::CameraWidget(QString title, QWidget* widget) :
        QWidget(widget)
    {                     
        setupUi(this);        
        setWindowTitle(title);
        renderer = new CameraView(widgetRenderer);    
        verticalLayout_2->addWidget(renderer);
        connect(comboBoxCameras, SIGNAL(currentIndexChanged(const QString &)),this,  SLOT(SetWindowTitle(const QString &)));
        //camera type signal
        connect(comboBoxCameras, SIGNAL(currentIndexChanged(int)),SLOT(UpdateCameraType(int))); 
        //projection type signal
        connect(comboBoxProjection, SIGNAL(currentIndexChanged(int)),SLOT(UpdateCameraProjection(int)));
        //wireframe signal
        connect(checkBoxWireframe, SIGNAL(stateChanged(int)), SLOT(UpdateWireframe(int))); 
    }
    
    CameraWidget::~CameraWidget()
    {
        delete renderer;
    }

    void CameraWidget::SetWindowTitle(const QString &name)
    {
        if (this->parentWidget())
        {
            if (this->parentWidget()->parentWidget())
            {
                QDockWidget *doc = dynamic_cast<QDockWidget*>(this->parentWidget()->parentWidget());           
                if (doc)
                {
                    doc->setWindowTitle(name + " Camera"); 
                    return;
                }
            }
            this->parentWidget()->setWindowTitle(name + " Camera");        
        }
    }

	void CameraWidget::UpdateCameraType(const int camera_type){
		parentWidget()->setProperty("DP_Camera",QVariant::fromValue(camera_type));
	}
	void CameraWidget::UpdateCameraProjection(const int projection_type){
		parentWidget()->setProperty("DP_Projection",QVariant::fromValue(projection_type));
	}
	void CameraWidget::UpdateWireframe(const int wireframe){
        //Qt::CheckState: 0: unchecked, 1: partial checked, 2: checked
		if (wireframe == 0)
			parentWidget()->setProperty("DP_Wireframe",QVariant::fromValue(false));
		else
			setProperty("DP_Wireframe",QVariant::fromValue(true));
	}

    //void CameraWidget::hideEvent(QHideEvent * event)
    //{
    //    if (isHidden())
    //        emit WidgetHidden();
    //}

    CameraView::CameraView(QWidget* widget) :
        QLabel(widget),
        left_mousebutton_pressed_(false)
    {              
        if (widget)
            widget->setMinimumSize(300,300);        
        setMinimumSize(300,300);
        setAlignment(Qt::AlignCenter);
        setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
        setText("Camera View");
    }

    void CameraView::mousePressEvent(QMouseEvent *e)
    {
        if (e->button() == Qt::LeftButton)
        {
            left_mousebutton_pressed_ = true;
            last_pos_= e->posF();
            if (!QApplication::overrideCursor())
                QApplication::changeOverrideCursor(Qt::SizeAllCursor);
            else
                QApplication::setOverrideCursor(Qt::SizeAllCursor);
            update_timer_.start();
        }
    }    

    void CameraView::mouseReleaseEvent(QMouseEvent *e)
    {
        if (e->button() == Qt::LeftButton)
        {
            left_mousebutton_pressed_ = false;
            last_pos_.setX(0);
            last_pos_.setY(0);
            if (QApplication::overrideCursor())
                QApplication::restoreOverrideCursor();
        }
    }

    void CameraView::wheelEvent(QWheelEvent *e)
    {
        emit Zoom(e->delta(), e->modifiers());
    }

    void CameraView::mouseMoveEvent(QMouseEvent *e)
    {
        if (left_mousebutton_pressed_)
        {
            if (update_timer_.elapsed() < 20)
                return;
            QPointF current_pos = e->posF();
            if (!last_pos_.isNull())
            {
                QPointF movement = current_pos-last_pos_;
                emit Move(movement.x()/width(), movement.y()/height());
                
                //emit Move(2*PI*movement.x()/width(),2*PI*movement.y()/height());
            }
            last_pos_ = current_pos;
            update_timer_.start();
        }
    }
}