#include "newrelayclassdialog.h"
#include "ui_newrelayclassdialog.h"


NewRelayClassDialog::NewRelayClassDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewRelayClassDialog)
{
    ui->setupUi(this);
    // убрать кнопку помощи
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    // надписи на стандартных кнопках
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("ОК"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Отмена"));
}


NewRelayClassDialog::~NewRelayClassDialog()
{
    delete ui;
}


QString NewRelayClassDialog::relayClassName() const
{
    return ui->leClassName->text();
}


QString NewRelayClassDialog::description() const
{
    return ui->leDescription->text();
}


int NewRelayClassDialog::delay() const
{
    return ui->sbDelay->value();
}
