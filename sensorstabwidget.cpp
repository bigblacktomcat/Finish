#include "sensorstabwidget.h"
#include "newsensortypedialog.h"
#include "ui_sensorstabwidget.h"

#include <QMessageBox>



SensorsTabWidget::SensorsTabWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SensorsTabWidget)
{
    ui->setupUi(this);
    initEventHandlers();
}

SensorsTabWidget::~SensorsTabWidget()
{
    delete ui;
}


void SensorsTabWidget::setObject(IssoObject *obj)
{
    m_object = obj;
    // если объект пуст, выход
    if (!m_object)
        return;
//    // отобразить настройки типов фиксированных датчиков:
//    //
//    // Дым
//    displayFixedSensorTypeSettings(SENSOR_SMOKE);
//    // Угарный газ
//    displayFixedSensorTypeSettings(SENSOR_CO);
//    // Вибрация
//    displayFixedSensorTypeSettings(VIBRATION);
//    // Кнопка Пожар
//    displayFixedSensorTypeSettings(BTN_FIRE);
//    // Кнопка Охрана
//    displayFixedSensorTypeSettings(BTN_SECURITY);
//    // Внутренняя температура
//    displayFixedSensorTypeSettings(VOLT_SENSORS);
//    // Внешняя температура
//    displayFixedSensorTypeSettings(TEMP_OUT);
//    // Питание (внутреннее питание по шине)
//    displayFixedSensorTypeSettings(VOLT_BUS);
//    // АКБ (внешнее питание)
//    displayFixedSensorTypeSettings(VOLT_BATTERY);
//    //
//    // заполнить список изменяемых датчиков
//    //
//    ui->listUnfixedSensorTypes->clear();
//    ui->listUnfixedSensorTypes->addItems(m_object->unfixedSensorTypeNames());
//    // если список не пуст, выбрать первый элемент
//    if (ui->listUnfixedSensorTypes->count() > 0)
//        ui->listUnfixedSensorTypes->item(0)->setSelected(true);
}


void SensorsTabWidget::initEventHandlers()
{
    connect(ui->btnAddUnfixedSensor,    SIGNAL(clicked(bool)),
            this,                       SLOT(processAddUnfixedSensorBtnClicked()));
    connect(ui->btnRemoveUnfixedSensor, SIGNAL(clicked(bool)),
            this,                       SLOT(processRemoveUnfixedSensorBtnClicked()));
    connect(ui->btnSaveFixedTypes,      SIGNAL(clicked(bool)),
            this,                       SLOT(processSaveFixedTypeBtnClicked()));
    connect(ui->btnSaveUnfixedType,     SIGNAL(clicked(bool)),
            this,                       SLOT(processSaveUnfixedTypeBtnClicked()));
    connect(ui->listUnfixedSensorTypes, SIGNAL(itemSelectionChanged()),
            this,                       SLOT(processUnfixedSensorTypesSelectionChanged()));
}


void SensorsTabWidget::processAddUnfixedSensorBtnClicked()
{
//    if (!m_object)
//        return;
//    // вывести диалог добавления изменяемых датчиков
//    NewSensorTypeDialog dlg(this);
//    int result = dlg.exec();
//    // если ОК
//    if (result == QDialog::Accepted)
//    {
//        // создать тип датчика
//        IssoSensorType sensorType(dlg.sensorTypeName(),
//                                  dlg.minValue(),
//                                  dlg.maxValue(),
//                                  false);
//        // добавить тип датчика в список типов датчиков системы
//        m_object->insertSensorType(sensorType);
//        // добавить имя типа в визуальный список
//        ui->listUnfixedSensorTypes->addItem(sensorType.name());
//        // выбрать добавленный элемент
//        QList<QListWidgetItem*> list =
//                ui->listUnfixedSensorTypes->findItems(sensorType.name(), Qt::MatchExactly);
//        if (!list.isEmpty())
//            list.first()->setSelected(true);
//    }
}


void SensorsTabWidget::processRemoveUnfixedSensorBtnClicked()
{
//    if (!m_object)
//        return;
//    // если тип изменяемого датчика не выбран, выход
//    if (ui->listUnfixedSensorTypes->selectedItems().isEmpty())
//         return;
//    // получить выбранный элемент визуального списка
//    QListWidgetItem* typeItem = ui->listUnfixedSensorTypes->selectedItems().first();
//    // удалить из списка типов датчиков системы
//    m_object->removeSensorType(typeItem->text());
//    // удалить элемент из визуального списка
//    ui->listUnfixedSensorTypes->removeItemWidget(typeItem);
//    // удалить изменяемый тип датчиков из списка выбранных для каждого модуля
//    foreach (IssoModule* module, m_object->modules())
//    {
////        module->removeUnfixedSensorTypesByName(typeItem->text());
//    }
//    // уничтожить удаленный элемент
//    delete typeItem;
}


