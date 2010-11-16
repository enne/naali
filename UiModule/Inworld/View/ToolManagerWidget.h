// For conditions of distribution and use, see copyright notice in license.txt

#ifndef incl_UiModule_ToolManagerWidget_h
#define incl_UiModule_ToolManagerWidget_h

#include "ui_ToolContainerWidget.h"
#include "ui_ToolManagerWidget.h"
#include <QObject>
#include <QList>
#include <QString>

namespace CoreUi
{
    class IToolWidget;

    class ToolContainerWidget : public QWidget, private Ui::toolContainerWidget
    {
        Q_OBJECT
    public:
        ToolContainerWidget(QString name, IToolWidget* tool_widget);
        ~ToolContainerWidget();
        QString GetName();
        IToolWidget* GetToolWidget();
    public slots:

    private:
        QString name_;
        IToolWidget* tool_widget_;
    };

    /**
     * Show all tool widgets at the bottom of main screen using ToolContainerWidget objects
     */
    class ToolManagerWidget : public QWidget, private Ui::toolManagerWidget
    {
        Q_OBJECT
    public:

        ToolManagerWidget();
        virtual ~ToolManagerWidget();

    public slots:
        /// IToolManager interface
        virtual void AddToolWidget(QString name, IToolWidget* widget);

        /// IToolManager interface
        virtual void RemoveToolWidget(IToolWidget* widget);

        /// IToolManager interface
        virtual void RemoveAllToolWidgets();

    private:
        QList<ToolContainerWidget*> tool_widgets_;

    signals:
        void ToolWidgetAdded(IToolWidget* widget);
        void ToolRemoved(IToolWidget* widget);
    };

} // namespace CoreUi

#endif // incl_UiModule_ToolManagerWidget_h
