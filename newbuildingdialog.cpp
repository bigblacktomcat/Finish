/**
\brief Реализация класса NewBuildingDialog добавляем здание и его описание
\date  2020-08-18
**/

#include "newbuildingdialog.h"
#include "ui_newbuildingdialog.h"
#include <QPushButton>


NewBuildingDialog::NewBuildingDialog(QList<int> availableNumbers, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewBuildingDialog)
{
    ui->setupUi(this);
    // убрать кнопку помощи
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    // надписи на стандартных кнопках
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("ОК"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Отмена"));
    // заполнить список доступных номеров зданий
    foreach (int number, availableNumbers)
    {
        // добавить описание и номер здания
        ui->cbNumber->addItem(QString::number(number), QVariant::fromValue(number));
    }
}


NewBuildingDialog::~NewBuildingDialog()
{
    delete ui;
}


QString NewBuildingDialog::name() const
{
    return ui->leName->text();
}


int NewBuildingDialog::number() const
{
    return ui->cbNumber->currentData().toInt();
}
