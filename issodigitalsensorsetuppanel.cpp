#include "issodigitalsensorsetuppanel.h"
#include "ui_issodigitalsensorsetuppanel.h"


IssoDigitalSensorSetupPanel::IssoDigitalSensorSetupPanel(QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::IssoDigitalSensorSetupPanel)
{
    ui->setupUi(this);
    // заполнить список условий
    ui->cbCondition->addItem(tr("ИЛИ"), QVariant(CONDITION_OR));
    ui->cbCondition->addItem(tr("И"), QVariant(CONDITION_AND));
}


IssoDigitalSensorSetupPanel::~IssoDigitalSensorSetupPanel()
{
    delete ui;
}


//int IssoDigitalSensorSetupPanel::normalValue() const
//{
//    return ui->sbNormal->value();
//}


//void IssoDigitalSensorSetupPanel::setNormalValue(int value)
//{
//    ui->sbNormal->setValue(value);
//}


int IssoDigitalSensorSetupPanel::warningValue() const
{
    return ui->sbWarning->value();
}


void IssoDigitalSensorSetupPanel::setWarningValue(int value)
{
    ui->sbWarning->setValue(value);
}


int IssoDigitalSensorSetupPanel::alarmValue() const
{
    return ui->sbAlarm->value();
}


void IssoDigitalSensorSetupPanel::setAlarmValue(int value)
{
    ui->sbAlarm->setValue(value);
}


IssoStateCondition IssoDigitalSensorSetupPanel::condition() const
{
    return (IssoStateCondition)ui->cbCondition->currentData().toInt();
}


void IssoDigitalSensorSetupPanel::setCondition(IssoStateCondition condition)
{
    int index = ui->cbCondition->findData(condition);
    if (index != -1)
        ui->cbCondition->setCurrentIndex(index);
}


QMap<IssoState, int> IssoDigitalSensorSetupPanel::stateValues() const
{
    QMap<IssoState, int> map;
//    map.insert(STATE_NORMAL, normalValue());
    map.insert(STATE_WARNING, warningValue());
    map.insert(STATE_ALARM, alarmValue());
    return map;
}


void IssoDigitalSensorSetupPanel::setStateValues(QMap<IssoState, int> stateValueMap)
{
//    setNormalValue(stateValueMap.value(STATE_NORMAL, 0));
    setWarningValue(stateValueMap.value(STATE_WARNING, 0));
    setAlarmValue(stateValueMap.value(STATE_ALARM, 0));
}
