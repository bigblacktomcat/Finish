#include "issochainconfig.h"


//========== IssoChainConfig ==========

IssoChainConfig::IssoChainConfig(IssoChainType type, const QString &name, bool enabled)
    : m_type(type),
      m_name(name),
      m_enabled(enabled)
{
}


IssoChainConfig::~IssoChainConfig()
{
//    qDebug() << "~IssoChainConfig" << m_name << endl;
}


void IssoChainConfig::saveToStream(QDataStream &stream)
{
    stream << m_type;
    stream << m_name;
    stream << m_enabled;
}


bool IssoChainConfig::loadFromStream(QDataStream &stream)
{
    if (stream.device()->bytesAvailable() < sizeof(m_type) + sizeof(m_name) + sizeof(m_enabled))
        return false;
    stream >> m_type;
    stream >> m_name;
    stream >> m_enabled;
    return true;
}


IssoChainType IssoChainConfig::type() const
{
    return m_type;
}


void IssoChainConfig::setType(const IssoChainType &type)
{
    m_type = type;
}


void IssoChainConfig::toString(QTextStream &stream)
{
    stream << "Chain name: " << m_name << endl;
    stream << "Chain type: " << (m_type == SMOKE_CHAIN ? "Analog" : "Digital") << endl;
    stream << "Chain enabled: " << (m_enabled ? "true" : "false") << endl;
}


QDataStream& operator<<(QDataStream &out, IssoChainConfig* config)
{
    config->saveToStream(out);
    return out;
}


QDataStream& operator>>(QDataStream &in, IssoChainConfig* config)
{
    config->loadFromStream(in);
    return in;
}


QString IssoChainConfig::name() const
{
    return m_name;
}


void IssoChainConfig::setName(const QString &name)
{
    m_name = name;
}


bool IssoChainConfig::enabled() const
{
    return m_enabled;
}


void IssoChainConfig::setEnabled(bool enabled)
{
    m_enabled = enabled;
}


//========== IssoAnalogChainConfig ==========

IssoAnalogChainConfig::IssoAnalogChainConfig(IssoChainType type, const QString &name, bool enabled, int sensorCount)
    : IssoChainConfig(type, name, enabled),
      m_sensorCount(sensorCount)
{
}


IssoAnalogChainConfig::~IssoAnalogChainConfig()
{
//    qDebug() << "~IssoAnalogChainConfig = " << m_name << endl;
}


void IssoAnalogChainConfig::saveToStream(QDataStream &stream)
{
    IssoChainConfig::saveToStream(stream);
    stream << m_sensorCount;
    stream << m_stateRanges;
}


bool IssoAnalogChainConfig::loadFromStream(QDataStream &stream)
{

    if (!IssoChainConfig::loadFromStream(stream))
        return false;

    stream >> m_sensorCount;
    stream >> m_stateRanges;
    return true;
}


int IssoAnalogChainConfig::sensorCount() const
{
    return m_sensorCount;
}


void IssoAnalogChainConfig::setSensorCount(int sensorCount)
{
    m_sensorCount = sensorCount;
}


QMap<IssoState, IssoStateRange> IssoAnalogChainConfig::stateRanges() const
{
    return m_stateRanges;
}


void IssoAnalogChainConfig::setStateRanges(const QMap<IssoState, IssoStateRange> &stateRanges)
{
    m_stateRanges = stateRanges;
}


void IssoAnalogChainConfig::insertStateRange(IssoState state, const IssoStateRange &range)
{
    m_stateRanges.insert(state, range);
}


void IssoAnalogChainConfig::removeStateRange(IssoState state)
{
    m_stateRanges.remove(state);
}


IssoStateRange IssoAnalogChainConfig::stateRange(IssoState state)
{
    return m_stateRanges.value(state);
}


void IssoAnalogChainConfig::toString(QTextStream &stream)
{
    IssoChainConfig::toString(stream);
    // вывести пары значений:
    // состояние - диапазон значений
    foreach (IssoState state, m_stateRanges.keys())
    {
        // получить диапазон
        IssoStateRange range = stateRange(state);
        stream << QString("    State = %1;   Range = [%2, %3]")
                    .arg(IssoCommonData::stringBySensorState(state))
                    .arg(range.min())
                    .arg(range.max()) << endl;
    }
    stream << endl;
}


void IssoAnalogChainConfig::initStates(const QSet<IssoState> states)
{
    m_stateRanges.clear();
    foreach (IssoState state, states)
        m_stateRanges.insert(state, IssoStateRange(0, 0));
}


