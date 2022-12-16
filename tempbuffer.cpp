enum IssoCmdId
{
    CMD_INFO,
    CMD_DESC_GET,
    CMD_DESC_SET,
    CMD_ALARM,
    CMD_RELAY,
    CMD_VOICE,
    CMD_MS_INFO,
    CMD_MS_INFO_RAW,
    CMD_MS_POLL_ADDRS,
    CMD_MS_SINGLE_INPUT_REG_GET,
    CMD_MS_SINGLE_HOLD_REG_GET,
    CMD_MS_SINGLE_HOLD_REG_SET,
    CMD_MS_BLOCK_HOLD_REG_GET,
    CMD_MS_BLOCK_HOLD_REG_SET,
    CMD_MULTISENSOR_EXIST_GET,
    // CMD_MS_VALID_SLAVES_GET,
    CMD_UNDEFINED = 0xFF
};
Q_DECLARE_METATYPE(IssoCmdId)

enum IssoDigitalSensorId
{
    DIGITALSENSOR_UNDEFINED,
    DIGITALSENSOR_SMOKE_O_LED_ON,
    DIGITALSENSOR_SMOKE_O_LED_OFF,
    DIGITALSENSOR_SMOKE_O,
    DIGITALSENSOR_TEMP_A,
    DIGITALSENSOR_TEMP_D,
    DIGITALSENSOR_SMOKE_E,
    DIGITALSENSOR_FLAME_1,
    DIGITALSENSOR_FLAME_2,
    DIGITALSENSOR_FLAME_3,
    DIGITALSENSOR_FLAME_4,
    DIGITALSENSOR_FLAME_5,
    DIGITALSENSOR_FLAME_6,
    DIGITALSENSOR_FLAME_MIN_VAL,
    DIGITALSENSOR_FLAME_STD_DEV,
    DIGITALSENSOR_CO,
    DIGITALSENSOR_VOC
};
Q_DECLARE_METATYPE(IssoDigitalSensorId)
enum IssoChainType
{
    SMOKE_CHAIN,
    MULTICRITERIAL,
    ALARM_BUTTON_CHAIN,
    MOVE_DETECTOR_CHAIN,
    TEMP_CHAIN,
    //
    VALVE_CHAIN,
    DOOR_CHAIN,
    PUMP_CHAIN,
    LATCH_CHAIN,
    //
    INTERNAL_SENSOR
};
Q_DECLARE_METATYPE(IssoChainType)

QDataStream &operator<<(QDataStream &out, IssoChainType type);
QDataStream &operator>>(QDataStream &in, IssoChainType& type);




enum class IssoGuiMode
{
    STANDART_MONITOR,
    TOUCH_SCREEN,
    TABLET
};
Q_DECLARE_METATYPE(IssoGuiMode)


IssoModuleRequest IssoExchangeManager::makeMultiSensorPollingAddrsRequest(const QHostAddress &ip, quint16 port,
                                                                          const QByteArray &multiSensorIds)
{
    // создать буфер параметров запроса
    IssoParamBuffer params;
    // добавить параметр адреса мультидатчика
    // MULTISENSOR_ADDRS массив адресов мультидатчиков
    params.insertParam(new IssoSizedArrayParam(MULTISENSOR_ADDRS, multiSensorIds));
    // сформировать запрос
    return IssoModuleRequest(ip, port, nextPackNum(), CMD_MS_POLL_ADDRS, params);
}
/**
 * @brief IssoExchangeManager::moduleMultiSensorAddrs получить конфиг адресного шлейфа
 * @param module
 * @return
 */
IssoModuleRequest::IssoModuleRequest(QHostAddress addr, quint16 port,
                                     quint16 number, IssoCmdId cmdId,
                                     const IssoParamBuffer& params)
    : IssoModulePacket(addr, port, number, IssoModuleCommand(cmdId, params))
{
    m_crc = calcCrc16();
}

IssoModulePacket::IssoModulePacket(QHostAddress addr, quint16 port, quint16 number, IssoModuleCommand command)
    : m_address(addr),
      m_port(port),
      m_number(number),
      m_command(command)
{
}

