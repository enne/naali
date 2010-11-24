// For conditions of distribution and use, see copyright notice in license.txt

#include "StableHeaders.h"
#include "DebugOperatorNew.h"

#include "LibraryWidget.h"

#include <QGraphicsScene>
#include <QDebug>

#include "MemoryLeakCheck.h"


namespace Library
{

    LibraryWidget::LibraryWidget(QGraphicsView *ui_view) : 
        QWidget(),
        ui_view_(ui_view)
    {
        setupUi(this);        

        connect(urlPushButton, SIGNAL(clicked()), this, SLOT(SetWebViewUrl()));
        connect(pushButtonStop, SIGNAL(clicked()), this, SLOT(StopDownload()));

        setMouseTracking(true);
        setAcceptDrops(true);
        webView->setMouseTracking(true);
        webView->setAcceptDrops(true);
        webView->setUrl(QUrl("http://www.realxtend.org/rexlib/"));
        urlLineEdit->setText("http://www.realxtend.org/rexlib/");

        pushButtonStop->hide();
    }

    LibraryWidget::~LibraryWidget()
    {
        if (webView)
            webView->stop();
    }

    void LibraryWidget::SetWebViewUrl()
    {          
        QString url = urlLineEdit->text();
        if (url.isEmpty())
            return;
        webView->setUrl(QUrl(url));        
    }

    void LibraryWidget::StopDownload()
    {
        stopDownload = true;
    }

    void LibraryWidget::SetInfoText(const QString text)
    {
        labelInfo->setText(text);
    }

    void LibraryWidget::HideStopButton()
    {
        pushButtonStop->hide();
    }

    void LibraryWidget::ShowStopButton()
    {
        pushButtonStop->show();
    }
}
