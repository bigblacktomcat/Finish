#include "issomodulescontroller.h"
#include "issoregisters.h"
#include "QtConcurrent/QtConcurrent"



IssoModulesController::IssoModulesController(QObject *parent)
    : QObject(parent)
{
}


IssoModulesController::~IssoModulesController()
{
    qDebug() << "~IssoModulesController";
}


IssoObject *IssoModulesController::object() const
{
    return m_object;
}


void IssoModulesController::setObject(IssoObject *object)
{
    m_object = object;
}


void IssoModulesController::analyzeInfoReply(const IssoModuleReply &reply)
{
    if (!m_object)
        return;
    // найти модуль по адресу
    IssoModule* module = findModuleByAddress(reply.address());
    if (!module)
        return;
    //
    // если модуль Offline, то очистить его параметры перед разбором пакета INFO
    if (module->moduleState() == INACTIVE)
        module->clearStateParams();
    //
    // разобрать буфер параметров модуля
    parseParamBuffer(module, reply.command().paramBuffer());
}


void IssoModulesController::analyzeRelayReply(const IssoModuleReply &reply)
{
    // найти модуль по адресу
    IssoModule* module = findModuleByAddress(reply.address());
    if (!module)
        return;
    //
    // если модуль Offline, перевести его в Online (отобразить актуальные параметры)
    if (module->moduleState() == INACTIVE)
        module->setOnline();
    //
    // получить указатель
    QSharedPointer<IssoParam> paramPtr = reply.command().paramPtrByIndex(0);
    // получить параметр состояния реле
    IssoSensorStateParam* stateParam = dynamic_cast<IssoSensorStateParam*>(paramPtr.data());
    if (!stateParam)
        return;
    // обновить параметр состояния реле (кнопки)
    module->updateStateParam(paramPtr);
    // получить состояние реле (кнопки)
    bool pressed = (stateParam->state() == RELAY_ON);
    // инициировать событие изменения состояния кнопки
    emit relayStateChanged(module->displayName(), stateParam->id(), pressed);
}


void IssoModulesController::analyzeDescriptorReply(const IssoModuleReply &reply)
{
    // переменные
    quint32 ip = 0;
    QByteArray mac = QByteArray(6, '0');
    quint16 port = 0;
    quint32 serverIp = 0;
    quint16 serverPort = 0;
    QByteArray phone = QByteArray(12, '0');
    bool phoneEnabled = false;
    quint8 crc = 0;

    // получить все параметры дескриптора:
    // IP-адрес
    IssoIpParam* ipParam =
            dynamic_cast<IssoIpParam*>(reply.command().paramById(DESC_IP));
    if (ipParam)
        ip = ipParam->ip();
    // МАС
    IssoArrayParam* macParam =
            dynamic_cast<IssoArrayParam*>(reply.command().paramById(DESC_MAC));
    if (macParam)
        mac = macParam->array();
    // порт
    IssoShortParam* portParam =
            dynamic_cast<IssoShortParam*>(reply.command().paramById(DESC_PORT));
    if (portParam)
        port = portParam->shortValue();
    // IP-адрес сервера
    IssoIpParam* serverIpParam =
            dynamic_cast<IssoIpParam*>(reply.command().paramById(DESC_SERVER_IP));
    if (serverIpParam)
        serverIp = serverIpParam->ip();
    // порт сервера
    IssoShortParam* serverPortParam =
            dynamic_cast<IssoShortParam*>(reply.command().paramById(DESC_SERVER_PORT));
    if (serverPortParam)
        serverPort = serverPortParam->shortValue();
    // номер телефона
    IssoArrayParam* phoneParam =
            dynamic_cast<IssoArrayParam*>(reply.command().paramById(DESC_PHONE));
    if (phoneParam)
        phone = phoneParam->array();
    // разрешение использовать телефон
    IssoBoolParam* phoneEnabledParam =
            dynamic_cast<IssoBoolParam*>(reply.command().paramById(DESC_PHONE_ENABLED));
    if (phoneEnabledParam)
        phoneEnabled = phoneEnabledParam->boolValue();
    // CRC
    IssoByteParam* crcParam =
            dynamic_cast<IssoByteParam*>(reply.command().paramById(DESC_CRC));
    if (crcParam)
        crc = crcParam->byteValue();
    //
    // сформировать дескриптор
    IssoModuleDescriptor desc(ip, mac, port, serverIp, serverPort, phone, phoneEnabled, crc);
    // если ответ с заводского ip, значит активация,
    // т.е. искать модуль по ip из дескриптора,
    // иначе искать модуль по ip из ответного пакета
    QHostAddress moduleIp =
            (reply.address().isEqual(IssoConst::DEFAULT_MODULE_IP, QHostAddress::ConvertV4MappedToIPv4) ?
                                                        QHostAddress(desc.ip()) : reply.address());
    // найти модуль по адресу
    IssoModule* module = findModuleByAddress(moduleIp);
    // если модуль не найден, вернуть пустое имя
    // (такого быть не должно)
    QString moduleName = (module ? module->displayName() : "");
    // инициировать событие
    switch(reply.command().id())
    {
        case CMD_DESC_GET:
        {
            emit descriptorRead(moduleName, desc);
            break;
        }
        case CMD_DESC_SET:
        {
            emit descriptorWritten(moduleName, desc);
            break;
        }
        default:
            break;
    }
}


