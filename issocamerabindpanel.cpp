#include "issocamerabindpanel.h"
#include "ui_issocamerabindpanel.h"


IssoCameraBindPanel::IssoCameraBindPanel(QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::IssoCameraBindPanel)
{
    ui->setupUi(this);
    initCombos();
}


IssoCameraBindPanel::~IssoCameraBindPanel()
{
    delete ui;
}


void IssoCameraBindPanel::initCombos()
{
    m_combos.insert(SENSOR_SMOKE, ui->cbSmoke);
    m_combos.insert(SENSOR_CO, ui->cbCO);
    m_combos.insert(BTN_FIRE, ui->cbBtnFire);
    m_combos.insert(BTN_SECURITY, ui->cbBtnSecurity);
    m_combos.insert(VOLT_BATTERY, ui->cbVoltBattery);
    m_combos.insert(VOLT_BUS, ui->cbVoltBus);
    m_combos.insert(VOLT_SENSORS, ui->cbVoltSensors);
    m_combos.insert(TEMP_OUT, ui->cbTemp);
    m_combos.insert(VIBRATION, ui->cbVibration);
    m_combos.insert(SENSOR_OPEN, ui->cbSensorOpen);
    m_combos.insert(SUPPLY_INTERNAL, ui->cbSupplyInternal);
    m_combos.insert(SUPPLY_EXTERNAL, ui->cbSupplyExternal);
}


void IssoCameraBindPanel::setCameraList(QList<IssoCamera *> cameras)
{
    // для каждого выпадающего списка
    foreach (QComboBox* combo, m_combos.values())
    {
        // очистить список ассоциированных камер
        combo->clear();
        // добавить пустую строку
        combo->addItem("", -1);
        // заполнить список ассоциированных камер
        foreach (IssoCamera* camera, cameras)
        {
            // добавить имя камеры и ID
            combo->addItem(camera->displayName(), QVariant(camera->id()));
        }
    }
}


QMap<IssoParamId, int> IssoCameraBindPanel::boundCameras()
{
    QMap<IssoParamId, int> map;
    // проход по всем комбо
    foreach (IssoParamId sensorId, m_combos.keys())
    {
        // получить комбо по ID датчика
        QComboBox* combo = m_combos.value(sensorId, nullptr);
        if (!combo)
            continue;
        // получить ID выбранной камеры
        int cameraId = combo->currentData().toInt();
        // если камера выбрана, добавить в коллекцию
        if (cameraId != -1)
            map.insert(sensorId, cameraId);
    }
    return map;
}


void IssoCameraBindPanel::setBoundCameras(const QMap<IssoParamId, int> &boundCameras)
{
    // проход по всем комбо
    foreach (IssoParamId sensorId, m_combos.keys())
    {
        // получить комбо по ID датчика
        QComboBox* combo = m_combos.value(sensorId, nullptr);
        if (!combo)
            continue;
        // найти Id камеры, связанной с текущим ID датчика
        int cameraId = boundCameras.value(sensorId, -1);
        //
        // найти индекс камеры в комбо по ID
        int camIndex = combo->findData(QVariant(cameraId));
        // выбрать камеру в комбо
        combo->setCurrentIndex(camIndex);
    }
}
