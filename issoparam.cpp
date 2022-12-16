#include "issoparam.h"


//========== IssoParam ==========

IssoParam::IssoParam() : IssoParam(PARAM_UNDEFINED)
{
}


IssoParam::IssoParam(IssoParamId id)
    : m_id(id)
{
}


IssoParam::IssoParam(const IssoParam &other) : IssoParam(other.id())
{
}


IssoParam::~IssoParam()
{
//    qDebug() << "Деструктор IssoParam: " << m_id << endl;
}


IssoParamId IssoParam::id() const
{
    return m_id;
}


bool IssoParam::loadFromStream(QDataStream *stream)
{
    if (stream->device()->bytesAvailable() < 1)
        return false;
    // считать из потока id параметра (1 байт)
    *stream >> m_id;
    return true;
}


void IssoParam::saveToStream(QDataStream *stream)
{
    // записать в поток id параметра (1 байт)
    *stream << m_id;
}


int IssoParam::size()
{
    return 1;
}


QString IssoParam::description()
{
    return QString("IssoParam:   id = %1").arg(m_id);
}


QByteArray IssoParam::bytes()
{
    // создать поток для записи
    QByteArray ba;
    QDataStream stream(&ba, QIODevice::WriteOnly);
    // записать в данные поток
    saveToStream(&stream);
    // вернуть заполненный массив
    return ba;
}



//========== IssoSensorStateParam ==========

IssoSensorStateParam::IssoSensorStateParam()
    : IssoSensorStateParam(PARAM_UNDEFINED, STATE_UNDEFINED)
{
}


IssoSensorStateParam::IssoSensorStateParam(IssoParamId id, IssoState state)
    : IssoParam(id),
      m_state(state)
{
}


IssoSensorStateParam::IssoSensorStateParam(const IssoSensorStateParam &other)
    : IssoParam(other),
      m_state(other.state())
{
}


IssoSensorStateParam::~IssoSensorStateParam()
{
}


IssoState IssoSensorStateParam::state() const
{
    return m_state;
}


bool IssoSensorStateParam::loadFromStream(QDataStream *stream)
{
    if (!IssoParam::loadFromStream(stream))
        return false;
    if (stream->device()->bytesAvailable() < 1)
        return false;
    // считать из потока состояние параметра (1 байт)
    quint8 stateVal;
    *stream >> stateVal;
    m_state = static_cast<IssoState>(stateVal);
    return true;
}


void IssoSensorStateParam::saveToStream(QDataStream *stream)
{
    IssoParam::saveToStream(stream);
    //
    // записать в поток состояние параметра (1 байт)
    *stream << static_cast<quint8>(m_state);
}


int IssoSensorStateParam::size()
{
    return IssoParam::size() + 1;
}


QString IssoSensorStateParam::description()
{
    return QString("IssoSensorStateParam:   id = %1;   state = %2")
                    .arg(m_id)
                    .arg(m_state, 2, 16, QLatin1Char( '0' ));
}



//========== IssoSensorExtStateParam ==========

IssoSensorExtStateParam::IssoSensorExtStateParam()
    : IssoSensorExtStateParam(PARAM_UNDEFINED, STATE_UNDEFINED, 0)
{
}


IssoSensorExtStateParam::IssoSensorExtStateParam(IssoParamId id, IssoState state, float value)
    : IssoSensorStateParam(id, state),
      m_floatValue(value)
{
}


IssoSensorExtStateParam::~IssoSensorExtStateParam()
{
}


bool IssoSensorExtStateParam::loadFromStream(QDataStream *stream)
{
    if (!IssoSensorStateParam::loadFromStream(stream))
        return false;
    if (stream->device()->bytesAvailable() < sizeof(m_floatValue))
        return false;
    // считать из потока float-значение
    // числа с плавающей точкой читать ТОЛЬКО так
    // (при использовании оператора >> позиция потока смещается не на 4, а на 8 байт)
    stream->readRawData((char*)&m_floatValue, sizeof(m_floatValue));
    return true;
}


void IssoSensorExtStateParam::saveToStream(QDataStream *stream)
{
    IssoSensorStateParam::saveToStream(stream);
    //
    // записать в поток float-значение
    stream->writeRawData((char*)&m_floatValue, sizeof(m_floatValue));
}


int IssoSensorExtStateParam::size()
{
    return IssoSensorStateParam::size() + sizeof(m_floatValue);
}


QString IssoSensorExtStateParam::description()
{
    return QString("IssoSensorExtStateParam:   id = %1;   state = %2;   floatValue = %3")
                    .arg(m_id)
                    .arg(m_state, 2, 16, QLatin1Char( '0' ))
                    .arg(m_floatValue);
}


float IssoSensorExtStateParam::value() const
{
    return m_floatValue;
}



//========== IssoIpParam ==========

IssoIpParam::IssoIpParam() : IssoIpParam(PARAM_UNDEFINED, 0)
{
}


IssoIpParam::IssoIpParam(IssoParamId id, quint32 ip)
    : IssoParam(id),
      m_ip(ip)
{
}


IssoIpParam::~IssoIpParam()
{
//    qDebug() << "Деструктор IssoIpParam" << endl;
}


quint32 IssoIpParam::ip() const
{
    return m_ip;
}


bool IssoIpParam::loadFromStream(QDataStream *stream)
{
    if (!IssoParam::loadFromStream(stream))
        return false;
    if (stream->device()->bytesAvailable() < sizeof(m_ip))
        return false;
    // ip-адрес
    *stream >> m_ip;
    return true;
}


void IssoIpParam::saveToStream(QDataStream *stream)
{
    IssoParam::saveToStream(stream);
    //
    // ip-адрес
    *stream << m_ip;
}


int IssoIpParam::size()
{
    return IssoParam::size() + sizeof(m_ip);
}


QString IssoIpParam::description()
{
    return QString("IssoIpParam:   id = %1;   ip = %2")
                    .arg(m_id).arg(QHostAddress(m_ip).toString());
}


//========== IssoByteParam ==========

IssoByteParam::IssoByteParam() : IssoByteParam(PARAM_UNDEFINED, 0)
{
}


IssoByteParam::IssoByteParam(IssoParamId id, quint8 byteValue)
    : IssoParam(id),
      m_byteValue(byteValue)
{
}