void IssoModulesController::analyzeMsPollAddrs(const IssoModuleReply &reply)
{
    if (!m_object)
        return;
    // найти модуль по адресу
    IssoModule* module = findModuleByAddress(reply.address());
    if (!module)
        return;
    // получить параметр опрашиваемых адресов мультидатчиков
    IssoSizedArrayParam* addrsParam =
            dynamic_cast<IssoSizedArrayParam*>(reply.command().paramById(MULTISENSOR_ADDRS));
    if (addrsParam)
    {
        emit multiSensorPollingAddrsReceived(module->uniqueId(), addrsParam->array());
    }
}


void IssoModulesController::analyzeMsSingleRegSetReply(const IssoModuleReply &reply)
{
    // получить параметр данных регистра
    auto regParam =
            dynamic_cast<IssoSingleRegInfoParam*>(reply.command().paramById(SINGLE_REG_DATA));
    if (!regParam)
        return;
    // проанализировать адрес регистра
    switch(regParam->regAddress())
    {
        // состояние мультидатчика
        case IssoHoldReg::MS_STATE:
        {
            // преобразовать значение в состояние
            IssoState msState = IssoMultiSensorData::valueToState(regParam->regValue());
            // если Норма, сбросить состояние мультидатчика в Норму
            if (msState == STATE_NORMAL)
                resetMsState(reply.address(), regParam->multiSensorId());
//            {
//                // сформировать данные для текущего мультидатчика
//                IssoMultiSensorData msData(regParam->multiSensorId(), 0, 0);
//                // создать коллекцию с данными текущего датчика в Норме
//                QMap<quint8, IssoMultiSensorData> msDataMap;
//                msDataMap.insert(msData.multiSensorId(), msData);
//                // сформировать буфер
//                IssoParamBuffer buffer;
//                buffer.insertParam(new IssoMultiSensorStateParam(MULTISENSOR, msDataMap));
//                // обработать буфер
//                parseParamBuffer(module, buffer);
//            }
            // известить об успешном сбросе состояния мультидатчика
            emit multiSensorRegActionCompleted(IssoMsRegAction::RESET_STATE,
                                                 reply.address(),
                                                 regParam->multiSensorId(),
                                                 msState == STATE_NORMAL);
            break;
        }
        default:
            break;
    }
}


void IssoModulesController::analyzeMsSingleRegSetTimeOut(const IssoModuleRequest &request)
{
    // получить параметр данных регистра
    auto regParam = dynamic_cast<IssoSingleRegInfoParam*>(request.command().paramById(SINGLE_REG_DATA));
    if (!regParam)
        return;
    // проанализировать адрес регистра
    switch(regParam->regAddress())
    {
        // состояние мультидатчика
        case IssoHoldReg::MS_STATE:
        {
            // известить об неудаче сброса состояния мультидатчика
            emit multiSensorRegActionCompleted(IssoMsRegAction::RESET_STATE,
                                                 request.address(),
                                                 regParam->multiSensorId(),
                                                 false);
            break;
        }
        default:
            break;
    }
}


