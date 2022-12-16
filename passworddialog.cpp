#include "passworddialog.h"
#include "ui_passworddialog.h"
#include <QMessageBox>
#include <QPushButton>
#include <QDebug>


PasswordDialog::PasswordDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PasswordDialog)
{
    ui->setupUi(this);
    // убрать кнопку помощи
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    // надписи на стандартных кнопках
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("ОК"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Отмена"));
}


PasswordDialog::~PasswordDialog()
{
    delete ui;
}


QString PasswordDialog::password() const
{
    return ui->lePassword->text();
}


bool PasswordDialog::inputPassword(QWidget* parent, const QString &correctPwd,
                                   const QString& errorMsg)
{
    PasswordDialog dlg(parent);
    bool ok = (dlg.exec() == QDialog::Accepted);
    QString pwd = dlg.password();
    // если пользователь нажал ok, но пароль неверный
    if ((ok) && (pwd != correctPwd))
    {
        QMessageBox::critical(parent, tr("Ошибка"), errorMsg);
    }
    return ((ok) && (pwd == correctPwd));
}