IssoModuleCommand::IssoModuleCommand(IssoCmdId cmdId, IssoParamBuffer params)
    : IssoModuleCommand(cmdId)
{
    m_paramBuffer = params;
}

IssoModuleCommand::IssoModuleCommand(IssoCmdId cmdId)
    : m_id(cmdId)
{
}

QByteArray IssoExchangeManager::moduleMultiSensorAddrs(IssoModule *module)
{
    if (!m_object || !module)
        return QByteArray();

    // получить конфиг адресного шлейфа
    auto msChainCfg = m_object->moduleAddressChainConfig(module->moduleConfigName());
    // вернуть массив ID мультидатчиков конфига адресного шлейфа,
    // либо пустой массив, если конфиг не найден
    return (msChainCfg ? msChainCfg->multiSensorIdArray() : QByteArray());

//    // получить конфиг адресного шлейфа, содержащегося в конфиге модуля
//    IssoChainConfig* chainCfg =
//            m_object->moduleChainConfig(module->getModuleConfigName(), MULTISENSOR);
//    // привести к типу адресного шлейфа
//    IssoDigitalChainConfig* digitalCfg = dynamic_cast<IssoDigitalChainConfig*>(chainCfg);
//    // вернуть массив ID мультидатчиков конфига адресного шлейфа,
//    // либо пустой массив, если конфиг не найден
//    return (digitalCfg ? digitalCfg->multiSensorIds() : QByteArray());
}
IssoModuleReply* IssoRequestSender::sendRequest(const IssoModuleRequest &request, bool &ok)
{
//    qDebug() << "Send to socket: " << request.command().bytes().toHex(' ').toUpper() << endl;

//    qDebug() << QString("Request outgoing: %1:%2")
//                        .arg(mp_sock->localAddress().toString())
//                        .arg(mp_sock->localPort()) << endl;

    ok = false;
    // если датаграмма не отправилась, вернуть пустой указатель
    if (mp_sock->writeDatagram(request.bytes(), request.address(), request.port()) == -1)
        return nullptr;
    //
    // известить об успешной отправке запроса
    emit requestSent(request);
    // если запрос широковещательный, то не ждем ответа
    if (request.address().isEqual(QHostAddress::Broadcast,
                                  QHostAddress::ConvertV4MappedToIPv4))
        return nullptr;

    // считать ответный пакет
    IssoModuleReply* reply = receiveReply();
    // корректность ответа
    ok = (reply && (reply->number() == request.number())); //&& (reply->isCorrect()));
//    // отладка
//    if (reply)
//    {
//        qDebug() << QString("reply num = %1,   request num = %2,    reply correct = %3")
//                    .arg(reply->number()).arg(request.number()).arg(reply->isCorrect());
//    }
    // вернуть ответ
    return reply;
}
QSharedPointer<IssoParam> IssoParamBuffer::insertParam(IssoParam* param)
{
    if (!param)
        return QSharedPointer<IssoParam>();

    QSharedPointer<IssoParam> p(param);
    m_params.replace(param->id(), p);
    return p;
}

IssoModuleRequest IssoExchangeManager::makeMultiSensorPollingAddrsRequest(const QHostAddress &ip, quint16 port,
                                                                          const QByteArray &multiSensorIds)
{
    // создать буфер параметров запроса
    IssoParamBuffer params;
    // добавить параметр адреса мультидатчика
    params.insertParam(new IssoSizedArrayParam(MULTISENSOR_ADDRS, multiSensorIds));
    // сформировать запрос
    return IssoModuleRequest(ip, port, nextPackNum(), CMD_MS_POLL_ADDRS, params);
}
 

 if (!module)
        return;
    // получить список мультидатчиков модуля
    QByteArray multiSensorIds = moduleMultiSensorAddrs(module);
    // сформировать запрос установки адресов мультидатчика для опроса
    auto request = makeMultiSensorPollingAddrsRequest(module->ip(), module->port(), multiSensorIds);
    // асинхронно добавить запрос в очередь
    addToUserQueueAsync(request);