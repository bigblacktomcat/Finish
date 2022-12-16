/**
\brief Описание диалогового класса NewScenarioDialog
Для нового сценария задаём имя, область действия
\date  2020-08-18
**/
#ifndef NEWSCENARIODIALOG_H
#define NEWSCENARIODIALOG_H

#include <QDialog>
#include <QPushButton>
#include "issocommondata.h"

namespace Ui {
    class NewScenarioDialog;
}

class NewScenarioDialog : public QDialog
{
        Q_OBJECT

    public:
        explicit NewScenarioDialog(QWidget *parent = 0);
        ~NewScenarioDialog();

        QString scenarioName() const;
        IssoScenarioScope scenarioScope() const;

    private:
        Ui::NewScenarioDialog *ui;
};

#endif // NEWSCENARIODIALOG_H
