/**
 *  For conditions of distribution and use, see copyright notice in license.txt
 *
 *  @file   UiProxyWidget.cpp
  */

#include "StableHeaders.h"
#include "DebugOperatorNew.h"
#include "UiWidget.h"
#include "MemoryLeakCheck.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QVariant>

UiWidget::UiWidget(QWidget *widget, QWidget *parent, Qt::WindowFlags flags, const QStringList &params):
QWidget(parent, flags),
widget_(widget)
{
    setWindowTitle(widget_->windowTitle());

    QHBoxLayout* h_box = new QHBoxLayout();
    h_box->addWidget(widget_);
    h_box->setContentsMargins(0,0,0,0);
    
    QVBoxLayout* v_box = new QVBoxLayout();
    v_box->addLayout(h_box);
    v_box->setContentsMargins(0,0,0,0);
    setLayout(v_box);

    //Inherit dynamic properties of embedded widget
    QListIterator<QByteArray> it(widget_->dynamicPropertyNames());
    while(it.hasNext())
    {
        QString propertyName = it.next();
        if (propertyName.isNull() || propertyName.isEmpty())
            continue;
        setProperty(propertyName.toStdString().c_str(), widget_->property(propertyName.toStdString().c_str()));
    }

    //Set parameters as dynamic properties
    int i = 1;
    while (i < params.size())
    {
        QString propertyName = params.at(i-1);
        if (propertyName.isNull() || propertyName.isEmpty())
            continue;
        setProperty(propertyName.toStdString().c_str(), QVariant::fromValue(params.at(i)));
        i += 2;
    }
}

UiWidget::~UiWidget(void)
{
}

void UiWidget::setVisible(bool visible)
{
    if (isVisible() != visible)
    {
        QWidget::setVisible(visible);
        sendVisibilityChanged(visible);
    }
}

void UiWidget::toggleVisibility()
{
    setVisible(!isVisible());
}

void UiWidget::sendVisibilityChanged(bool visible)
{
    emit visibilityChanged(visible);
}