//========== IssoSmokeChainConfig ==========

IssoSmokeChainConfig::IssoSmokeChainConfig(const QString &name, bool enabled, int sensorCount)
    : IssoAnalogChainConfig(SMOKE_CHAIN, name, enabled, sensorCount)
{
}


IssoSmokeChainConfig::~IssoSmokeChainConfig()
{
//    qDebug() << "~IssoSmokeChainConfig = " << m_name << endl;
}


//========== IssoTempChainConfig ==========

IssoTempChainConfig::IssoTempChainConfig(const QString &name, bool enabled, int sensorCount)
    : IssoAnalogChainConfig(TEMP_CHAIN, name, enabled, sensorCount)
{
}


IssoTempChainConfig::~IssoTempChainConfig()
{
//    qDebug() << "~IssoTempChainConfig = " << m_name << endl;
}


//========== IssoAlarmButtonConfig ==========

IssoAlarmButtonConfig::IssoAlarmButtonConfig(const QString &name, bool enabled, int sensorCount)
    : IssoAnalogChainConfig(ALARM_BUTTON_CHAIN, name, enabled, sensorCount)
{
}


IssoAlarmButtonConfig::~IssoAlarmButtonConfig()
{
//    qDebug() << "~IssoAlarmButtonConfig = " << m_name << endl;
}


//========== IssoMoveDetectorConfig ==========

IssoMoveDetectorConfig::IssoMoveDetectorConfig(const QString &name, bool enabled, int sensorCount)
    : IssoAnalogChainConfig(MOVE_DETECTOR_CHAIN, name, enabled, sensorCount)
{
}


IssoMoveDetectorConfig::~IssoMoveDetectorConfig()
{
//    qDebug() << "~IssoMoveDetectorConfig = " << m_name << endl;
}


//========== IssoDigitalChainConfig ==========

IssoAddressChainConfig::IssoAddressChainConfig(const QString &name, bool enabled, int sensorCount)
    : IssoChainConfig(MULTICRITERIAL, name, enabled)
{
    addNewMultiSensorConfigs(sensorCount);
}


IssoAddressChainConfig::~IssoAddressChainConfig()
{
    removeAllMultiSensorConfigs();
}


void IssoAddressChainConfig::saveToStream(QDataStream &stream)
{
    IssoChainConfig::saveToStream(stream);
    //
    // карта конфигураций мультидатчиков
    //
    // количество конфигураций
    stream << m_multiSensorConfigs.size();
    // каждая конфигурация из карты
    foreach (IssoMultiSensorConfig* cfg, m_multiSensorConfigs.values())
    {
        // значение - конфигурация датчика
        stream << *cfg;
    }
}


bool IssoAddressChainConfig::loadFromStream(QDataStream &stream)
{
    if (!IssoChainConfig::loadFromStream(stream))
        return false;
    //
    // карта конфигураций мультидатчиков
    //
    // количество конфигураций
    int cfgCount;
    stream >> cfgCount;
    // каждая конфигурация из карты
    while(cfgCount > 0)
    {
        // значение - конфигурация датчика
        IssoMultiSensorConfig* cfg = new IssoMultiSensorConfig();
        stream >> *cfg;
        // добавить в карту
        insertMultiSensorConfig(cfg);
        // уменьшить счетчик
        cfgCount--;
    }
    return true;
}


QMap<QString, IssoMultiSensorConfig *> IssoAddressChainConfig::multiSensorConfigs() const
{
    return m_multiSensorConfigs;
}


void IssoAddressChainConfig::setMultiSensorConfigs(const QMap<QString, IssoMultiSensorConfig *> &configs)
{
    m_multiSensorConfigs = configs;
}


QStringList IssoAddressChainConfig::multiSensorConfigNames() const
{
    auto list = m_multiSensorConfigs.values();
    // сортировать список мультидатчиков по ID
    std::sort(list.begin(), list.end(),
              [](const IssoMultiSensorConfig* a, const IssoMultiSensorConfig* b) ->
                                        bool { return a->id() < b->id(); });
    // сформировать список имен конфигов мультидатчиков
    QStringList cfgNames;
    foreach (IssoMultiSensorConfig* cfg, list)
    {
        cfgNames << cfg->displayName();
    }
    return cfgNames;
}


int IssoAddressChainConfig::multiSensorConfigCount() const
{
    return m_multiSensorConfigs.size();
}


