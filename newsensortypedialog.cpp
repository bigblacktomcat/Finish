#include "newsensortypedialog.h"
#include "ui_newsensortypedialog.h"
#include <QPushButton>


NewSensorTypeDialog::NewSensorTypeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewSensorTypeDialog)
{
    ui->setupUi(this);
    // убрать кнопку помощи
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    // надписи на стандартных кнопках
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("ОК"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Отмена"));
}


NewSensorTypeDialog::~NewSensorTypeDialog()
{
    delete ui;
}


QString NewSensorTypeDialog::sensorTypeName() const
{
    return ui->leName->text();
}


float NewSensorTypeDialog::minValue() const
{
    return ui->dsbMin->value();
}


float NewSensorTypeDialog::maxValue() const
{
    return ui->dsbMax->value();
}