IssoByteParam::~IssoByteParam()
{
}


quint8 IssoByteParam::byteValue() const
{
    return m_byteValue;
}


bool IssoByteParam::loadFromStream(QDataStream *stream)
{
    if (!IssoParam::loadFromStream(stream))
        return false;
    if (stream->device()->bytesAvailable() < sizeof(m_byteValue))
        return false;
    // 1-байтное значение
    *stream >> m_byteValue;
    return true;
}


void IssoByteParam::saveToStream(QDataStream *stream)
{
    IssoParam::saveToStream(stream);
    //
    // 1-байтное значение
    *stream << m_byteValue;
}


int IssoByteParam::size()
{
    return IssoParam::size() + sizeof(m_byteValue);
}


QString IssoByteParam::description()
{
    return QString("IssoByteParam:   id = %1;   byteValue = %2")
                    .arg(m_id).arg(m_byteValue);
}


//========== IssoSingleRegAddrParam ==========

IssoSingleRegAddrParam::IssoSingleRegAddrParam()
    : IssoSingleRegAddrParam(PARAM_UNDEFINED, 0, 0)
{
}


IssoSingleRegAddrParam::IssoSingleRegAddrParam(IssoParamId id,
                                           quint8 multiSensorId,
                                           quint8 regAddr)
    : IssoByteParam(id, multiSensorId),
      m_regAddress(regAddr)
{
}


IssoSingleRegAddrParam::~IssoSingleRegAddrParam()
{
}


bool IssoSingleRegAddrParam::loadFromStream(QDataStream *stream)
{
    if (!IssoByteParam::loadFromStream(stream))
        return false;
    if (stream->device()->bytesAvailable() < sizeof(m_regAddress))
        return false;
    // номер регистра
    *stream >> m_regAddress;
    return true;
}


void IssoSingleRegAddrParam::saveToStream(QDataStream *stream)
{
    IssoByteParam::saveToStream(stream);
    //
    // номер регистра
    *stream << m_regAddress;
}


int IssoSingleRegAddrParam::size()
{
    return IssoByteParam::size() + sizeof(m_regAddress);
}


QString IssoSingleRegAddrParam::description()
{
    return QString("IssoSingleRegAddrParam:   id = %1;   addr = %2;   regAddress = %3")
                    .arg(m_id).arg(m_byteValue).arg(m_regAddress);
}


quint8 IssoSingleRegAddrParam::regAddress() const
{
    return m_regAddress;
}


quint8 IssoSingleRegAddrParam::multiSensorId() const
{
    return m_byteValue;
}


//========== IssoSingleRegInfoParam ==========

IssoSingleRegInfoParam::IssoSingleRegInfoParam()
    :IssoSingleRegInfoParam(PARAM_UNDEFINED, 0, 0, 0)
{
}


IssoSingleRegInfoParam::IssoSingleRegInfoParam(IssoParamId id, quint8 multiSensorId,
                                           quint8 regAddress, quint16 regValue)
    : IssoSingleRegAddrParam(id, multiSensorId, regAddress),
      m_regValue(regValue)
{
}


IssoSingleRegInfoParam::~IssoSingleRegInfoParam()
{
}


bool IssoSingleRegInfoParam::loadFromStream(QDataStream *stream)
{
    if (!IssoSingleRegAddrParam::loadFromStream(stream))
        return false;
    if (stream->device()->bytesAvailable() < sizeof(m_regValue))
        return false;
    // значение регистра
    *stream >> m_regValue;
    return true;
}


void IssoSingleRegInfoParam::saveToStream(QDataStream *stream)
{
    IssoSingleRegAddrParam::saveToStream(stream);
    //
    // значение регистра
    *stream << m_regValue;
}


int IssoSingleRegInfoParam::size()
{
    return IssoSingleRegAddrParam::size() + sizeof(m_regValue);
}


QString IssoSingleRegInfoParam::description()
{
    return QString("IssoSingleRegInfoParam:   id = %1;   addr = %2;   "
                                    "regAddress = %3;   regValue = %4")
            .arg(m_id).arg(m_byteValue).arg(m_regAddress).arg(m_regValue);
}


quint8 IssoSingleRegInfoParam::regValue() const
{
    return m_regValue;
}


//========== IssoBlockRegAddrParam ==========

IssoBlockRegAddrParam::IssoBlockRegAddrParam()
    : IssoSingleRegAddrParam(PARAM_UNDEFINED, 0, 0)
{
}


IssoBlockRegAddrParam::IssoBlockRegAddrParam(IssoParamId id, quint8 multiSensorId,
                                             quint8 regAddress, quint8 regCount)
    : IssoSingleRegAddrParam(id, multiSensorId, regAddress),
      m_regCount(regCount)
{
}


IssoBlockRegAddrParam::~IssoBlockRegAddrParam()
{
}


bool IssoBlockRegAddrParam::loadFromStream(QDataStream *stream)
{
    if (!IssoSingleRegAddrParam::loadFromStream(stream))
        return false;
    if (stream->device()->bytesAvailable() < sizeof(m_regCount))
        return false;
    // количество регистров
    *stream >> m_regCount;
    return true;
}


void IssoBlockRegAddrParam::saveToStream(QDataStream *stream)
{
    IssoSingleRegAddrParam::saveToStream(stream);
    //
    // количество регистров
    *stream << m_regCount;
}


int IssoBlockRegAddrParam::size()
{
    return IssoSingleRegAddrParam::size() + sizeof(m_regCount);
}


QString IssoBlockRegAddrParam::description()
{
    return QString("IssoBlockRegAddrParam:   id = %1;   addr = %2;   "
                                    "regAddress = %3;   regCount = %4")
            .arg(m_id).arg(m_byteValue).arg(m_regAddress).arg(m_regCount);
}


quint8 IssoBlockRegAddrParam::regCount() const
{
    return m_regCount;
}



//========== IssoBlockRegInfoParam ==========

IssoBlockRegInfoParam::IssoBlockRegInfoParam()
{
}