void IssoAddressChainConfig::addNewMultiSensorConfigs(int configCount)
{
    // в цикле добавить каждый конфиг мультидатчика в конфиг адресного шлейфа
    for (int i = 0; i < configCount; ++i)
    {
        // получить следующий ID
        int id = nextMultiSensorId();
        if (id == -1)
            break;
        // создать конфиг мультидатчика
        IssoMultiSensorConfig* multiSensorCfg = new IssoMultiSensorConfig(id);
        // добавить конфиг мультидатчика в конфиг шлейфа
        insertMultiSensorConfig(multiSensorCfg);
    }
}


void IssoAddressChainConfig::insertMultiSensorConfig(IssoMultiSensorConfig *cfg)
{
    m_multiSensorConfigs.insert(cfg->displayName(), cfg);
}


void IssoAddressChainConfig::removeMultiSensorConfig(const QString &cfgName)
{
    // получить конфиг мультидатчика
    IssoMultiSensorConfig* cfg = m_multiSensorConfigs.value(cfgName, nullptr);
    // удалить из коллекции
    m_multiSensorConfigs.remove(cfgName);
    // если не пуст, уничтожить
    if (cfg)
        delete cfg;
}


void IssoAddressChainConfig::removeAllMultiSensorConfigs()
{
    foreach (QString cfgName, m_multiSensorConfigs.keys())
    {
        removeMultiSensorConfig(cfgName);
    }
}


IssoMultiSensorConfig *IssoAddressChainConfig::multiSensorConfig(const QString &cfgName)
{
    return m_multiSensorConfigs.value(cfgName, nullptr);
}


IssoMultiSensorConfig *IssoAddressChainConfig::multiSensorConfig(int cfgId)
{
    return m_multiSensorConfigs.value(IssoCommonData::makeMultiSensorName(cfgId), nullptr);
}


int IssoAddressChainConfig::nextMultiSensorId() const
{
    for (int i = 1; i <= MAX_MULTISENSOR_ID; ++i)
    {
        // сформировать имя мультидатчика по ID
        QString name = IssoCommonData::makeMultiSensorName(i);
        // если мультидатчик с таким именем не найден, вернуть ID
        if (!m_multiSensorConfigs.contains(name))
            return i;
    }
    return -1;
}

QByteArray IssoAddressChainConfig::multiSensorIdArray() const
{
    QByteArray ba;
    foreach (IssoMultiSensorConfig* cfg, m_multiSensorConfigs)
    {
        ba.append((char)cfg->id());
    }
    return ba;
}


QList<quint8> IssoAddressChainConfig::multiSensorIdList() const
{
    QList<quint8> list;
    foreach (IssoMultiSensorConfig* cfg, m_multiSensorConfigs)
    {
        list << cfg->id();
    }
    std::sort(list.begin(), list.end());
    return list;
}


void IssoAddressChainConfig::toString(QTextStream &stream)
{
    IssoChainConfig::toString(stream);
    // вывести пары значений:
    foreach (IssoMultiSensorConfig* cfg, m_multiSensorConfigs.values())
    {
        cfg->toString(stream);
    }
    stream << endl;
}


//========== IssoChainConfigFactory ==========

IssoChainConfig *IssoChainConfigFactory::make(IssoChainType type,
                                              const QString &name,
                                              int sensorCount,
                                              bool enabled)
{
    IssoChainConfig* chainCfg = nullptr;
    switch(type)
    {
        case SMOKE_CHAIN:
        {
            chainCfg = new IssoSmokeChainConfig(name, enabled, sensorCount);
            break;
        }
        case TEMP_CHAIN:
        {
            chainCfg = new IssoTempChainConfig(name, enabled, sensorCount);
            break;
        }
        case ALARM_BUTTON_CHAIN:
        {
            chainCfg = new IssoAlarmButtonConfig(name, enabled, sensorCount);
            break;
        }
        case MOVE_DETECTOR_CHAIN:
        {
            chainCfg = new IssoMoveDetectorConfig(name, enabled, sensorCount);
            break;
        }
        case MULTICRITERIAL:
        {
            chainCfg = new IssoAddressChainConfig(name, enabled, sensorCount);
            break;
        }
        default:
            break;
    }
    return chainCfg;
}


IssoChainConfig *IssoChainConfigFactory::make(IssoChainType type)
{
    return IssoChainConfigFactory::make(type, "", true, 1);
}

