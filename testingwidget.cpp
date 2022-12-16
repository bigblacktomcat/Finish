#include "testingwidget.h"
#include "ui_testingwidget.h"

#include <QButtonGroup>


TestingWidget::TestingWidget(IssoCore *core, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TestingWidget)
{
    ui->setupUi(this);
    //
    if (core)
    {
        // охраняемый объект
        m_object = core->guardedObject();
        // настроить связи
        initExchangerHandlers(core->exchanger());
        initControllerHandlers(core->modulesController());
    }
    // проинициализировать компоненты
    initControls();
    // обновить список модулей
    updateModuleList();
//    processModuleNameSelectionChanged();
}


TestingWidget::~TestingWidget()
{
    delete ui;
}


void TestingWidget::processModuleNameSelectionChanged(const QString &moduleName)
{
    // очистить список адресов мультидатчиков
    ui->cbMsAddress->clear();
    // добавить строку
    ui->cbMsAddress->addItem(ALL_ITEMS);
    // если выбраны все модули, не заполнять список мультидатчиков
    if (moduleName.contains(ALL_ITEMS))
        return;
    //
    if (!m_object)
        return;
    // найти модуль по имени
    IssoModule* module = m_object->findModuleByDisplayName(moduleName);
    if (!module)
        return;
    // получить конфиг адресного шлейфа
    auto msChainCfg = m_object->moduleAddressChainConfig(module->moduleConfigName());
    if (!msChainCfg)
        return;
    // заполнить выпадающий список адресов мультидатчиков
    foreach (quint8 id, msChainCfg->multiSensorIdList())
        ui->cbMsAddress->addItem(QString::number(id));
}


void TestingWidget::processBtnEmulateInfoClicked()
{
    if (!m_object)
        return;
    // подготовить параметры запроса INFO
    IssoParamBuffer params = makeInfoParams();
    // для каждого выбранного
    foreach (auto moduleName, selectedModuleNames())
    {
        // найти модуль по имени
        auto module = m_object->findModuleByDisplayName(moduleName);
        if (!module)
            continue;
        // команда
        IssoModuleCommand cmd(CMD_INFO, params);
        // ответ
        IssoModuleReply reply(module->ip(), module->port(), 100, cmd.bytes(), 1, 254);
        // сэмулировать событие получения INFO
        emit replyEventEmulated(reply, true);
    }
}


void TestingWidget::processBtnEmulateMasgClicked()
{
    if (!m_object)
        return;
    // для каждого выбранного модуля
    foreach (auto moduleName, selectedModuleNames())
    {
        // найти модуль по имени
        auto module = m_object->findModuleByDisplayName(moduleName);
        if (!module)
            continue;
        // получить список адресов МД модуля
        auto msAddrs = selectedMsAddresses(module->moduleConfigName());
        // сформировать параметры ответа для всех МД
        IssoParamBuffer params = makeMasgParams(msAddrs);
        // команда
        IssoModuleCommand cmd(CMD_MS_INFO, params);
        // ответ
        IssoModuleReply reply(module->ip(), module->port(), 100, cmd.bytes(), 1, 254);
        // сэмулировать событие получения MASG
        emit replyEventEmulated(reply, true);
    }
}

//void TestingWidget::processBtnEmulateMasgClicked()
//{
//    if (!m_object)
//        return;
//    // для каждого выбранного модуля
//    foreach (auto moduleName, selectedModuleNames())
//    {
//        // найти модуль по имени
//        auto module = m_object->findModuleByDisplayName(moduleName);
//        if (!module)
//            continue;
//        // для каждого выбранного адреса МД
//        foreach (auto msAddr, selectedMsAddresses(module->moduleConfigName()))
//        {
//            // подготовить параметры запроса MASG
//            IssoParamBuffer params = makeMasgParams(msAddr);
//            // команда
//            IssoModuleCommand cmd(CMD_MS_INFO, params);
//            // ответ
//            IssoModuleReply reply(module->ip(), module->port(), 100, cmd.bytes(), 1, 254);
//            // сэмулировать событие получения MASG
//            emit replyEventEmulated(reply, true);
//        }
//    }
//}


QStringList TestingWidget::selectedModuleNames()
{
    QStringList list;
    // если выбран пункт "Все", добавить все имена
    if (ui->cbModuleName->currentText().contains(ALL_ITEMS))
    {
        for (int i = 1; i < ui->cbModuleName->count(); ++i)
            list << ui->cbModuleName->itemText(i);
    }
    else
        list << ui->cbModuleName->currentText();
    // вернуть список
    return list;
}


