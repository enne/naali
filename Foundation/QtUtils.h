/**
 *  For conditions of distribution and use, see copyright notice in license.txt
 *
 *  @file   QtUtils.h
 *  @brief  Cross-platform utility functions using Qt.
 */

#ifndef incl_Foundation_QtUtils_h
#define incl_Foundation_QtUtils_h

#include "CoreTypes.h"
#include <QStringList>

class QFileDialog;

namespace QtUtils
{
    /// Opens a non-modal file dialog
    /** @param filter The files to be shown.
        @param caption Dialog's caption
        @param dir Working directory.
        @param parent Parent widget. If you give parent widget, the file dialog will be part of the Naali UI scene.
            Give 0 if you want the dialog to be external.
        @param initiator QObject that initiated the dialog and wants the results
        @param slot Slot on initiator object, to which dialog's finished() signal will be 
               connected. Result value will be 1 if OK was pressed. Use sender() and dynamic 
               cast to QFileDialog to get to know the chosen file(s)
        @return The created file dialog
    */
    QFileDialog* OpenFileDialogNonModal(
        const QString& filter,
        const QString& caption,
        const QString& dir,
        QWidget* parent,
        QObject* initiator,
        const char* slot);

    /// Opens a non-modal file save dialog
    /** @param filter The files to be shown.
        @param caption Dialog's caption
        @param dir Working directory.
        @param parent Parent widget. If you give parent widget, the file dialog will be part of the Naali UI scene.
            Give 0 if you want the dialog to be external.
        @param initiator QObject that initiated the dialog and wants the results
        @param slot Slot on initiator object, to which dialog's finished() signal will be 
               connected. Result value will be 1 if OK was pressed. Use sender() and dynamic 
               cast to QFileDialog to get to know the chosen file(s)
        @return The created file dialog
    */
    QFileDialog* SaveFileDialogNonModal(
        const QString& filter,
        const QString& caption,
        const QString& dir,
        QWidget* parent,
        QObject* initiator,
        const char* slot);

    /// Opens the OS's open file dialog.
    /** @param filter The files to be shown.
        @param caption Dialog's caption.
        @param dir Working directory.
        @return The absolute path to the chosen file.
    */
    std::string GetOpenFileName(
        const std::string &filter,
        const std::string &caption,
        const std::string &dir);

    /// Opens the OS's save file dialog.
    /** @param filter The files to be shown.
        @param caption Dialog's caption.
        @param dir Working directory.
        @return The absolute path to the chosen file.
    */
    std::string GetSaveFileName(
        const std::string &filter,
        const std::string &caption,
        const std::string &dir);

    /// Same as GetOpenFileName but for multiple files.
    /** @return List of absolute paths to the chosen files.
    */
    StringList GetOpenFileNames(
        const std::string &filter,
        const std::string &caption,
        const std::string &dir);

    /// Same as GetOpenFileNames but with hardcoded reX filters.
    /** @return List of absolute paths to the chosen files.
    */
    StringList GetOpenRexFileNames(const std::string &dir);

    /// Same as GetOpenFileNames but return QStringList instead of std::list<std::string>.
    QStringList GetOpenRexFilenames(const std::string &dir);

    /// @return The absolute path of the application's current directory.
    std::string GetCurrentPath();
};

#endif