void IssoModulesController::analyzeMsBlockRegGetReply(const IssoModuleReply &reply)
{
    // получить параметр данных регистра
    auto regParam =
            dynamic_cast<IssoBlockRegInfoParam*>(reply.command().paramById(BLOCK_REG_DATA));
    if (!regParam)
        return;
    // проанализировать начальный адрес блока регистров
    switch(regParam->regAddress())
    {
        // маска подключенных внутренних датчиков
        case IssoHoldReg::SENSOR_MASK:
        {
            // если блок регистров не меньше 19
            if (regParam->regCount() >= IssoConst::THRESHOLD_REG_COUNT)
            {
                // сохранить пороги в конфиге мультидатчика
                bool success = saveThresholdData(reply.address(),
                                                 regParam->multiSensorId(),
                                                 regParam->regArray());
                // известить об успешной записи порогов мультидатчика
                emit multiSensorRegActionCompleted(IssoMsRegAction::READ_THRESHOLD,
                                                   reply.address(),
                                                   regParam->multiSensorId(),
                                                   success);
            }
            break;
        }
        default:
            break;
    }
}


void IssoModulesController::analyzeMsBlockRegGetTimeOut(const IssoModuleRequest &request)
{
    // получить параметр данных регистра
    auto regParam =
            dynamic_cast<IssoBlockRegAddrParam*>(request.command().paramById(BLOCK_REG_ADDR));
    if (!regParam)
        return;
    // проанализировать начальный адрес блока регистров
    switch(regParam->regAddress())
    {
        // маска подключенных внутренних датчиков
        case IssoHoldReg::SENSOR_MASK:
        {
            // если блок регистров не меньше 19
            if (regParam->regCount() >= IssoConst::THRESHOLD_REG_COUNT)
            {
                // известить о неудаче записи порогов мультидатчика
                emit multiSensorRegActionCompleted(IssoMsRegAction::READ_THRESHOLD,
                                                     request.address(),
                                                     regParam->multiSensorId(),
                                                     false);
            }
            break;
        }
        default:
            break;
    }
}


void IssoModulesController::analyzeMsBlockRegSetReply(const IssoModuleReply &reply)
{
    // получить параметр данных регистра
    auto regParam =
            dynamic_cast<IssoBlockRegInfoParam*>(reply.command().paramById(BLOCK_REG_DATA));
    if (!regParam)
        return;
    // проанализировать начальный адрес блока регистров
    switch(regParam->regAddress())
    {
        // маска подключенных внутренних датчиков
        case IssoHoldReg::SENSOR_MASK:
        {
            // если блок регистров не меньше 19
            if (regParam->regCount() >= IssoConst::THRESHOLD_REG_COUNT)
            {
                // известить об успешной записи порогов мультидатчика
                emit multiSensorRegActionCompleted(IssoMsRegAction::WRITE_THRESHOLD,
                                                     reply.address(),
                                                     regParam->multiSensorId(),
                                                     true);
            }
            break;
        }
        default:
            break;
    }
}


void IssoModulesController::analyzeMsBlockRegSetTimeOut(const IssoModuleRequest &request)
{
    // получить параметр данных регистра
    auto regParam =
            dynamic_cast<IssoBlockRegInfoParam*>(request.command().paramById(BLOCK_REG_DATA));
    if (!regParam)
        return;
    // проанализировать начальный адрес блока регистров
    switch(regParam->regAddress())
    {
        // маска подключенных внутренних датчиков
        case IssoHoldReg::SENSOR_MASK:
        {
            // если блок регистров не меньше 19
            if (regParam->regCount() >= IssoConst::THRESHOLD_REG_COUNT)
            {
                // известить о неудаче записи порогов мультидатчика
                emit multiSensorRegActionCompleted(IssoMsRegAction::WRITE_THRESHOLD,
                                                     request.address(),
                                                     regParam->multiSensorId(),
                                                     false);
            }
            break;
        }
        default:
            break;
    }
}


//void IssoModulesController::processReplyReceived(IssoModuleReply reply, bool ok)
//{
////    qDebug() << "processReplyReceived - Thread ID = "
////             << QThread::currentThreadId() << endl;