IssoBlockRegInfoParam::IssoBlockRegInfoParam(IssoParamId id, quint8 multiSensorId,
                                             quint8 regAddress, const QByteArray &regArray)
    : IssoBlockRegAddrParam(id, multiSensorId, regAddress,
                            regArray.length() / sizeof(quint16)), // длина массива / размер регистра
      m_regArray(regArray)
{
}


IssoBlockRegInfoParam::~IssoBlockRegInfoParam()
{
}


bool IssoBlockRegInfoParam::loadFromStream(QDataStream *stream)
{
    if (!IssoBlockRegAddrParam::loadFromStream(stream))
        return false;
    // задать размер массива (количество регистров * размер регистра)
    m_regArray.resize(m_regCount * sizeof(quint16));
    //
    // проверить возможность чтения
    if (stream->device()->bytesAvailable() < sizeof(m_regArray))
        return false;
    // массив регистров
    stream->readRawData(m_regArray.data(), m_regArray.length());
    return true;
}


void IssoBlockRegInfoParam::saveToStream(QDataStream *stream)
{
    IssoBlockRegAddrParam::saveToStream(stream);
    //
    // массив регистров
    stream->writeRawData(m_regArray.data(), m_regArray.length());
}


int IssoBlockRegInfoParam::size()
{
    return IssoBlockRegAddrParam::size() + m_regArray.length();
}


QString IssoBlockRegInfoParam::description()
{
    return QString("IssoBlockRegInfoParam:   id = %1;   addr = %2;   "
                                    "regAddress = %3;   regCount = %4    "
                                    "regArray = %5")
            .arg(m_id).arg(m_byteValue).arg(m_regAddress).arg(m_regCount)
            .arg(QString(m_regArray.toHex(' ').toUpper()));
}


QByteArray IssoBlockRegInfoParam::regArray() const
{
    return m_regArray;
}


//========== IssoShortParam ==========

IssoShortParam::IssoShortParam() : IssoShortParam(PARAM_UNDEFINED, 0)
{
}


IssoShortParam::IssoShortParam(IssoParamId id, quint16 shortValue)
    : IssoParam(id),
      m_shortValue(shortValue)
{
}


IssoShortParam::~IssoShortParam()
{
}


quint16 IssoShortParam::shortValue() const
{
    return m_shortValue;
}


bool IssoShortParam::loadFromStream(QDataStream *stream)
{
    if (!IssoParam::loadFromStream(stream))
        return false;
    if (stream->device()->bytesAvailable() < sizeof(m_shortValue))
        return false;
    // 2-байтное значение
    *stream >> m_shortValue;
    return true;
}


void IssoShortParam::saveToStream(QDataStream *stream)
{
    IssoParam::saveToStream(stream);
    //
    // 2-байтное значение
    *stream << m_shortValue;
}


int IssoShortParam::size()
{
    return IssoParam::size() + sizeof(m_shortValue);
}


QString IssoShortParam::description()
{
    return QString("IssoShortParam:   id = %1;   shortValue = %2")
                    .arg(m_id).arg(m_shortValue);
}


//========== IssoArrayParam ==========

IssoArrayParam::IssoArrayParam(int size) : IssoParam(PARAM_UNDEFINED)
{
    m_array.resize(size);
}


IssoArrayParam::IssoArrayParam(IssoParamId id, const QByteArray &array)
    : IssoParam(id),
      m_array(array)
{
}


IssoArrayParam::~IssoArrayParam()
{
}


bool IssoArrayParam::loadFromStream(QDataStream *stream)
{
    if (!IssoParam::loadFromStream(stream))
        return false;
    if (stream->device()->bytesAvailable() < m_array.length())
        return false;
    // массив
    stream->readRawData(m_array.data(), m_array.length());
    return true;
}


void IssoArrayParam::saveToStream(QDataStream *stream)
{
    IssoParam::saveToStream(stream);
    //
    // массив
    stream->writeRawData(m_array.data(), m_array.length());
}


int IssoArrayParam::size()
{
    return IssoParam::size() + m_array.length();
}


QString IssoArrayParam::description()
{
    return QString("IssoArrayParam:   id = %1;   array = %2")
                    .arg(m_id).arg(QString(m_array.toHex(' ').toUpper()));
}


QByteArray IssoArrayParam::array() const
{
    return m_array;
}


//========== IssoSizedArrayParam ==========

IssoSizedArrayParam::IssoSizedArrayParam()
    : IssoArrayParam(0)
{
}


IssoSizedArrayParam::IssoSizedArrayParam(IssoParamId id, const QByteArray &array)
    : IssoArrayParam(id, array)
{
}


IssoSizedArrayParam::~IssoSizedArrayParam()
{
}


bool IssoSizedArrayParam::loadFromStream(QDataStream *stream)
{
    if (!IssoParam::loadFromStream(stream))
        return false;
    // размер массива
    quint8 arraySize;
    // убедиться в наличии данных для считывания
    if (stream->device()->bytesAvailable() < sizeof(arraySize))
        return false;
    // считать размер массива
    *stream >> arraySize;
    // изменить размер массива
    m_array.resize(arraySize);
    //
    // убедиться в наличии данных для считывания
    if (stream->device()->bytesAvailable() < m_array.length())
        return false;
    // считать массив
    stream->readRawData(m_array.data(), m_array.length());
    return true;
}


void IssoSizedArrayParam::saveToStream(QDataStream *stream)
{
    IssoParam::saveToStream(stream);
    //
    // записать размер массива
    quint8 arraySize = m_array.length();
    *stream << arraySize;
    // массив
    stream->writeRawData(m_array.data(), m_array.length());
}


int IssoSizedArrayParam::size()
{
    return IssoArrayParam::size() + sizeof(quint8);
}


QString IssoSizedArrayParam::description()
{
    return QString("IssoSizedArrayParam:   id = %1;   size = %2;   array = %3")
                    .arg(m_id)
                    .arg(m_array.length())
                    .arg(QString(m_array.toHex(' ').toUpper()));
}



//========== IssoBoolParam ==========

IssoBoolParam::IssoBoolParam() : IssoByteParam()
{
}


IssoBoolParam::IssoBoolParam(IssoParamId id, bool value)
    : IssoByteParam(id, value ? 1 : 0)
{
}


IssoBoolParam::~IssoBoolParam()
{
}