QList<quint8> TestingWidget::selectedMsAddresses(const QString& moduleCfgName)
{
    QList<quint8> list;
    if (!m_object)
        return list;
    // получить конфиг адресного шлейфа
    auto msChainConfig = m_object->moduleAddressChainConfig(moduleCfgName);
    if (!msChainConfig)
        return list;
    // если выбран пункт "Все", добавить все адреса
    if (ui->cbMsAddress->currentText().contains(ALL_ITEMS))
    {
        // присвоить список МД конфига модуля
        list = msChainConfig->multiSensorIdList();
    }
    else
    {
        // получить список МД конфига модуля
        quint8 msAddr = ui->cbMsAddress->currentText().toInt();
        // если выбранный адрес МД содержится в конфиге, добавить его
        if (msChainConfig->multiSensorIdList().contains(msAddr))
            list << msAddr;
    }
    // вернуть список
    return list;
}


void TestingWidget::initExchangerHandlers(IssoExchangeManager *exchanger)
{

}


void TestingWidget::initControllerHandlers(IssoModulesController *controller)
{
    connect(this,       SIGNAL(replyEventEmulated(IssoModuleReply,bool)),
            controller, SLOT(processReplyReceived(IssoModuleReply,bool)));
//    connect(ui->tabGraphPlan,           SIGNAL(testCtrlAlarmReceived(IssoModuleReply)),
//            m_core.modulesController(), SLOT(processAlarmReceived(IssoModuleReply)));
//    connect(ui->tabGraphPlan,           SIGNAL(testCtrlReplyTimedOut(IssoModuleRequest)),
//            m_core.modulesController(), SLOT(processReplyTimedOut(IssoModuleRequest)));
}


void TestingWidget::updateModuleList()
{
    if (!m_object)
        return;
    ui->cbModuleName->addItem(ALL_ITEMS);
    ui->cbModuleName->addItems(m_object->moduleNames());
}


void TestingWidget::initControls()
{
    // инициализировать компоненты панели Датчики / Шлейфы
    initIntSensorControls();
    // инициализировать компоненты панели Мультидатчик
    initMsSensorControls();
    // инициализировать компоненты панели Реле
    initRelayControls();
    // задать обработчики событий
    initControlHandlers();
}


void TestingWidget::initIntSensorControls()
{
    // состояния
    m_intSensorStates.insert(SENSOR_SMOKE, ui->cbStateSmoke);
    m_intSensorStates.insert(SENSOR_CO, ui->cbStateCo);
    m_intSensorStates.insert(SENSOR_OPEN, ui->cbStateOpen);
    m_intSensorStates.insert(VIBRATION, ui->cbStateVibration);
    m_intSensorStates.insert(BTN_FIRE, ui->cbStateBtnFire);
    m_intSensorStates.insert(BTN_SECURITY, ui->cbStateBtnSecurity);
    m_intSensorStates.insert(ANALOG_CHAIN_1, ui->cbStateChain1);
    m_intSensorStates.insert(ANALOG_CHAIN_2, ui->cbStateChain2);
    m_intSensorStates.insert(ANALOG_CHAIN_3, ui->cbStateChain3);
    m_intSensorStates.insert(ANALOG_CHAIN_4, ui->cbStateChain4);
    m_intSensorStates.insert(ANALOG_CHAIN_5, ui->cbStateChain5);
    m_intSensorStates.insert(ANALOG_CHAIN_6, ui->cbStateChain6);
    m_intSensorStates.insert(VOLT_SENSORS, ui->cbStateVoltSensors);
    m_intSensorStates.insert(TEMP_OUT, ui->cbStateTemp);
    m_intSensorStates.insert(VOLT_BUS, ui->cbStateVoltBus);
    m_intSensorStates.insert(VOLT_BATTERY, ui->cbStateVoltBattery);
    m_intSensorStates.insert(SUPPLY_INTERNAL, ui->cbStateSupplyInt);
    m_intSensorStates.insert(SUPPLY_EXTERNAL, ui->cbStateSupplyExt);
    // значения
    m_intSensorValues.insert(VOLT_SENSORS, ui->dsbValueVoltSensors);
    m_intSensorValues.insert(TEMP_OUT, ui->dsbValueTemp);
    m_intSensorValues.insert(VOLT_BUS, ui->dsbValueVoltBus);
    m_intSensorValues.insert(VOLT_BATTERY, ui->dsbValueVoltBattery);
    //
    // заполнить выпадающие списки состояний
    QStringList stateNames;
    stateNames << IssoCommonData::stringBySensorState(STATE_NORMAL)
               << IssoCommonData::stringBySensorState(STATE_WARNING)
               << IssoCommonData::stringBySensorState(STATE_ALARM)
               << IssoCommonData::stringBySensorState(STATE_BREAK);
    foreach (auto combo, m_intSensorStates)
        combo->addItems(stateNames);
}


