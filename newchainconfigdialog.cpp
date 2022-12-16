#include "newchainconfigdialog.h"
#include "ui_newchainconfigdialog.h"


NewChainConfigDialog::NewChainConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewChainConfigDialog)
{
    ui->setupUi(this);
    // убрать кнопку помощи
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    // надписи на стандартных кнопках
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("ОК"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Отмена"));
    //
    // сформировать группу радиокнопок
    QButtonGroup* radioGroup = new QButtonGroup(this);
    radioGroup->addButton(ui->rbtnSmoke, SMOKE_CHAIN);
    radioGroup->addButton(ui->rbtnMulticriterial, MULTICRITERIAL);
    radioGroup->addButton(ui->rbtnAlarmButton, ALARM_BUTTON_CHAIN);
    radioGroup->addButton(ui->rbtnMoveDetector, MOVE_DETECTOR_CHAIN);
    radioGroup->addButton(ui->rbtnTemp, TEMP_CHAIN);
}


NewChainConfigDialog::~NewChainConfigDialog()
{
    delete ui;
}


QString NewChainConfigDialog::name() const
{
    return ui->leName->text();
}


void NewChainConfigDialog::setName(const QString &name)
{
    ui->leName->setText(name);
}


IssoChainType NewChainConfigDialog::chainType() const
{
    return static_cast<IssoChainType>(ui->rbtnSmoke->group()->checkedId());
}


int NewChainConfigDialog::sensorCount()
{
    return ui->sbSensorCount->value();
}


void NewChainConfigDialog::setSensorCount(int count)
{
    ui->sbSensorCount->setValue(count);
}
