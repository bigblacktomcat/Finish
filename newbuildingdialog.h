#ifndef NEWBUILDINGDIALOG_H
#define NEWBUILDINGDIALOG_H

#include <QDialog>

namespace Ui {
    class NewBuildingDialog;
}

class NewBuildingDialog : public QDialog
{
        Q_OBJECT

    public:
        explicit NewBuildingDialog(QList<int> availableNumbers, QWidget *parent = 0);
        ~NewBuildingDialog();

        QString name() const;
        int number() const;

    private:
        Ui::NewBuildingDialog *ui;
};

#endif // NEWBUILDINGDIALOG_H
