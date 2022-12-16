#ifndef NEWCHAINCONFIGDIALOG_H
#define NEWCHAINCONFIGDIALOG_H

#include <QDialog>
#include <QButtonGroup>
#include <QPushButton>
#include "issocommondata.h"

namespace Ui {
    class NewChainConfigDialog;
}

class NewChainConfigDialog : public QDialog
{
        Q_OBJECT

    public:
        explicit NewChainConfigDialog(QWidget *parent = 0);
        ~NewChainConfigDialog();

        QString name() const;
        void setName(const QString& name);

        IssoChainType chainType() const;

        int sensorCount();
        void setSensorCount(int count);

    private:
        Ui::NewChainConfigDialog *ui;
};

#endif // NEWCHAINCONFIGDIALOG_H
