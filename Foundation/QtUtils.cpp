/**
 *  For conditions of distribution and use, see copyright notice in license.txt
 *
 *  @file   QtUtils.cpp
 *  @brief  Cross-platform utility functions using Qt.
 */

#include "StableHeaders.h"
#include "QtUtils.h"

#include <QString>
#include <QFileDialog>
#include <QDir>
#include <QCloseEvent>
#include <QGraphicsProxyWidget>

namespace QtUtils
{

class CustomFileDialog : public QFileDialog
{
public:
    CustomFileDialog(QWidget* parent, const QString& caption, const QString& dir, const QString& filter) :
        QFileDialog(parent, caption, dir, filter)
    {
    }

protected:
    virtual void hideEvent(QHideEvent* e)
    {
        if (e->type() == QEvent::Hide)
        {
            emit finished(0);
            deleteLater();
        }
    }
    virtual void closeEvent(QCloseEvent* e)
    {
        if (e->type() == QEvent::Hide)
        {
            emit finished(0);
        }
    }
};

QFileDialog* OpenFileDialogNonModal(
    const QString& filter,
    const QString& caption,
    const QString& dir,
    QWidget* parent,
    QObject* initiator,
    const char* slot)
{
    QFileDialog* dialog = new CustomFileDialog(parent, caption, dir, filter);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    QObject::connect(dialog, SIGNAL(finished(int)), initiator, slot);
    dialog->show();
    dialog->resize(500, 300);

    if (dialog->graphicsProxyWidget())
        dialog->graphicsProxyWidget()->setWindowTitle(caption);

    return dialog;
}

QFileDialog *SaveFileDialogNonModal(
    const QString& filter,
    const QString& caption,
    const QString& dir,
    QWidget* parent,
    QObject* initiator,
    const char* slot)
{
    QFileDialog* dialog = new CustomFileDialog(parent, caption, dir, filter);
    dialog->setFileMode(QFileDialog::AnyFile);
    dialog->setAcceptMode(QFileDialog::AcceptSave);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    QObject::connect(dialog, SIGNAL(finished(int)), initiator, slot);
    dialog->show();
    dialog->resize(500, 300);

    if (dialog->graphicsProxyWidget())
        dialog->graphicsProxyWidget()->setWindowTitle(caption);

    return dialog;
}

std::string GetOpenFileName(
    const std::string &filter,
    const std::string &caption,
    const std::string &dir)
{
    QString qcaption(caption.c_str());
    QString qdir(dir.c_str());
    QString qfilter(filter.c_str());
    QWidget *parent = 0;

    QString filename = QFileDialog::getOpenFileName(parent, qcaption, qdir, qfilter);

    return filename.toStdString();
}

std::string GetSaveFileName(
    const std::string &filter,
    const std::string &caption,
    const std::string &dir)
{
    QString qcaption(caption.c_str());
    QString qdir(dir.c_str());
    QString qfilter(filter.c_str());
    QWidget *parent = 0;

    QString filename = QFileDialog::getSaveFileName(parent, qcaption, qdir, qfilter);

    return filename.toStdString();
}

StringList GetOpenFileNames(
    const std::string &filter,
    const std::string &caption,
    const std::string &dir)
{
    QString qcaption(caption.c_str());
    QString qdir(dir.c_str());
    QString qfilter(filter.c_str());
    QWidget *parent = 0;
    StringList filelist;

    QStringList filenames = QFileDialog::getOpenFileNames(parent, qcaption, qdir, qfilter);

    // Convert QStringList to std::list<std::string> list.
    for(QStringList::iterator q_it = filenames.begin(); q_it != filenames.end(); ++q_it)
        filelist.push_back(q_it->toStdString());

    return filelist;
}

StringList GetOpenRexFileNames(const std::string &dir)
{
    QString qfilter(
        "Images (*.tga; *.bmp; *.jpg; *.jpeg; *.png);;"
        "Sounds (*.ogg; *.wav);;"
        "Ogre 3D Models (*.mesh);;"
        "Ogre Particle Scripts (*.particle);;"
        "Ogre Skeleton (*.skeleton);;"
        "Ogre Material (*.material);;"
        "Flash Animation (*.swf);;"
        "All Files (*.*)");
    QString qcaption("Open");
    QString qdir(dir.c_str());
    QWidget *parent = 0;
    StringList filelist;

    QStringList filenames = QFileDialog::getOpenFileNames(parent, qcaption, qdir, qfilter);

    // Convert QStringList to std::list<std::string> list.
    for(QStringList::iterator q_it = filenames.begin(); q_it != filenames.end(); ++q_it)
        filelist.push_back(q_it->toStdString());

    return filelist;
}

QStringList GetOpenRexFilenames(const std::string &dir)
{
    QString qfilter(
        "Images (*.tga; *.bmp; *.jpg; *.jpeg; *.png);;"
        "Sounds (*.ogg; *.wav);;"
        "Ogre 3D Models (*.mesh);;"
        "Ogre Particle Scripts (*.particle);;"
        "Ogre Skeleton (*.skeleton);;"
        "Ogre Material (*.material);;"
        "Flash Animation (*.swf);;"
        "All Files (*.*)");
    QString qcaption("Open");
    QString qdir(dir.c_str());
    QWidget *parent = 0;

    return QFileDialog::getOpenFileNames(parent, qcaption, qdir, qfilter);
}

std::string GetCurrentPath()
{
    return QDir::currentPath().toStdString();
}

}