void TestingWidget::initMsSensorControls()
{
    // состояния внутренних датчиков МД
    m_msSensorStates.insert(DIGITALSENSOR_SMOKE_O, ui->cbMsStateSmokeO);
    m_msSensorStates.insert(DIGITALSENSOR_TEMP_A, ui->cbMsStateTempA);
    m_msSensorStates.insert(DIGITALSENSOR_TEMP_D, ui->cbMsStateTempD);
    m_msSensorStates.insert(DIGITALSENSOR_SMOKE_E, ui->cbMsStateSmokeE);
    m_msSensorStates.insert(DIGITALSENSOR_FLAME_STD_DEV, ui->cbMsStateFlameStdDev);
    m_msSensorStates.insert(DIGITALSENSOR_CO, ui->cbMsStateCo);
    m_msSensorStates.insert(DIGITALSENSOR_VOC, ui->cbMsStateVoc);
    // значения
    m_msSensorValues.insert(DIGITALSENSOR_SMOKE_O, ui->sbMsValueSmokeO);
    m_msSensorValues.insert(DIGITALSENSOR_TEMP_A, ui->sbMsValueTempA);
    m_msSensorValues.insert(DIGITALSENSOR_TEMP_D, ui->sbMsValueTempD);
    m_msSensorValues.insert(DIGITALSENSOR_SMOKE_E, ui->sbMsValueSmokeE);
    m_msSensorValues.insert(DIGITALSENSOR_FLAME_STD_DEV, ui->sbMsValueFlameStdDev);
    m_msSensorValues.insert(DIGITALSENSOR_CO, ui->sbMsValueCo);
    m_msSensorValues.insert(DIGITALSENSOR_VOC, ui->sbMsValueVoc);
    //
    // заполнить выпадающие списки состояний
    QStringList stateNames;
    stateNames << IssoCommonData::stringBySensorState(STATE_NORMAL)
               << IssoCommonData::stringBySensorState(STATE_WARNING)
               << IssoCommonData::stringBySensorState(STATE_ALARM)
               << IssoCommonData::stringBySensorState(STATE_BREAK);
    // состояние внутренних датчиков МД
    foreach (auto combo, m_msSensorStates)
    {
        combo->addItems(stateNames);
    }
    // общее состояние МД
    ui->cbMsCommonState->addItems(stateNames);
}


void TestingWidget::initRelayControls()
{
    // заполнить выпадающий список наименований реле
    QStringList relayNames;
    relayNames << IssoCommonData::stringByParamId(INT_RELAY_1)
               << IssoCommonData::stringByParamId(INT_RELAY_2)
               << IssoCommonData::stringByParamId(INT_RELAY_3)
               << IssoCommonData::stringByParamId(INT_RELAY_4)
               << IssoCommonData::stringByParamId(INT_RELAY_5);
    ui->cbRelayName->addItems(relayNames);
    //
    // сформировать группу радиокнопок
    QButtonGroup* radioGroup = new QButtonGroup(this);
    radioGroup->addButton(ui->rbtnStateRelayOff, 0);
    radioGroup->addButton(ui->rbtnStateRelayOn, 1);
}


void TestingWidget::initControlHandlers()
{
    connect(ui->cbModuleName, SIGNAL(currentIndexChanged(QString)),
            this,             SLOT(processModuleNameSelectionChanged(QString)));
    connect(ui->btnEmulateInfo, SIGNAL(clicked(bool)),
            this,               SLOT(processBtnEmulateInfoClicked()));
    connect(ui->btnEmulateMasg, SIGNAL(clicked(bool)),
            this,               SLOT(processBtnEmulateMasgClicked()));
}


