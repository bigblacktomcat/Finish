#include "issoparambuffer.h"


IssoParamBuffer::IssoParamBuffer()
{
}


IssoParamBuffer::IssoParamBuffer(const QByteArray &buffer)
{
    parseBytes(buffer);
}


IssoParamBuffer::IssoParamBuffer(const QMultiMap<IssoParamId, QSharedPointer<IssoParam> > &params)
    : m_params(params)
{
}


IssoParamBuffer::IssoParamBuffer(const IssoParamBuffer &other)
{
    this->m_params = other.params();
}


QMultiMap<IssoParamId, QSharedPointer<IssoParam> > IssoParamBuffer::params() const
{
    return m_params;
}


void IssoParamBuffer::setParams(const QMultiMap<IssoParamId, QSharedPointer<IssoParam> > &params)
{
    m_params = params;
}


void IssoParamBuffer::clearParams()
{
    m_params.clear();
}


QSharedPointer<IssoParam> IssoParamBuffer::insertParam(IssoParam* param)
{
    if (!param)
        return QSharedPointer<IssoParam>();

    QSharedPointer<IssoParam> p(param);
    m_params.replace(param->id(), p);
    return p;
}


void IssoParamBuffer::insertParamPtr(QSharedPointer<IssoParam> paramPtr)
{
    m_params.replace(paramPtr.data()->id(), paramPtr);
}


void IssoParamBuffer::removeParam(IssoParamId id)
{
    m_params.remove(id);
}


int IssoParamBuffer::paramCount() const
{
    return m_params.size();
}


bool IssoParamBuffer::isEmpty() const
{
    return m_params.isEmpty();
}


IssoParam* IssoParamBuffer::paramById(const IssoParamId &paramId)
{
    QSharedPointer<IssoParam> p = m_params.value(paramId);
    IssoParam* param = p.isNull() ? nullptr : p.data();
    return param;
}


QList<QSharedPointer<IssoParam> > IssoParamBuffer::paramsById(IssoParamId paramId)
{
    return m_params.values(paramId);
}


IssoParam *IssoParamBuffer::paramByIndex(int index) const
{
    if ((index < 0) || (index >= m_params.values().size()))
        return nullptr;
    else
        return m_params.values().at(index).data();
}


QSharedPointer<IssoParam> IssoParamBuffer::paramPtrByIndex(int index) const
{
    if ((index < 0) || (index >= m_params.values().size()))
        return QSharedPointer<IssoParam>();
    else
        return m_params.values().at(index);
}


bool IssoParamBuffer::paramExists(const IssoParamId &paramId) const
{
    return m_params.contains(paramId);
}


QByteArray IssoParamBuffer::bytes() const
{
    // создать массив полученного размера
    QByteArray ba;
    // создать поток записи в массив
    QDataStream stream(&ba, QIODevice::WriteOnly);
    // записать все параметры в поток
    foreach (QSharedPointer<IssoParam> p, m_params.values())
    {
        p.data()->saveToStream(&stream);
    }
    // вернуть массив данных
    return ba;
}


IssoModuleState IssoParamBuffer::getResultState()
{
    // если буфер параметров пуст, значит модуль неактивен (INACTIVE),
    // иначе рассчитать результирующее состояние (исходное ACTIVE - наименее приоритетное)
    IssoModuleState resultState = (m_params.isEmpty() ? INACTIVE : ACTIVE);
    // обойти коллекцию параметров
    foreach (IssoParamId paramId, m_params.keys())
    {
        // обойти параметры, соответствующие ключу
        foreach (QSharedPointer<IssoParam> paramPtr, m_params.values(paramId))
        {
            // преобразовать в параметр состояния
            IssoSensorStateParam* stateParam =
                    dynamic_cast<IssoSensorStateParam*>(paramPtr.data());
            if (!stateParam)
                continue;
            // получить состояние модуля по состоянию текущего датчика
            IssoModuleState newState = IssoCommonData::moduleStateByParamState(stateParam->state());
            // сохраняется состояние модуля, значение которого меньше (приоритет выше)
            resultState = (newState < resultState ? newState : resultState);

//            qDebug() << IssoCommonData::stringByParamId(paramId) << endl
//                     << IssoCommonData::stringBySensorState(stateParam->state()) << endl;
        }
    }
    return resultState;
}