QString IssoBoolParam::description()
{
    return QString("IssoBoolParam:   id = %1;   boolValue = %2")
                    .arg(m_id).arg(m_byteValue == 1 ? "true" : "false");
}


bool IssoBoolParam::boolValue()
{
    return (m_byteValue != 0);
}



//========== IssoMultiSensorData ==========


IssoMultiSensorData::IssoMultiSensorData()
    : IssoMultiSensorData(0, 0, 0)
{
}


IssoMultiSensorData::IssoMultiSensorData(quint8 multiSensorId,
                                         quint8 state, quint16 stateDetails)
    : m_multiSensorId(multiSensorId),
      m_stateValue(state),
      m_detailsValue(stateDetails)
{
    // сформировать коллекцию состояний каналов
    m_detailsMap = makeDigitalSensorStates(stateDetails);
}


quint8 IssoMultiSensorData::multiSensorId() const
{
    return m_multiSensorId;
}


void IssoMultiSensorData::setMultiSensorId(const quint8 &multiSensorId)
{
    m_multiSensorId = multiSensorId;
}


quint8 IssoMultiSensorData::stateValue() const
{
    return m_stateValue;
}


void IssoMultiSensorData::setStateValue(const quint8 &state)
{
    m_stateValue = state;
}


quint16 IssoMultiSensorData::detailsValue() const
{
    return m_detailsValue;
}


void IssoMultiSensorData::setDetailsValue(const quint16 &stateDetails)
{
    m_detailsValue = stateDetails;
}



bool IssoMultiSensorData::valid() const
{
    return ((m_detailsValue & 0x8000) == 0);
}


void IssoMultiSensorData::setValid(bool valid)
{
    quint16 value = (valid ? 0 : 0x8000);
    m_detailsValue = ((m_detailsValue & 0x7FFF) | value);
}


IssoState IssoMultiSensorData::state()
{
    IssoState res;
    switch(m_stateValue)
    {
        // Норма
        case 0:
        {
            res = STATE_NORMAL;
            break;
        }
        // Внимание
        case 1:
        {
            res = STATE_WARNING;
            break;
        }
        // Тревога
        case 2:
        {
            res = STATE_ALARM;
            break;
        }
        // Неисправность
        case 3:
        {
            res = STATE_FAIL;
            break;
        }
        // Не в сети (Обрыв / КЗ)
        case 4:
        {
            res = STATE_BREAK;
            break;
        }
        // Неопределенное состояние (такого быть не должно)
        default:
        {
            res = STATE_UNDEFINED;
            break;
        }
    }
    return res;
}


bool IssoMultiSensorData::alarmed()
{
    auto state = this->state();
    return ((state == STATE_WARNING) || (state == STATE_ALARM));
}


int IssoMultiSensorData::alarmChannels()
{
    int result = 0;
    foreach (auto detailState, m_detailsMap)
    {
        if ((detailState == STATE_WARNING) ||
                (detailState == STATE_ALARM))
            result++;
    }
    return result;
}


QMap<IssoDigitalSensorId, IssoState> IssoMultiSensorData::getDigitalSensorStates()
{
    return m_detailsMap;
}


QMap<IssoDigitalSensorId, IssoState> IssoMultiSensorData::makeDigitalSensorStates(quint16 detailsValue)
{
    QMap<IssoDigitalSensorId, IssoState> digitalSensorStates;
    // разобрать сырое значение по 2 бита на каждый цифровой датчик
    // и добавить в коллекцию:
    //
    // состояние датчика Дым (оптич.)
    digitalSensorStates.insert(DIGITALSENSOR_SMOKE_O, detailsToState(detailsValue, 0));
    // состояние датчика Темп. (аналог.)
    digitalSensorStates.insert(DIGITALSENSOR_TEMP_A, detailsToState(detailsValue, 2));
    // состояние датчика Темп. (цифр.)
    digitalSensorStates.insert(DIGITALSENSOR_TEMP_D, detailsToState(detailsValue, 4));
    // состояние датчика Дым (электр.)
    digitalSensorStates.insert(DIGITALSENSOR_SMOKE_E, detailsToState(detailsValue, 6));
    // состояние датчика Пламя
    digitalSensorStates.insert(DIGITALSENSOR_FLAME_STD_DEV, detailsToState(detailsValue, 8));
    // состояние датчика CO
    digitalSensorStates.insert(DIGITALSENSOR_CO, detailsToState(detailsValue, 10));
    // состояние датчика ЛОВ
    digitalSensorStates.insert(DIGITALSENSOR_VOC, detailsToState(detailsValue, 12));
    return digitalSensorStates;
}


quint16 IssoMultiSensorData::makeDetailsValue(const QMap<IssoDigitalSensorId, IssoState> &digitalStates)
{
    quint16 result = 0;
    // состояние датчика Дым (оптич.)
    result = stateToDetails(result, 0, digitalStates.value(DIGITALSENSOR_SMOKE_O));
    // состояние датчика Темп. (аналог.)
    result = stateToDetails(result, 2, digitalStates.value(DIGITALSENSOR_TEMP_A));
    // состояние датчика Темп. (цифр.)
    result = stateToDetails(result, 4, digitalStates.value(DIGITALSENSOR_TEMP_D));
    // состояние датчика Дым (электр.)
    result = stateToDetails(result, 6, digitalStates.value(DIGITALSENSOR_SMOKE_E));
    // состояние датчика Пламя
    result = stateToDetails(result, 8, digitalStates.value(DIGITALSENSOR_FLAME_STD_DEV));
    // состояние датчика CO
    result = stateToDetails(result, 10, digitalStates.value(DIGITALSENSOR_CO));
    // состояние датчика ЛОВ
    result = stateToDetails(result, 12, digitalStates.value(DIGITALSENSOR_VOC));
    // вернуть результат
    return result;
}



// ВРЕМЕННО
IssoState IssoMultiSensorData::valueToState(quint8 value)
{
    if (value == 0)
        return STATE_NORMAL;
    else if (value & 0x02)
        return STATE_ALARM;
    else if (value & 0x01)
        return STATE_WARNING;
    return STATE_FAIL;
}