//    QTime t;
//    t.start();

//    // если ответ некорректен, не обрабатывать
//    if (!ok)
//        return;
//    //
////    QtConcurrent::run(
////        [this, reply]()
////        {
////            qDebug() << "new thread - Thread ID = "
////                      << QThread::currentThreadId() << endl;


//            switch (reply.command().id())
//            {
//                // получены данные о состоянии реле
//                case CMD_RELAY:
//                {
//                    analyzeRelayReply(reply);
//                    break;
//                }
//                // получены состояния встроенных датчиков
//                case CMD_INFO:
//                // или состояния мультидатчиков
//                case CMD_MS_INFO:
//                // или сырые данные мультидатчиков
//                case CMD_MS_INFO_RAW:
//                {
//                    // проанализировать параметры ответа
//                    analyzeInfoReply(reply);
//                    break;
//                }
//                // получено подтверждение установки адресов
//                // мультидатчиков, подлежащих опросу
//                case CMD_MS_POLL_ADDRS:
//                {
//                    // обработать подтверждение установки опрашиваемых адресов мультидатчиков
//                    analyzeMsPollAddrs(reply);
//                    break;
//                }
//        //        // получено подтверждение сброса состояний мультидатчика
//        //        case CMD_MULTISENSOR_STATES_RESET:
//        //        {
//        //            analyzeMultiSensorResetReply(reply);
//        //            break;
//        //        }
//                // получен дескриптор модуля в ответ на запрос / установку дескриптора
//                case CMD_DESC_GET:
//                case CMD_DESC_SET:
//                {
//                    analyzeDescriptorReply(reply);
//                    break;
//                }
//                // получены данные одиночного регистра
//        //        case CMD_MS_SINGLE_INPUT_REG_GET:
//        //        case CMD_MS_SINGLE_HOLD_REG_GET:
//                case CMD_MS_SINGLE_HOLD_REG_SET:
//                {
//                    analyzeMsSingleRegSetReply(reply);
//                    break;
//                }
//                case CMD_MS_BLOCK_HOLD_REG_SET:
//                {
//                    analyzeMsBlockRegSetReply(reply);
//                    break;
//                }
//                default:
//                    break;
//            }
////        });

////    qDebug() << QString("process reply time = %1 ms").arg(t.elapsed()) << endl;
//}

void IssoModulesController::processReplyReceived(IssoModuleReply reply, bool ok)
{
//    QTime t;
//    t.start();

    // если ответ некорректен, не обрабатывать
    if (!ok)
        return;
    //
    switch (reply.command().id())
    {
        // получены данные о состоянии реле
        case CMD_RELAY:
        {
            analyzeRelayReply(reply);
            break;
        }
        // получены состояния встроенных датчиков
        case CMD_INFO:
        // или состояния мультидатчиков
        case CMD_MS_INFO:
        // или сырые данные мультидатчиков
        case CMD_MS_INFO_RAW:
        {
            // проанализировать параметры ответа
            analyzeInfoReply(reply);
            break;
        }
        // получено подтверждение установки адресов
        // мультидатчиков, подлежащих опросу
        case CMD_MS_POLL_ADDRS:
        {
            // обработать подтверждение установки опрашиваемых адресов мультидатчиков
            analyzeMsPollAddrs(reply);
            break;
        }
//        // получено подтверждение сброса состояний мультидатчика
//        case CMD_MULTISENSOR_STATES_RESET:
//        {
//            analyzeMultiSensorResetReply(reply);
//            break;
//        }
        // получен дескриптор модуля в ответ на запрос / установку дескриптора
        case CMD_DESC_GET:
        case CMD_DESC_SET:
        {
            analyzeDescriptorReply(reply);
            break;
        }
        // получены данные одиночного регистра
//        case CMD_MS_SINGLE_INPUT_REG_GET:
//        case CMD_MS_SINGLE_HOLD_REG_GET:
        case CMD_MS_SINGLE_HOLD_REG_SET:
        {
            analyzeMsSingleRegSetReply(reply);
            break;
        }
        case CMD_MS_BLOCK_HOLD_REG_GET:
        {
            analyzeMsBlockRegGetReply(reply);
            break;
        }
        case CMD_MS_BLOCK_HOLD_REG_SET:
        {
            analyzeMsBlockRegSetReply(reply);
            break;
        }
        case CMD_MULTISENSOR_EXIST_GET:
        {
        analyzeInfoReply(reply);
        qDebug() << QString("process reply multisensor_exist");
        break;
        }
        default:
            break;
    }

//    qDebug() << QString("process reply time = %1 ms").arg(t.elapsed()) << endl;
}