void IssoParamBuffer::parseBytes(const QByteArray &bytes)
{
    QDataStream stream(bytes);
    while(!stream.atEnd())
    {
        // считать id параметра без смещения позиции
        quint8 idValue;
        stream.device()->peek((char*)&idValue, sizeof(idValue));
        // преобразовать значение в тип id параметра
        IssoParamId id = static_cast<IssoParamId>(idValue);
        // указатель на параметр
        IssoParam* p = nullptr;
        // считать весь параметр со смещением позиции
        switch(id)
        {
            // ip-адрес
            case DESC_IP:
            case DESC_SERVER_IP:
            {
                // создать параметр типа IP-адрес
                p = new IssoIpParam();
                break;
            }
            // МАС-адрес
            case DESC_MAC:
            {
                // создать параметр типа массив
                p = new IssoArrayParam(6);
                break;
            }
            // порт
            case DESC_PORT:
            case DESC_SERVER_PORT:
            {
                p = new IssoShortParam();
                break;
            }
            // номер телефона
            case DESC_PHONE:
            {
                p = new IssoArrayParam(12);
                break;
            }
            // разрешение использовать телефон
            case DESC_PHONE_ENABLED:
            {
                p = new IssoBoolParam();
                break;
            }
            // контрольная сумма
            case DESC_CRC:
            // номер записи в памяти контроллера
            case STORED_REC_NUM:
            // тип воспроизведения записи
            case PLAYBACK_TYPE:
            // общее число блоков данных
            case REC_BLOCK_TOTAL:
            // номер текущего блока данныхё
            case REC_BLOCK_CURRENT:
            // адрес мультидатчика
            case MULTISENSOR_ADDR:
            {
                p = new IssoByteParam();
                break;
            }
            // состояния параметров (датчиков и реле)
            case BTN_FIRE:
            case BTN_SECURITY:
            case SENSOR_SMOKE:
            case SENSOR_CO:
            case ANALOG_CHAIN_1:
            case ANALOG_CHAIN_2:
            case ANALOG_CHAIN_3:
            case ANALOG_CHAIN_4:
            case ANALOG_CHAIN_5:
            case ANALOG_CHAIN_6:
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
            case SUPPLY_EXTERNAL:
            case SUPPLY_INTERNAL:
            case SENSOR_OPEN:
            case VIBRATION:
            {
                p = new IssoSensorStateParam();
                break;
            }
            // расширенные состояния
            // (датчиков напряжения и температуры)
            case VOLT_BATTERY:
            case VOLT_BUS:
            case VOLT_SENSORS:
            case TEMP_OUT:
            {
                p = new IssoSensorExtStateParam();
                break;
            }
            // размер блока данных записи
            case REC_BLOCK_SIZE:
            {
                // считать параметр размера блока данных
                IssoShortParam* sizeParam = new IssoShortParam();
                // если размер блока данных успешно считан
                if (sizeParam->loadFromStream(&stream))
                {
                    // добавить параметр в карту
                    m_params.insert(sizeParam->id(), QSharedPointer<IssoParam>(sizeParam));
                    //
                    // считать id следующего параметра без смещения позиции
                    quint8 blockIdVal;
                    stream.device()->peek((char*)&blockIdVal, sizeof(blockIdVal));
                    // преобразовать значение в тип id параметра
                    IssoParamId blockId = static_cast<IssoParamId>(blockIdVal);
                    // убедиться, что далее идет блок данных
                    if (blockId != REC_BLOCK_DATA)
                        break;
                    //
                    // создать параметр для считывания блока данных
                    IssoArrayParam* dataParam = new IssoArrayParam(sizeParam->shortValue());
                    // если блок данных успешно считан
                    if (dataParam->loadFromStream(&stream))
                        // добавить параметр в карту
                        m_params.insert(dataParam->id(), QSharedPointer<IssoParam>(dataParam));
                }
                break;
            }
            // адресный шлейф с набором мультидатчиков
            case MULTISENSOR:
            {
                p = new IssoMultiSensorStateParam();
                break;
            }
            // расширенные данные мультидатчика
            case MULTISENSOR_STATE_RAW:
            {
                p = new IssoMultiSensorRawDataParam();
                break;
            }
            // массив адресов мультидатчиков
            case MULTISENSOR_ADDRS:
            {
                p = new IssoSizedArrayParam();
                break;
            }
            // данные о существовании датчика
            case MULTISENSOR_EXIST_DATA:
            {
                p = new IssoMultiSensorExistDataParam();
                break;
            }
            // адрес регистра
            case SINGLE_REG_ADDR:
            {
                p = new IssoSingleRegAddrParam();
                break;
            }
            // данные регистра
            case SINGLE_REG_DATA:
            {
                p = new IssoSingleRegInfoParam();
                break;
            }
            // адрес начала блока регистров
            case BLOCK_REG_ADDR:
            {
                p = new IssoBlockRegAddrParam();
                break;
            }
            // данные блока регистров
            case BLOCK_REG_DATA:
            {
                p = new IssoBlockRegInfoParam();
                break;
            }
            // другие значения встречаться не должны
            default:
                return;
        }
        // если параметр создан и успешно загружен
        if (p && p->loadFromStream(&stream))
        {
            // добавить параметр в карту
            m_params.insert(p->id(), QSharedPointer<IssoParam>(p));
        }

//        qDebug() << p->description();
    }
}