// ВЕРНУТЬ!!!
//IssoState IssoMultiSensorData::valueToState(quint8 value)
//{
//    IssoState res;
//    switch(value)
//    {
//        case 0:
//        {
//            res = STATE_NORMAL;
//            break;
//        }
//        case 1:
//        {
//            res = STATE_WARNING;
//            break;
//        }
//        case 2:
//        {
//            res = STATE_ALARM;
//            break;
//        }
//        default:
//        {
//            res = STATE_FAIL;
//            break;
//        }
//    }
//    return res;
//}


quint8 IssoMultiSensorData::stateToValue(IssoState state)
{
    quint8 stateValue;
    // преобразовать состояние цифрового датчика в значение
    switch (state)
    {
        case STATE_NORMAL:
        {
            stateValue = 0;
            break;
        }
        case STATE_WARNING:
        {
            stateValue = 1;
            break;
        }
        case STATE_ALARM:
        {
            stateValue = 2;
            break;
        }
        default:
        {
            stateValue = 3;
            break;
        }
    }
    return stateValue;
}



IssoState IssoMultiSensorData::detailsToState(quint16 detailsValue, quint8 shiftBits)
{
    // получить значение состояния датчика согласно смещению
    quint8 value = (quint8)((detailsValue >> shiftBits) & 0x03);
    // преобразовать и вернуть состояние
    return valueToState(value);
}


quint16 IssoMultiSensorData::stateToDetails(quint16 detailsValue, quint8 shiftBits,
                                            IssoState state)
{
    quint16 stateValue = stateToValue(state);
    // сместить до целевой позиции
    stateValue <<= shiftBits;
    // вернуть общее значение, объединенное с состоянием датчика
    return (detailsValue | stateValue);
}

int IssoMultiSensorData::size()
{
    return sizeof(m_multiSensorId)
            + sizeof(m_stateValue) + sizeof(m_detailsValue);
}



QDataStream& operator>>(QDataStream &in, IssoMultiSensorData &data)
{
    // ID мультидатчика (адрес)
    in >> data.m_multiSensorId;
    // общее состояние
    in >> data.m_stateValue;
    // состояния встроенных цифровых датчиков + флаг достоверности (старший бит)
    in >> data.m_detailsValue;
    //
    // сформировать коллекцию состояний каналов
    data.m_detailsMap = IssoMultiSensorData::makeDigitalSensorStates(data.m_detailsValue);
    return in;
}


QDataStream& operator<<(QDataStream &out, const IssoMultiSensorData &data)
{
    // ID мультидатчика (адрес)
    out << data.m_multiSensorId;
    // общее состояние
    out << data.m_stateValue;
    // состояния встроенных цифровых датчиков + флаг достоверности (старший бит)
    out << data.m_detailsValue;
    return out;
}



//========== IssoMultiSensorStateParam ==========

IssoMultiSensorStateParam::IssoMultiSensorStateParam()
    : IssoMultiSensorStateParam(PARAM_UNDEFINED, QMap<quint8, IssoMultiSensorData>())
{
}


IssoMultiSensorStateParam::IssoMultiSensorStateParam(IssoParamId id,
                                                     QMap<quint8, IssoMultiSensorData> dataMap)
    : IssoSensorStateParam(id, STATE_UNDEFINED),
      m_dataMap(dataMap)
{
    // вычислить результирующее состоание
    m_state = calcResultState();
}


IssoMultiSensorStateParam::IssoMultiSensorStateParam(const IssoMultiSensorStateParam &other)
    : IssoSensorStateParam(other),
      m_dataMap(other.dataMap())
{
}


IssoMultiSensorStateParam::~IssoMultiSensorStateParam()
{
}


QMap<quint8, IssoMultiSensorData> IssoMultiSensorStateParam::dataMap() const
{
    return m_dataMap;
}


IssoMultiSensorData IssoMultiSensorStateParam::multiSensorData(quint8 addr)
{
    return m_dataMap.value(addr);
}


bool IssoMultiSensorStateParam::loadFromStream(QDataStream *stream)
{
    // класс наследует IssoSensorStateParam, однако поле m_state
    // не загружается из потока, а рассчитывается, поэтому вызывается loadFromStream
    // класса IssoParam, чтобы избежать загрузки из потока поля m_state
    if (!IssoParam::loadFromStream(stream))
        return false;
    // количество мультидатчиков
    quint8 sensorCount;
    // убедиться, что буфер содержит поле Количество мультидатчиков
    if (stream->device()->bytesAvailable() < sizeof(sensorCount))
        return false;
    // загрузить количество мультидатчиков
    *stream >> sensorCount;
    if (sensorCount < 1)
        return true;

    // загрузить данные для всех датчиков
    for (int i = 0; i < sensorCount; ++i)
    {
        IssoMultiSensorData data;
        // убедиться, что поток содержит достаточно данных
        if (stream->device()->bytesAvailable() < data.size()/*sizeof(data)*/)
            return false;
        // структура данных мультидатчика
        *stream >> data;
        // добавить структуру в коллекцию
        m_dataMap.insert(data.multiSensorId(), data);
    }
    // обновить состояние
    m_state = calcResultState();
    return true;
}


void IssoMultiSensorStateParam::saveToStream(QDataStream *stream)
{
    // класс наследует IssoSensorStateParam, однако поле m_state
    // не сохраняется в поток, т.к. оно является рассчитанным из
    // состояний цифровых датчиков, поэтому вызывается saveToStream
    // класса IssoParam, чтобы избежать записи в поток поля m_state
    IssoParam::saveToStream(stream);
    //
    // количество мультидатчиков
    quint8 sensorCount = m_dataMap.size();
    // записать количество мультидатчиков
    *stream << sensorCount;
    // записать данные для всех датчиков
    foreach (IssoMultiSensorData data, m_dataMap)
    {
        *stream << data;
    }
}


int IssoMultiSensorStateParam::size()
{
    // класс наследует IssoSensorStateParam, однако поле m_state
    // является расситанным, т.е. не фигурирует при передаче данных,
    // поэтому не должно учитываться при расчете размера объекта
    return IssoParam::size() + sizeof(quint8) +
            m_dataMap.size() * sizeof(IssoMultiSensorData);
}


QString IssoMultiSensorStateParam::description()
{
    return QString("IssoMultiSensorStateParam");
}


