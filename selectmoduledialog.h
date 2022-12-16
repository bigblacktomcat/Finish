#ifndef SELECTMODULEDIALOG_H
#define SELECTMODULEDIALOG_H

#include <QDialog>

namespace Ui {
    class SelectModuleDialog;
}

class SelectModuleDialog : public QDialog
{
        Q_OBJECT

    public:
        explicit SelectModuleDialog(const QStringList& moduleNames, QWidget *parent = 0);
        ~SelectModuleDialog();

        QString selectedModuleName();

    private:
        Ui::SelectModuleDialog *ui;
};

#endif // SELECTMODULEDIALOG_H
