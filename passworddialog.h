#ifndef PASSWORDDIALOG_H
#define PASSWORDDIALOG_H

#include <QDialog>

namespace Ui {
    class PasswordDialog;
}

class PasswordDialog : public QDialog
{
        Q_OBJECT

    private:
        Ui::PasswordDialog *ui;

    public:
        explicit PasswordDialog(QWidget *parent = 0);
        ~PasswordDialog();

        QString password() const;
        static bool inputPassword(QWidget *parent, const QString& correctPwd,
                                  const QString &errorMsg);
};

#endif // PASSWORDDIALOG_H