QMap<quint8, IssoMultiSensorData> IssoMultiSensorStateParam::diffDataMap(
                                    const QMap<quint8, IssoMultiSensorData> &otherDataMap)
{
    // если текущая коллекция пуста, вернуть новую коллекцию
    if (m_dataMap.isEmpty())
        return otherDataMap;
    // если новая коллекция пуста, вернуть текущую коллекцию
    if (otherDataMap.isEmpty())
        return m_dataMap;
    //
    QMap<quint8, IssoMultiSensorData> resultMap;
    foreach (auto otherData, otherDataMap)
    {
        // если данные мультидатчика получены впервые, добавить их
        if (!m_dataMap.contains(otherData.multiSensorId()))
        {
            resultMap.insert(otherData.multiSensorId(), otherData);
        }
        // если данные уже существуют, сравнить их
        else
        {
            // текущие данные мультидатчика
            auto curData = m_dataMap.value(otherData.multiSensorId());
            // сравнить состояния (общие и детализацию)
            if ((curData.stateValue() != otherData.stateValue()) ||
                (curData.detailsValue() != otherData.detailsValue()))
            {
                resultMap.insert(otherData.multiSensorId(), otherData);
            }
        }
    }
    return resultMap;
}


IssoState IssoMultiSensorStateParam::calcResultState()
{
    // найти состояние с вышим приоритетом
    IssoState res = STATE_UNDEFINED;
    foreach (IssoMultiSensorData data, m_dataMap)
    {
        // результат - состояние с высшим приоритетом
        res = IssoCommonData::higherPriorityState(res, data.state());
        // если тревога, то выход
        if (res == STATE_ALARM)
            break;
    }
    return res;
}



//========== IssoMultiSensorRawDataParam ==========

IssoMultiSensorRawDataParam::IssoMultiSensorRawDataParam()
    : IssoMultiSensorRawDataParam(PARAM_UNDEFINED)
{
}


IssoMultiSensorRawDataParam::IssoMultiSensorRawDataParam(IssoParamId id)
    : IssoMultiSensorRawDataParam(id, IssoMultiSensorData(),
                                  QMap<IssoDigitalSensorId, quint16>())
{
}


IssoMultiSensorRawDataParam::IssoMultiSensorRawDataParam(IssoParamId id,
                                                         const IssoMultiSensorData &data,
                                                         const QMap<IssoDigitalSensorId, quint16> &rawValues)
    : IssoSensorStateParam(id, STATE_UNDEFINED),
      m_data(data),
      m_rawValues(rawValues)
{
    m_state = m_data.state();
}


IssoMultiSensorRawDataParam::~IssoMultiSensorRawDataParam()
{
}


bool IssoMultiSensorRawDataParam::loadFromStream(QDataStream *stream)
{
    // класс наследует IssoSensorStateParam, однако поле m_state
    // не загружается из потока, а рассчитывается, поэтому вызывается loadFromStream
    // класса IssoParam, чтобы избежать загрузки из потока поля m_state
    if (!IssoParam::loadFromStream(stream))
        return false;
    //
    // ожидаемый размер данных:
    // - размер структуры данных мультидатчика
    // - размер сырых данных
    qint64 expectedSize = m_data.size()/*sizeof(m_data)*/ + RAW_DATA_SIZE;
    //
    // убедиться, что буфер содержит достаточно данных
    if (stream->device()->bytesAvailable() < expectedSize)
        return false;

    // структура данных мультидатчика
    *stream >> m_data;
    // обновить состояние
    m_state = m_data.state();
    //
    // считать сырое значение для каждого цифрового датчика
    quint16 rawVal;
    //
    // значение датчика Дым (оптич.) LED ON
    *stream >> rawVal;
    m_rawValues.insert(DIGITALSENSOR_SMOKE_O_LED_ON, rawVal);
    // значение датчика Дым (оптич.) LED OFF
    *stream >> rawVal;
    m_rawValues.insert(DIGITALSENSOR_SMOKE_O_LED_OFF, rawVal);
    // интегрированное значение датчика Дым (оптич.) LED ON - LED OFF
    *stream >> rawVal;
    m_rawValues.insert(DIGITALSENSOR_SMOKE_O, rawVal);
    // значение датчика Темп. (аналог.)
    *stream  >> rawVal;
    m_rawValues.insert(DIGITALSENSOR_TEMP_A, rawVal);
    //
    // значение датчика Темп. (цифр.)
    *stream  >> rawVal;
    m_rawValues.insert(DIGITALSENSOR_TEMP_D, rawVal);
    //
    // значение датчика Дым (электр.)
    *stream  >> rawVal;
    m_rawValues.insert(DIGITALSENSOR_SMOKE_E, rawVal);
    //
    // значение датчика Пламя - 1
    *stream  >> rawVal;
    m_rawValues.insert(DIGITALSENSOR_FLAME_1, rawVal);
    //
    // значение датчика Пламя - 2
    *stream  >> rawVal;
    m_rawValues.insert(DIGITALSENSOR_FLAME_2, rawVal);
    //
    // значение датчика Пламя - 3
    *stream  >> rawVal;
    m_rawValues.insert(DIGITALSENSOR_FLAME_3, rawVal);
    //
    // значение датчика Пламя - 4
    *stream  >> rawVal;
    m_rawValues.insert(DIGITALSENSOR_FLAME_4, rawVal);
    //
    // значение датчика Пламя - 5
    *stream  >> rawVal;
    m_rawValues.insert(DIGITALSENSOR_FLAME_5, rawVal);
    //
    // значение датчика Пламя - 6
    *stream  >> rawVal;
    m_rawValues.insert(DIGITALSENSOR_FLAME_6, rawVal);
    //
    // значение датчика Пламя (усредненное значение минимальных показателей Пламя 1 - 6)
    *stream  >> rawVal;
    m_rawValues.insert(DIGITALSENSOR_FLAME_MIN_VAL, rawVal);
    //
    // значение датчика Пламя (среднеквадратичное отклонение)
    *stream  >> rawVal;
    m_rawValues.insert(DIGITALSENSOR_FLAME_STD_DEV, rawVal);
    //
    // значение датчика CO
    *stream >> rawVal;
    m_rawValues.insert(DIGITALSENSOR_CO, rawVal);
    //
    // значение датчика ЛОВ
    *stream >> rawVal;
    m_rawValues.insert(DIGITALSENSOR_VOC, rawVal);
    //
    return true;
}