void IssoModulesController::processReplyTimedOut(const IssoModuleRequest &request)
{
    // найти модуль по адресу
    IssoModule* module = findModuleByAddress(request.address());
    // обработать команду согласно ID
    switch (request.command().id())
    {
        case CMD_INFO:
        {
            // если модуль не найден, то выход
            if (!module)
                return;

            // новое состояние модуля неактивное
            IssoModuleState newModuleState = INACTIVE;
            // если состояние модуля изменилось
            if (module->moduleState() != newModuleState)
            {
                // сбросить состояния датчиков / шлейфов модуля
                module->clearStateParams();
                // инициировать событие изменения состояния модуля
                emit moduleStateChanged(module->displayName(), newModuleState);
                // инициировать событие изменения состояний датчиков
                emit sensorStatesChanged(module->displayName()/*, QSet<IssoParamId>()*/);
            }
            break;
        }
        case CMD_DESC_GET:
        case CMD_DESC_SET:
        {
            // если модуль не найден, вернуть пустое имя
            // (во время активации модуль имеет заводские настройки
            // и не может быть найден, т.е. имя будет пустым)
            QString moduleName = (module ? module->displayName() : "");
            // инициировать событие ошибки получения / установки дескриптора
            emit descriptorFailed(moduleName, request.command().id());
            break;
        }
//        case CMD_MS_SINGLE_INPUT_REG_GET:
//        case CMD_MS_SINGLE_HOLD_REG_GET:
        case CMD_MS_SINGLE_HOLD_REG_SET:
        {
            analyzeMsSingleRegSetTimeOut(request);
            break;
        }
        case CMD_MS_BLOCK_HOLD_REG_GET:
        {
            analyzeMsBlockRegGetTimeOut(request);
            break;
        }
        case CMD_MS_BLOCK_HOLD_REG_SET:
        {
            analyzeMsBlockRegSetTimeOut(request);
            break;
        }
        default:
            break;
    }
}


void IssoModulesController::processAlarmReceived(IssoModuleReply reply)
{
//    qDebug() << "processAlarmReceived = " <<
//                reply.command()->bytes().toHex(' ').toUpper() << endl;

    // если команда не тревожная, не обрабатывать
    if (reply.command().id() != CMD_ALARM)
        return;
    // проанализировать параметры ответа
    analyzeInfoReply(reply);
}


void IssoModulesController::updateModuleParams(const QString &moduleName,
                                               const IssoParamBuffer &paramBuffer)
{
    if (!m_object)
        return;
    // найти модуль по имени
    IssoModule* module = findModuleByDisplayName(moduleName);
    if (!module)
        return;

    if (paramBuffer.isEmpty())
    {
        module->clearStateParams();
        emit sensorStatesChanged(module->displayName());
    }
    else
        parseParamBuffer(module, paramBuffer);
}


IssoModule *IssoModulesController::findModuleByAddress(QHostAddress addr)
{
    return (m_object ? m_object->findModuleByAddress(addr, true) : nullptr);
}


IssoModule *IssoModulesController::findModuleByDisplayName(const QString &displayName)
{
    return (m_object ? m_object->findModuleByDisplayName(displayName) : nullptr);
}


IssoBuilding *IssoModulesController::findBuildingByNumber(int number)
{
    return (m_object ? m_object->findBuildingByNumber(number) : nullptr);
}