void SensorsTabWidget::processSaveFixedTypeBtnClicked()
{
    // Дым
    updateFixedSensorTypeSettings(SENSOR_SMOKE);
    // Угарный газ
    updateFixedSensorTypeSettings(SENSOR_CO);
    // Вибрация
    updateFixedSensorTypeSettings(VIBRATION);
    // Кнопка Пожар
    updateFixedSensorTypeSettings(BTN_FIRE);
    // Кнопка Охрана
    updateFixedSensorTypeSettings(BTN_SECURITY);
    // Внутренняя температура
    updateFixedSensorTypeSettings(VOLT_SENSORS);
    // Внешняя температура
    updateFixedSensorTypeSettings(TEMP_OUT);
    // Питание (внутреннее питание по шине)
    updateFixedSensorTypeSettings(VOLT_BUS);
    // АКБ (внешнее питание)
    updateFixedSensorTypeSettings(VOLT_BATTERY);
    //
    // сообщить об изменении настроек фиксированных типов датчиков
    QMessageBox::information(this, tr("Информация"),
                             tr("Настройки фиксированных типов датчиков сохранены"));
}


void SensorsTabWidget::processSaveUnfixedTypeBtnClicked()
{
//    if (!m_object)
//        return;
//    // если тип изменяемого датчика не выбран, выход
//    if (ui->listUnfixedSensorTypes->selectedItems().isEmpty())
//         return;
//    // получить выбранный элемент визуального списка
//    QListWidgetItem* typeItem = ui->listUnfixedSensorTypes->selectedItems().first();
//    // текущее имя типа датчика
//    QString oldTypeName = typeItem->text();
//    // новые характеристики типа датчика
//    QString newTypeName = ui->leUnfixedSensorTypeName->text();
//    float minValue = ui->dsbUnfixedSensorMin->value();
//    float maxValue = ui->dsbUnfixedSensorMax->value();
//    // создать тип датчика с новыми характеристиками
//    IssoSensorType sensorType(newTypeName, minValue, maxValue, false);

//    // если имя типа изменилось, обновить имена типов датчиков в каждом модуле
//    m_object->updateUnfixedSensorType(oldTypeName, sensorType);

//    // изменить текст элемента визуального списка
//    typeItem->setText(newTypeName);
//    //
//    // сообщить об изменении настроек изменяемого типа датчиков
//    QMessageBox::information(this, tr("Информация"),
//                             tr("Настройки изменяемого типа датчиков <%1> сохранены")
//                             .arg(newTypeName));
}


void SensorsTabWidget::processUnfixedSensorTypesSelectionChanged()
{
//    if (!m_object)
//        return;

//    QString typeName = "";
//    float minVal = 0;
//    float maxVal = 0;
//    // если тип изменяемого датчика выбран
//    if (!ui->listUnfixedSensorTypes->selectedItems().isEmpty())
//    {
//        // получить выбранный элемент визуального списка
//        QListWidgetItem* typeItem = ui->listUnfixedSensorTypes->selectedItems().first();
//        // получить тип датчика по имени типа
//        bool found;
//        IssoSensorType sensorType = m_object->findSensorType(typeItem->text(), found);
//        // если тип найден, вывести его данные
//        if (found)
//        {
//            typeName = sensorType.name();
//            minVal = sensorType.minValue().toFloat();
//            maxVal = sensorType.maxValue().toFloat();
//        }
//    }
//    // обновить значения полей
//    ui->leUnfixedSensorTypeName->setText(typeName);
//    ui->dsbUnfixedSensorMin->setValue(minVal);
//    ui->dsbUnfixedSensorMax->setValue(maxVal);
}


void SensorsTabWidget::displayFixedSensorTypeSettings(IssoParamId paramId)
{
//    if (!m_object)
//        return;
//    // имя типа датчика
//    QString typeName;
//    // имя типа датчика
//    typeName = IssoCommonData::stringByParamId(paramId);
//    // найти тип датчика по имени
//    bool ok;
//    IssoSensorType sensorType = m_object->findSensorType(typeName, ok);
//    if (ok)
//    {
//        // отобразить диапазон штатных значений
//        setFixedSensorTypeGroupBoxValues(paramId, sensorType.minValue().toFloat(),
//                                                sensorType.maxValue().toFloat());
//    }
}