void IssoMultiSensorRawDataParam::saveToStream(QDataStream *stream)
{
    // класс наследует IssoSensorStateParam, однако поле m_state
    // не сохраняется в поток, т.к. оно является рассчитанным из
    // состояний цифровых датчиков, поэтому вызывается saveToStream
    // класса IssoParam, чтобы избежать записи в поток поля m_state
    IssoParam::saveToStream(stream);
    //
    // структура данных мультидатчика
    *stream << m_data;
    //
    // записать сырое значение для каждого цифрового датчика
    quint16 rawVal;
    //
    // значение датчика Дым (оптич.) LED ON
    rawVal = m_rawValues.value(DIGITALSENSOR_SMOKE_O_LED_ON, 0);
    *stream << rawVal;
    //
    // значение датчика Дым (оптич.) LED OFF
    rawVal = m_rawValues.value(DIGITALSENSOR_SMOKE_O_LED_OFF, 0);
    *stream << rawVal;
    //
    // значение датчика Дым (оптич.)
    rawVal = m_rawValues.value(DIGITALSENSOR_SMOKE_O, 0);
    *stream << rawVal;
    //
    // значение датчика Темп. (аналог.)
    rawVal = m_rawValues.value(DIGITALSENSOR_TEMP_A, 0);
    *stream << rawVal;
    //
    // значение датчика Темп. (цифр.)
    rawVal = m_rawValues.value(DIGITALSENSOR_TEMP_D, 0);
    *stream << rawVal;
    //
    // значение датчика Дым (электр.)
    rawVal = m_rawValues.value(DIGITALSENSOR_SMOKE_E, 0);
    *stream << rawVal;
    //
    // значение датчика Пламя - 1
    rawVal = m_rawValues.value(DIGITALSENSOR_FLAME_1, 0);
    *stream << rawVal;
    //
    // значение датчика Пламя - 2
    rawVal = m_rawValues.value(DIGITALSENSOR_FLAME_2, 0);
    *stream << rawVal;
    //
    // значение датчика Пламя - 3
    rawVal = m_rawValues.value(DIGITALSENSOR_FLAME_3, 0);
    *stream << rawVal;
    //
    // значение датчика Пламя - 4
    rawVal = m_rawValues.value(DIGITALSENSOR_FLAME_4, 0);
    *stream << rawVal;
    //
    // значение датчика Пламя - 5
    rawVal = m_rawValues.value(DIGITALSENSOR_FLAME_5, 0);
    *stream << rawVal;
    //
    // значение датчика Пламя - 6
    rawVal = m_rawValues.value(DIGITALSENSOR_FLAME_6, 0);
    *stream << rawVal;
    //
    // значение датчика Пламя (усредненное значение минимальных показателей Пламя 1 - 6)
    rawVal = m_rawValues.value(DIGITALSENSOR_FLAME_MIN_VAL, 0);
    *stream << rawVal;
    //
    // значение датчика Пламя (среднеквадратичное отклонение)
    rawVal = m_rawValues.value(DIGITALSENSOR_FLAME_STD_DEV, 0);
    *stream << rawVal;
    //
    // значение датчика CO
    rawVal = m_rawValues.value(DIGITALSENSOR_CO, 0);
    *stream << rawVal;
    //
    // значение датчика ЛОВ
    rawVal = m_rawValues.value(DIGITALSENSOR_VOC, 0);
    *stream << rawVal;
}

//bool IssoMultiSensorRawDataParam::loadFromStream(QDataStream *stream)
//{
//    if (!IssoMultiSensorStateParam::loadFromStream(stream))
//        return false;
//    if (stream->device()->bytesAvailable() < RAW_DATA_SIZE)
//        return false;
//    // считать сырое значение для каждого цифрового датчика
//    quint8 rawVal;
//    //
//    // состояние датчика Дым (оптич.)
//    if (stream->readRawData((char*)&rawVal, sizeof(rawVal)) == -1)
//        return false;
//    m_rawValues.insert(DIGITALSENSOR_SMOKE_OPTICAL, ((float)rawVal / 10.0f));
//    // состояние датчика Темп. (аналог.)
//    if (stream->readRawData((char*)&rawVal, sizeof(rawVal)) == -1)
//        return false;
//    m_rawValues.insert(DIGITALSENSOR_TEMP_ANALOG, ((float)rawVal / 10.0f));
//    // состояние датчика Темп. (цифр.)
//    if (stream->readRawData((char*)&rawVal, sizeof(rawVal)) == -1)
//        return false;
//    m_rawValues.insert(DIGITALSENSOR_TEMP_DIGITAL, ((float)rawVal / 10.0f));
//    // состояние датчика Дым (электр.)
//    if (stream->readRawData((char*)&rawVal, sizeof(rawVal)) == -1)
//        return false;
//    m_rawValues.insert(DIGITALSENSOR_SMOKE_ELECTROCHEMICAL, ((float)rawVal / 10.0f));
//    // состояние датчика Пламя
//    if (stream->readRawData((char*)&rawVal, sizeof(rawVal)) == -1)
//        return false;
//    m_rawValues.insert(DIGITALSENSOR_FLAME, ((float)rawVal / 10.0f));
//    // состояние датчика CO
//    if (stream->readRawData((char*)&rawVal, sizeof(rawVal)) == -1)
//        return false;
//    m_rawValues.insert(DIGITALSENSOR_CO, ((float)rawVal / 10.0f));
//    // состояние датчика ЛОВ
//    if (stream->readRawData((char*)&rawVal, sizeof(rawVal)) == -1)
//        return false;
//    m_rawValues.insert(DIGITALSENSOR_FLY_MATTER, ((float)rawVal / 10.0f));
//    //
//    return true;
//}


