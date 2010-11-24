// For conditions of distribution and use, see copyright notice in license.txt

#include "DebugOperatorNew.h"

#include "NaaliUi.h"
#include "NaaliMainWindow.h"
#include "NaaliGraphicsView.h"
#include "Framework.h"

#include <QEvent>
#include <QLayout>
#include <QVBoxLayout>
#include <QScrollBar>

#include "MemoryLeakCheck.h"

/// The SuppressedPaintWidget is used as a viewport for the main Naali QGraphicsView. Its purpose is
/// to disable all automatic drawing of the QGraphicsView to screen so that we can composit an Ogre
/// 3D render with the Qt widgets added to a QGraphicsScene.
class SuppressedPaintWidget : public QWidget {
public:
    SuppressedPaintWidget(QWidget *parent = 0, Qt::WindowFlags f = 0);
    virtual ~SuppressedPaintWidget() {}

protected:
    virtual bool event(QEvent *event);
    virtual void paintEvent(QPaintEvent *event);
};

SuppressedPaintWidget::SuppressedPaintWidget(QWidget *parent, Qt::WindowFlags f)
:QWidget(parent, f)
{
    setAttribute(Qt::WA_PaintOnScreen);
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_OpaquePaintEvent, true);
}

bool SuppressedPaintWidget::event(QEvent *event)
{
    if (event->type() == QEvent::UpdateRequest)
        return true;

    if (event->type() == QEvent::Paint)
        return true;

    if (event->type() == QEvent::Wheel)
        return true;

    if (event->type() == QEvent::Resize)
        return true;

    return QWidget::event(event);
}

void SuppressedPaintWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
}


NaaliUi::NaaliUi(Foundation::Framework *owner_)
:owner(owner_)
{
    mainWindow = new NaaliMainWindow(owner);
    mainWindow->setAutoFillBackground(false);
//    mainWindow->setUpdatesEnabled(false);

    // Apply the Naali main window icon. \todo use .ico file type
    QIcon icon("./data/ui/images/naali_icon.png");
    mainWindow->setWindowIcon(icon);

    graphicsView = new NaaliGraphicsView(mainWindow);

    ///\todo Memory leak below, see very end of ~Renderer() for comments.
    QVBoxLayout *layout = new QVBoxLayout(mainWindow);
    mainWindow->setLayout(layout);
    mainWindow->layout()->setMargin(0);
    layout->setContentsMargins(0,0,0,0);
    mainWindow->layout()->addWidget(graphicsView);

    QWidget *viewportWidget = new SuppressedPaintWidget();
    graphicsView->setViewport(viewportWidget);
    viewportWidget->setAttribute(Qt::WA_DontShowOnScreen, true);
    viewportWidget->setGeometry(0, 0, graphicsView->width(), graphicsView->height());
    viewportWidget->setContentsMargins(0,0,0,0);

    mainWindow->setContentsMargins(0,0,0,0);
    graphicsView->setContentsMargins(0,0,0,0);
    
    graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    graphicsView->horizontalScrollBar()->setValue(0);
    graphicsView->horizontalScrollBar()->setRange(0, 0);

    graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    graphicsView->verticalScrollBar()->setValue(0);
    graphicsView->verticalScrollBar()->setRange(0, 0);

    // Setup Qts mainwindow with title and geometry
//    mainWindow->setWindowTitle(QString(window_title_.c_str()));
//    mainWindow->setGeometry(window_left, window_top, width, height);
//    if (maximized)
//        mainWindow->showMaximized();

    graphicsScene = new QGraphicsScene(this);

    graphicsView->setScene(graphicsScene);
    graphicsView->scene()->setSceneRect(graphicsView->rect());
    connect(graphicsScene, SIGNAL(changed(const QList<QRectF> &)), graphicsView, SLOT(HandleSceneChanged(const QList<QRectF> &))); 

    connect(mainWindow, SIGNAL(WindowResizeEvent(int,int)), graphicsView, SLOT(Resize(int,int))); 

    mainWindow->LoadWindowSettingsFromFile();
    graphicsView->Resize(mainWindow->width(), mainWindow->height());

    graphicsView->show();
    mainWindow->show();
    viewportWidget->show();

    /// Do a full repaint of the view now that we've shown it.
    graphicsView->MarkViewUndirty();
}

NaaliUi::~NaaliUi()
{
}

NaaliMainWindow *NaaliUi::MainWindow() const
{
    return mainWindow;
}

NaaliGraphicsView *NaaliUi::GraphicsView() const
{
    return graphicsView;
}

QGraphicsScene *NaaliUi::GraphicsScene() const
{
    return graphicsScene;
}