IssoState TestingWidget::msSensorState(IssoDigitalSensorId id)
{
    // компонент, содержащий состояние встроенного датчика МД
    auto combo = m_msSensorStates.value(id);
    // состояние встроенного датчика МД
    return IssoCommonData::sensorStateByString(combo->currentText());
}


QMap<IssoDigitalSensorId, IssoState> TestingWidget::msSensorStates()
{
    QMap<IssoDigitalSensorId, IssoState> states;
    foreach (auto msSensorId, m_msSensorStates.keys())
    {
        // добавить состояние
        states.insert(msSensorId, msSensorState(msSensorId));
    }
    return states;
}


IssoParamBuffer TestingWidget::makeInfoParams()
{
    IssoParamBuffer params;
    foreach (auto paramId, m_intSensorStates.keys())
    {
        // указатель на параметр
        IssoParam* p = nullptr;
        //
        // компонент состояния встроенного датчика
        auto combo = m_intSensorStates.value(paramId);
        // состояние
        IssoState state = IssoCommonData::sensorStateByString(combo->currentText());
        // создать параметр
        switch(paramId)
        {
            case VOLT_BATTERY:
            case VOLT_BUS:
            case VOLT_SENSORS:
            case TEMP_OUT:
            {
                // компонент значения встроенного датчика
                auto spin = m_intSensorValues.value(paramId);
                // значение
                double value = spin->value();
                // параметр расширенного состояния
                p = new IssoSensorExtStateParam(paramId, state, value);
                break;
            }
            default:
            {
                // параметр состояния
                p = new IssoSensorStateParam(paramId, state);
                break;
            }
        }
        // добавить параметр в буфер
        params.insertParam(p);
    }
    return params;
}


IssoParamBuffer TestingWidget::makeMasgParams(QList<quint8> msAddrs)
{
    // общее состояние мультидатчика:
    //      0 - норма
    //      1 - внимание
    //      2 - тревога
    //      3 - неисправность
    //      4 - обрыв / КЗ
    //
    // буфер параметров ответа
    IssoParamBuffer params;
    //
    // общее состояние мультидатчика
    IssoState msState =
            IssoCommonData::sensorStateByString(ui->cbMsCommonState->currentText());
    // преобразовать в значение
    quint8 msStateValue = IssoMultiSensorData::stateToValue(msState);
    //
    // получить состояния встроенных датчиков МД
    auto states = msSensorStates();
    // преобразовать их в 2-байтное число
    quint16 detailsValue = IssoMultiSensorData::makeDetailsValue(states);
    //
    // коллекция данных мультидатчиков
    QMap<quint8, IssoMultiSensorData> dataMap;
    // добавить данные текущего МД из списка
    foreach (quint8 msAddr, msAddrs)
    {
        dataMap.insert(msAddr, IssoMultiSensorData(msAddr, msStateValue, detailsValue));
    }
//    dataMap.insert(msAddrs, IssoMultiSensorData(msAddrs, msStateValue, detailsValue));
    //
    // добавить параметр в буфер
    params.insertParam(new IssoMultiSensorStateParam(MULTISENSOR, dataMap));
    return params;



//    //
//    // получить конфиг адресного шлейфа
//    auto msChainConfig = m_object->moduleAddressChainConfig(module->moduleConfigName());
//    if (msChainConfig)
//    {
//        QMap<quint8, IssoMultiSensorData> dataMap;
//        IssoMultiSensorData multiData;
//        //
//        // задать состояния каждого мультидатчика на адресном шлейфе
//        foreach (quint8 id, msChainConfig->multiSensorIdList())
//        {
//            switch(id % 5)
//            {
//                case 0:
//                {
//                    multiData = IssoMultiSensorData(id, 0, 0x0000);
//                    break;
//                }
//                case 1:
//                {
//                    multiData = IssoMultiSensorData(id, 1, 0x0005);
//                    break;
//                }
//                case 2:
//                {
//                    multiData = IssoMultiSensorData(id, 2, 0x002A);
//                    break;
//                }
//                case 3:
//                {
//                    multiData = IssoMultiSensorData(id, 3, 0x003F);
//                    break;
//                }

//                case 4:
//                {
//                    multiData = IssoMultiSensorData(id, 4, 0x1555);
//                    break;
//                }
//                default:
//                    break;
//            }
//            dataMap.insert(multiData.multiSensorId(), multiData);
//        }
//        // добавить параметр в буфер
//        params.insertParam(new IssoMultiSensorStateParam(MULTISENSOR, dataMap));
//    }
//    return params;
}