//void IssoMultiSensorRawDataParam::saveToStream(QDataStream *stream)
//{
//    IssoMultiSensorStateParam::saveToStream(stream);
//    //
//    // записать сырое значение для каждого цифрового датчика
//    quint8 rawVal;
//    //
//    // состояние датчика Дым (оптич.)
//    rawVal = (quint8)(qRound(m_rawValues.value(DIGITALSENSOR_SMOKE_OPTICAL, 0.0f) * 10));
//    stream->writeRawData((char*)&rawVal, sizeof(rawVal));
//    // состояние датчика Темп. (аналог.)
//    rawVal = (quint8)(qRound(m_rawValues.value(DIGITALSENSOR_TEMP_ANALOG, 0.0f) * 10));
//    stream->writeRawData((char*)&rawVal, sizeof(rawVal));
//    // состояние датчика Темп. (цифр.)
//    rawVal = (quint8)(qRound(m_rawValues.value(DIGITALSENSOR_TEMP_DIGITAL, 0.0f) * 10));
//    stream->writeRawData((char*)&rawVal, sizeof(rawVal));
//    // состояние датчика Дым (электр.)
//    rawVal = (quint8)(qRound(m_rawValues.value(DIGITALSENSOR_SMOKE_ELECTROCHEMICAL, 0.0f) * 10));
//    stream->writeRawData((char*)&rawVal, sizeof(rawVal));
//    // состояние датчика Пламя
//    rawVal = (quint8)(qRound(m_rawValues.value(DIGITALSENSOR_FLAME, 0.0f) * 10));
//    stream->writeRawData((char*)&rawVal, sizeof(rawVal));
//    // состояние датчика CO
//    rawVal = (quint8)(qRound(m_rawValues.value(DIGITALSENSOR_CO, 0.0f) * 10));
//    stream->writeRawData((char*)&rawVal, sizeof(rawVal));
//    // состояние датчика ЛОВ
//    rawVal = (quint8)(qRound(m_rawValues.value(DIGITALSENSOR_FLY_MATTER, 0.0f) * 10));
//    stream->writeRawData((char*)&rawVal, sizeof(rawVal));
//}


int IssoMultiSensorRawDataParam::size()
{
    // класс наследует IssoSensorStateParam, однако поле m_state
    // является расситанным, т.е. не фигурирует при передаче данных,
    // поэтому не должно учитываться при расчете размера объекта
    return IssoParam::size() + m_data.size() + RAW_DATA_SIZE;
}


QString IssoMultiSensorRawDataParam::description()
{
    QString statesStr;
    auto sensorStates = m_data.getDigitalSensorStates();
    foreach (IssoDigitalSensorId sensorId, sensorStates.keys())
    {
        statesStr += QString("%1: %2 (%3)\n")
                        .arg(IssoCommonData::stringByDigitalSensorId(sensorId))
                        .arg(IssoCommonData::stringBySensorState(sensorStates.value(sensorId)))
                        .arg(m_rawValues.value(sensorId, 0));
    }
    return QString("IssoMultiSensorRawDataParam:   "
                   "id = %1;   multiSensor id = %2;   state = %3;   details = %4 \n%5")
                    .arg(m_id).arg(m_data.multiSensorId()).arg(m_state).arg(m_data.detailsValue()).arg(statesStr);
}


IssoMultiSensorData IssoMultiSensorRawDataParam::data() const
{
    return m_data;
}


QMap<IssoDigitalSensorId, quint16> IssoMultiSensorRawDataParam::rawValues() const
{
    return m_rawValues;
}


//========== IssoMultiSensorExistDataParam ==========

IssoMultiSensorExistDataParam::IssoMultiSensorExistDataParam()
    : IssoMultiSensorExistDataParam(PARAM_UNDEFINED, 0, 0, 0, 0, 0, 0)
{
}


IssoMultiSensorExistDataParam::IssoMultiSensorExistDataParam(IssoParamId id,
                                                             quint8 address,
                                                             quint8 deviceType,
                                                             quint16 firstSN,
                                                             quint16 secondSN,
                                                             quint16 majorVersion,
                                                             quint16 minorVersion)
    : IssoParam(id),
      m_address(address),
      m_deviceType(deviceType),
      m_firstSN(firstSN),
      m_secondSN(secondSN),
      m_majorVersion(majorVersion),
      m_minorVersion(minorVersion)
{
}


IssoMultiSensorExistDataParam::~IssoMultiSensorExistDataParam()
{
}


bool IssoMultiSensorExistDataParam::loadFromStream(QDataStream *stream)
{
    if (!IssoParam::loadFromStream(stream))
        return false;
    // проверить размер данных
    if (stream->device()->bytesAvailable() <
            sizeof(m_address) + sizeof(m_deviceType) +
            sizeof(m_firstSN) + sizeof(m_secondSN) +
            sizeof(m_majorVersion) + sizeof(m_minorVersion))
        return false;
    // считать данные
    *stream >> m_address;
    *stream >> m_deviceType;
    *stream >> m_firstSN;
    *stream >> m_secondSN;
    *stream >> m_majorVersion;
    *stream >> m_minorVersion;
    return true;
}


void IssoMultiSensorExistDataParam::saveToStream(QDataStream *stream)
{
    IssoParam::saveToStream(stream);
    // записать данные
    *stream << m_address;
    *stream << m_deviceType;
    *stream << m_firstSN;
    *stream << m_secondSN;
    *stream << m_majorVersion;
    *stream << m_minorVersion;
}


int IssoMultiSensorExistDataParam::size()
{
    return IssoParam::size() + sizeof(m_address) + sizeof(m_deviceType) +
                               sizeof(m_firstSN) + sizeof(m_secondSN) +
                               sizeof(m_majorVersion) + sizeof(m_minorVersion);
}


QString IssoMultiSensorExistDataParam::description()
{
    return QString("IssoMultiSensorExistDataParam");
}


quint8 IssoMultiSensorExistDataParam::address() const
{
    return m_address;
}


quint8 IssoMultiSensorExistDataParam::deviceType() const
{
    return m_deviceType;
}


quint16 IssoMultiSensorExistDataParam::firstSN() const
{
    return m_firstSN;
}


quint16 IssoMultiSensorExistDataParam::secondSN() const
{
    return m_secondSN;
}


quint16 IssoMultiSensorExistDataParam::majorVersion() const
{
    return m_majorVersion;
}


quint16 IssoMultiSensorExistDataParam::minorVersion() const
{
    return m_minorVersion;
}