void IssoModulesController::parseParamBuffer(IssoModule *module,
                                             const IssoParamBuffer &buffer)
{
    if (!m_object || !module)
        return;

    // текущее состояние модуля
    IssoModuleState curModuleState = module->moduleState();
    //
    // определить, получен ли буфер впервые
    bool initBuffer = module->stateParams().isEmpty();
    // очистить список измененных параметров
    module->clearChangedParams();
    //
    // обработать все параметры команды
    for (int i = 0; i < buffer.paramCount(); ++i)
    {
        // получить параметр состояния датчика
        QSharedPointer<IssoParam> stateParamPtr = buffer.paramPtrByIndex(i);
        if (stateParamPtr.isNull())
            continue;
        // разобрать параметр состояния
        parseStateParam(module, stateParamPtr);
    }
    //
    // проанализировать состояния датчиков и реле
    // и установить результирующее состояние модуля
    module->analyzeSensorStates();
    //
    // если состояние модуля изменилось
    if (module->moduleState() != curModuleState)
    {
        // инициировать событие изменения состояния модуля
        emit moduleStateChanged(module->displayName(), module->moduleState());
    }
    //
    // если состояния датчиков изменились,
    // инициировать событие изменения состояний датчиков
    if (!module->changedParams().isEmpty())
        emit sensorStatesChanged(module->displayName());
    // !!!
    // Нужно передавать буфер параметров в сигнал,
    // иначе есть шанс, что разбор следующего пакета (асинхронный)
    // очистит измененные параметры до их обработки
    // !!!
}


void IssoModulesController::parseStateParam(IssoModule *module,
                                            const QSharedPointer<IssoParam>& stateParamPtr)
{
    IssoSensorStateParam* stateParam =
            dynamic_cast<IssoSensorStateParam*>(stateParamPtr.data());
    if (!stateParam)
        return;

    // обработать в зависимости от id параметра
    switch (stateParam->id())
    {
        //
        // состояние встроенного датчика
        case BTN_FIRE:
        case BTN_SECURITY:
        case SENSOR_SMOKE:
        case SENSOR_CO:
        case SUPPLY_EXTERNAL:
        case SUPPLY_INTERNAL:
        case VIBRATION:
        case SENSOR_OPEN:
        //
        // состояние датчика со значением
        case VOLT_BATTERY:
        case VOLT_BUS:
        case VOLT_SENSORS:
        case TEMP_OUT:
        {
            // если параметр не подключен, не обрабатывать
            if (!m_object->moduleParamEnabled(module->moduleConfigName(), stateParam->id()))
                return;
            // обновить состояние датчика модуля
            module->updateStateParam(stateParamPtr);
            break;
        }
        //
        // состояние аналогового шлейфа
        case ANALOG_CHAIN_1:
        case ANALOG_CHAIN_2:
        case ANALOG_CHAIN_3:
        case ANALOG_CHAIN_4:
        case ANALOG_CHAIN_5:
        case ANALOG_CHAIN_6:
        {
            // если параметр не подключен, не обрабатывать
            if (!m_object->moduleParamEnabled(module->moduleConfigName(), stateParam->id()))
                return;
            // получить конфиг шлейфа модуля
            IssoChainConfig* chainCfg =
                    m_object->moduleChainConfig(module->moduleConfigName(), stateParam->id());
            if (!chainCfg)
                return;
//            // если полученный тип шлейфа не аналоговый, далее
//            if (chainCfg->type() != ANALOG)
//                return;
            // обновить состояние датчика модуля
            module->updateStateParam(stateParamPtr);
            break;
        }
        //
        // состояние мультидатчика
        case MULTISENSOR:
        {
            // если параметр не подключен, не обрабатывать
            if (!m_object->moduleParamEnabled(module->moduleConfigName(), stateParam->id()))
                return;
//            //
//            // если модуль на сервисном обслуживании, не обрабатывать
//            IssoMultiSensorStateParam* param =
//                    dynamic_cast<IssoMultiSensorStateParam*>(stateParam);
//            if ((param) && (module->hasServiceMultiSensorId(param->multiSensorId())))
//            {
//                return;
//            }
            //
            // получить конфиг шлейфа модуля
            IssoChainConfig* chainCfg =
                    m_object->moduleChainConfig(module->moduleConfigName(), stateParam->id());
            if (!chainCfg)
                return;
            // если полученный тип шлейфа не цифровой, далее
            if (chainCfg->type() != MULTICRITERIAL)
                return;
            // обновить состояние мультидатчика
            module->updateMultiSensorStateParam(stateParamPtr);
            break;
        }
        case MULTISENSOR_STATE_RAW:
        {
//            // если модуль на сервисном обслуживании, не обрабатывать
//            IssoMultiSensorStateParam* param =
//                    dynamic_cast<IssoMultiSensorStateParam*>(stateParam);
//            if ((param) && (module->hasServiceMultiSensorId(param->multiSensorId())))
//            {
//                return;
//            }
//            //
//            // получить конфиг шлейфа модуля
//            IssoChainConfig* chainCfg =
//                    m_object->moduleChainConfig(module->getModuleConfigName(), stateParam->id());
//            if (!chainCfg)
//                return;
//            // если полученный тип шлейфа не цифровой, далее
//            if (chainCfg->type() != DIGITAL)
//                return;

//            // обновить состояние мультидатчика
//            module->updateMultiSensorStateParam(stateParamPtr);

            //
            // если параметр определяет сырое состояние
            if (stateParam->id() == MULTISENSOR_STATE_RAW)
            {
                // получить параметр расширенного состояния мультидатчика
                IssoMultiSensorRawDataParam* multiRawParam =
                        dynamic_cast<IssoMultiSensorRawDataParam*>(stateParam);
                if (multiRawParam)
                {
                    // инициировать событие получения сырых данных мультидатчика
                    emit multiSensorRawDataReceived(
//                                module->uniqueId(),
                                module->displayName(),
                                multiRawParam->data().multiSensorId(),
                                multiRawParam->data().state(),
                                multiRawParam->data().getDigitalSensorStates(),
                                multiRawParam->rawValues());
                }
            }
            break;
        }
        //
        // состояние реле (кнопки)
        case INT_RELAY_1:
        case INT_RELAY_2:
        case INT_RELAY_3:
        case INT_RELAY_4:
        case INT_RELAY_5:
        case INT_RELAY_6:
        case EXT_RELAY_1:
        case EXT_RELAY_2:
        case EXT_RELAY_3:
        case EXT_RELAY_4:
        case EXT_RELAY_5:
//        case OUT_ALARM:
//        case OUT_SIGNAL:
        {
            // обновить параметр состояния реле (кнопки)
//            module->updateStateParam(stateParam);
            module->updateStateParam(stateParamPtr);
            // инициировать событие изменения состояния реле
            // только при первом ответе от модуля
//            if (initBuffer)
            {
                // получить состояние реле (кнопки)
                bool pressed = (stateParam->state() == RELAY_ON);
                // инициировать событие изменения состояния кнопки
                emit relayStateChanged(module->displayName(), stateParam->id(), pressed);
            }
            break;
        }
        default:
            break;
    }
}