void SensorsTabWidget::updateFixedSensorTypeSettings(IssoParamId paramId)
{
//    if (!m_object)
//        return;
//    // имя типа датчика
//    QString sensorTypeName;
//    // диапазон штатных значений
//    float minValue, maxValue;
//    // получить имя типа датчика по параметру
//    sensorTypeName = IssoCommonData::stringByParamId(paramId);
//    // получить диапазон штатных значений
//    getFixedSensorTypeGroupBoxValues(paramId, minValue, maxValue);
//    // обновить тип датчика
//    m_object->insertSensorType(IssoSensorType(sensorTypeName, minValue, maxValue));
}


void SensorsTabWidget::getFixedSensorTypeGroupBoxValues(IssoParamId paramId,
                                                        float &minValue, float &maxValue)
{
    // найти визуальные компоненты, соответствующие параметру
    QDoubleSpinBox* spinMin = nullptr;
    QDoubleSpinBox* spinMax = nullptr;
    switch(paramId)
    {
        case BTN_FIRE:
        {
            spinMin = ui->dsbBtnFireMin;
            spinMax = ui->dsbBtnFireMax;
            break;
        }
        case BTN_SECURITY:
        {
            spinMin = ui->dsbBtnSecurityMin;
            spinMax = ui->dsbBtnSecurityMax;
            break;
        }
        case SENSOR_SMOKE:
        {
            spinMin = ui->dsbSensorSmokeMin;
            spinMax = ui->dsbSensorSmokeMax;
            break;
        }
        case SENSOR_CO:
        {
            spinMin = ui->dsbSensorCOMin;
            spinMax = ui->dsbSensorCOMax;
            break;
        }
        case VOLT_BATTERY:
        {
            spinMin = ui->dsbVoltBatteryMin;
            spinMax = ui->dsbVoltBatteryMax;
            break;
        }
        case VOLT_BUS:
        {
            spinMin = ui->dsbVoltBusMin;
            spinMax = ui->dsbVoltBusMax;
            break;
        }
        case VOLT_SENSORS:
        {
            spinMin = ui->dsbTempInMin;
            spinMax = ui->dsbTempInMax;
            break;
        }
        case TEMP_OUT:
        {
            spinMin = ui->dsbTempOutMin;
            spinMax = ui->dsbTempOutMax;
            break;
        }
        case VIBRATION:
        {
            spinMin = ui->dsbVibrationMin;
            spinMax = ui->dsbVibrationMax;
            break;
        }
        default:
            break;
    }
    // получить характеристики типа датчика из компонентов
    if (spinMin && spinMax)
    {
        // минимальное штатное значение
        minValue = spinMin->value();
        // максимальное штатное значение
        maxValue = spinMax->value();
    }
}


void SensorsTabWidget::setFixedSensorTypeGroupBoxValues(IssoParamId paramId,
                                                        float minValue, float maxValue)
{
    // найти визуальные компоненты, соответствующие параметру
    QDoubleSpinBox* spinMin = nullptr;
    QDoubleSpinBox* spinMax = nullptr;
    switch(paramId)
    {
        case BTN_FIRE:
        {
            spinMin = ui->dsbBtnFireMin;
            spinMax = ui->dsbBtnFireMax;
            break;
        }
        case BTN_SECURITY:
        {
            spinMin = ui->dsbBtnSecurityMin;
            spinMax = ui->dsbBtnSecurityMax;
            break;
        }
        case SENSOR_SMOKE:
        {
            spinMin = ui->dsbSensorSmokeMin;
            spinMax = ui->dsbSensorSmokeMax;
            break;
        }
        case SENSOR_CO:
        {
            spinMin = ui->dsbSensorCOMin;
            spinMax = ui->dsbSensorCOMax;
            break;
        }
        case VOLT_BATTERY:
        {
            spinMin = ui->dsbVoltBatteryMin;
            spinMax = ui->dsbVoltBatteryMax;
            break;
        }
        case VOLT_BUS:
        {
            spinMin = ui->dsbVoltBusMin;
            spinMax = ui->dsbVoltBusMax;
            break;
        }
        case VOLT_SENSORS:
        {
            spinMin = ui->dsbTempInMin;
            spinMax = ui->dsbTempInMax;
            break;
        }
        case TEMP_OUT:
        {
            spinMin = ui->dsbTempOutMin;
            spinMax = ui->dsbTempOutMax;
            break;
        }
        case VIBRATION:
        {
            spinMin = ui->dsbVibrationMin;
            spinMax = ui->dsbVibrationMax;
            break;
        }
        default:
            break;
    }
    // отобразить характеристики типа датчика
    if (spinMin && spinMax)
    {
        // минимальное штатное значение
        spinMin->setValue(minValue);
        // максимальное штатное значение
        spinMax->setValue(maxValue);
    }
}