void IssoModulesController::resetMsState(const QHostAddress &moduleIp, int msId)
{
    // найти модуль по адресу
    IssoModule* module = findModuleByAddress(moduleIp);
    if (!module)
        return;
    //
    // получить параметр данных мультидатчиков
    auto msParam = dynamic_cast<IssoMultiSensorStateParam*>(module->stateParamById(MULTISENSOR));
    // получить коллекцию данных мультидатчиков модуля
    auto msDataMap = (msParam ? msParam->dataMap() : QMap<quint8, IssoMultiSensorData>());
    // задать данные текущего мультидатчика в состоянии Норма
    msDataMap.insert(msId, IssoMultiSensorData(msId, 0, 0));
    // сформировать буфер
    IssoParamBuffer buffer;
    buffer.insertParam(new IssoMultiSensorStateParam(MULTISENSOR, msDataMap));
    // обработать буфер
    parseParamBuffer(module, buffer);
}


bool IssoModulesController::saveThresholdData(const QHostAddress& moduleIp,
                                              int multiSensorId,
                                              const QByteArray &thresholdData)
{
    // найти модуль по IP-адресу
    auto module = findModuleByAddress(moduleIp);
    if (!module)
        return false;
    // получить конфиг мультидатчика
    auto msCfg = (m_object ? m_object->findMultiSensorCfgByModuleId(module->uniqueId(),
                                                               multiSensorId)
                           : nullptr);
    if (!msCfg)
        return false;
    // получить массив данных конфига мультидатчика
    msCfg->setThresholdData(thresholdData);
    return true;
}

