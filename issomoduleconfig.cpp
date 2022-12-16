#include "issomoduleconfig.h"


IssoModuleConfig::IssoModuleConfig(QString name)
    : m_name(name)
{
}


IssoModuleConfig::~IssoModuleConfig()
{
    qDebug() << "~IssoModuleConfig" << m_name << endl;
    removeAllSensorConfigs();
}


QMap<IssoParamId, IssoAnalogChainConfig*> IssoModuleConfig::sensorConfigs() const
{
    return m_sensorConfigs;
}


void IssoModuleConfig::setSensorConfigs(const QMap<IssoParamId, IssoAnalogChainConfig *> &sensorConfigs)
{
    m_sensorConfigs = sensorConfigs;
}


void IssoModuleConfig::insertSensorConfig(IssoParamId id, IssoAnalogChainConfig *sensorConfig)
{
    m_sensorConfigs.insert(id, sensorConfig);
}


void IssoModuleConfig::removeSensorConfig(IssoParamId id)
{
    // получить конфигурацию встроенного датчика
    IssoAnalogChainConfig* cfg = sensorConfig(id);
    // удалить из коллекции
    m_sensorConfigs.remove(id);
    // если конфигурация не пуста, уничтожить
    if (cfg)
        delete cfg;
}


void IssoModuleConfig::removeAllSensorConfigs()
{
    foreach(IssoParamId id, m_sensorConfigs.keys())
    {
        removeSensorConfig(id);
    }
}


IssoAnalogChainConfig *IssoModuleConfig::sensorConfig(IssoParamId id) const
{
    return m_sensorConfigs.value(id, nullptr);
}


bool IssoModuleConfig::sensorEnabled(IssoParamId id) const
{
    IssoAnalogChainConfig* cfg = sensorConfig(id);
    return (cfg ? cfg->enabled() : false);
}


void IssoModuleConfig::saveToStream(QDataStream &stream)
{
    // имя конфигурации модуля
    stream << m_name;
    //
    // карта конфигураций встроенных датчиков
    //
    // количество конфигураций
    stream << m_sensorConfigs.size();
    // каждая конфигурация из карты
    foreach (IssoParamId id, m_sensorConfigs.keys())
    {
        // ключ - id датчика
        stream << id;
        // значение - конфигурация датчика
        stream << sensorConfig(id);
    }
    //
    // карта имен конфигураций шлейфов
    //
    stream << m_chainConfigLinks;
    //
    // карта имен классов реле
    //
    stream << m_relayClassLinks;
    //
    // карта имен сценариев
    //
    stream << m_scenarioLinks;
    //
    // имена классов на кноках панели управления
    //
    stream << m_buttonClassLinks;
}


void IssoModuleConfig::loadFromStream(QDataStream &stream)
{
    // имя конфигурации модуля
    stream >> m_name;
    //
    // карта конфигураций встроенных датчиков
    //
    // количество конфигураций
    int sensorCfgCount;
    stream >> sensorCfgCount;
    // каждая конфигурация из карты
    while(sensorCfgCount > 0)
    {
        // ключ - id датчика
        IssoParamId id;
        stream >> id;
        // значение - конфигурация датчика
        IssoAnalogChainConfig* sensorCfg = new IssoAnalogChainConfig();
        stream >> sensorCfg;
        // добавить в карту
        insertSensorConfig(id, sensorCfg);
        // уменьшить счетчик
        sensorCfgCount--;
    }
    //
    // карта имен конфигураций шлейфов
    //
    stream >> m_chainConfigLinks;
    //
    // карта имен классов реле
    //
    stream >> m_relayClassLinks;
    //
    // карта имен сценариев
    //
    stream >> m_scenarioLinks;
    //
    // имена классов на кноках панели управления
    //
    stream >> m_buttonClassLinks;
}


QString IssoModuleConfig::name() const
{
    return m_name;
}


void IssoModuleConfig::setName(const QString &name)
{
    m_name = name;
}


QMap<IssoParamId, IssoChainConfigLink> IssoModuleConfig::chainConfigLinks() const
{
    return m_chainConfigLinks;
}


void IssoModuleConfig::setChainConfigLinks(const QMap<IssoParamId, IssoChainConfigLink> &chainConfigNames)
{
    m_chainConfigLinks = chainConfigNames;
}


void IssoModuleConfig::insertChainConfigLink(IssoParamId id,
                                             const QString &chainConfigName,
                                             bool enabled)
{
    if (chainConfigName.isEmpty())
        removeChainConfigLink(id);
    else
        m_chainConfigLinks.insert(id, IssoChainConfigLink(id, chainConfigName, enabled));
}


void IssoModuleConfig::removeChainConfigLink(IssoParamId id)
{
    m_chainConfigLinks.remove(id);
}


IssoChainConfigLink IssoModuleConfig::chainConfigLink(IssoParamId id) const
{
    return m_chainConfigLinks.value(id, IssoChainConfigLink());
}


QString IssoModuleConfig::chainConfigName(IssoParamId id) const
{
    IssoChainConfigLink link = chainConfigLink(id);
    return link.chainCfgName();
}


bool IssoModuleConfig::chainEnabled(IssoParamId id) const
{
    IssoChainConfigLink link = chainConfigLink(id);
    return link.enabled();
}


bool IssoModuleConfig::containsChainConfigLink(const QString &chainConfigName)
{
    foreach (IssoChainConfigLink link, m_chainConfigLinks.values())
    {
        // если имя конфига шлейфа совпадает с целевым, вернуть true
        if (link.chainCfgName() == chainConfigName)
            return true;
    }
    return false;
}


void IssoModuleConfig::removeChainConfigLinksByName(const QString &chainConfigName)
{
    foreach (IssoChainConfigLink link, m_chainConfigLinks.values())
    {
        // если имя конфига шлейфа совпадает с целевым, удалить ссылку
        if (link.chainCfgName() == chainConfigName)
            removeChainConfigLink(link.paramId());
    }
}


bool IssoModuleConfig::paramEnabled(IssoParamId id) const
{
    bool result = false;
    // обработать в зависимости от id параметра
    switch (id)
    {
        // состояние встроенного датчика
        case BTN_FIRE:
        case BTN_SECURITY:
        case SENSOR_SMOKE:
        case SENSOR_CO:
        case SUPPLY_EXTERNAL:
        case SUPPLY_INTERNAL:
        case VIBRATION:
        case SENSOR_OPEN:
        // состояние датчика со значением
        case VOLT_BATTERY:
        case VOLT_BUS:
        case VOLT_SENSORS:
        case TEMP_OUT:
        {
            result = sensorEnabled(id);
            break;
        }
        // состояние шлейфа
        case ANALOG_CHAIN_1:
        case ANALOG_CHAIN_2:
        case ANALOG_CHAIN_3:
        case ANALOG_CHAIN_4:
        case ANALOG_CHAIN_5:
        case ANALOG_CHAIN_6:
        case MULTISENSOR:
        {
            result = chainEnabled(id);
            break;
        }
        default:
            break;
    }
    return result;
}


IssoChainConfigLink IssoModuleConfig::chainConfigLink(const QString &chainConfigName) const
{
    IssoChainConfigLink result;
    foreach (IssoChainConfigLink link, m_chainConfigLinks.values())
    {
        // если имя конфига шлейфа совпадает с целевым, вернуть ссылку
        if (link.chainCfgName() == chainConfigName)
        {
            result = link;
            break;
        }
    }
    return result;
}


QMap<IssoModuleState, QString> IssoModuleConfig::scenarioLinks() const
{
    return m_scenarioLinks;
}


void IssoModuleConfig::setScenarioLinks(const QMap<IssoModuleState, QString> &scenarioLinks)
{
    m_scenarioLinks = scenarioLinks;
}


void IssoModuleConfig::insertScenarioLink(IssoModuleState moduleState, const QString &scenarioName)
{
    m_scenarioLinks.insert(moduleState, scenarioName);
}


void IssoModuleConfig::removeScenarioLink(IssoModuleState moduleState)
{
    m_scenarioLinks.remove(moduleState);
}


void IssoModuleConfig::removeScenarioName(const QString &scenarioName)
{
    // пока имя сценария присутствует в коллекции
    while(m_scenarioLinks.values().contains(scenarioName))
    {
        // получить ключ по знчению
        IssoModuleState state = m_scenarioLinks.key(scenarioName);
        // удалить из коллекции
        m_scenarioLinks.remove(state);
    }
}


void IssoModuleConfig::updateScenarioName(const QString &oldName, const QString &newName)
{
    // пока имя сценария присутствует в коллекции
    while(m_scenarioLinks.values().contains(oldName))
    {
        // получить ключ по знчению
        IssoModuleState state = m_scenarioLinks.key(oldName);
        // удалить старое имя из коллекции
        m_scenarioLinks.remove(state);
        // добавить новое имя в коллекцию
        m_scenarioLinks.insert(state, newName);
    }
}


QMap<IssoParamId, QString> IssoModuleConfig::relayClassLinks() const
{
    return m_relayClassLinks;
}


void IssoModuleConfig::setRelayClassLinks(const QMap<IssoParamId, QString> &relayClassLinks)
{
    m_relayClassLinks = relayClassLinks;
}


void IssoModuleConfig::insertRelayClassLink(IssoParamId paramId, const QString &relayClassName)
{
    m_relayClassLinks.insert(paramId, relayClassName);
}


void IssoModuleConfig::removeRelayClassLink(IssoParamId paramId)
{
    m_relayClassLinks.remove(paramId);
}


QString IssoModuleConfig::relayClassName(IssoParamId paramId) const
{
    return m_relayClassLinks.value(paramId, "");
}


void IssoModuleConfig::removeRelayClassName(const QString &relayClassName)
{
    // удалить все пары [id реле : имя класса] с заданным именем класса
    while(m_relayClassLinks.values().contains(relayClassName))
    {
        // получить ключ по знчению
        IssoParamId paramId = m_relayClassLinks.key(relayClassName);
        // удалить из коллекции
        m_relayClassLinks.remove(paramId);
    }
    //
    // удалить все пары [номер кнопки : имя класса] с заданным именем класса
    while(m_buttonClassLinks.values().contains(relayClassName))
    {
        // получить ключ по знчению
        int btnNum = m_buttonClassLinks.key(relayClassName);
        // удалить из коллекции
        m_buttonClassLinks.remove(btnNum);
    }
}


void IssoModuleConfig::updateRelayClassName(const QString &oldName, const QString &newName)
{
    // если имя не изменилось, выход
    if (oldName == newName)
        return;

    // обновить все пары [id реле : имя класса] с заданным именем класса
    while(m_relayClassLinks.values().contains(oldName))
    {
        // получить ключ по знчению
        IssoParamId paramId = m_relayClassLinks.key(oldName);
        // удалить старое имя из коллекции
        m_relayClassLinks.remove(paramId);
        // добавить новое имя в коллекцию
        m_relayClassLinks.insert(paramId, newName);
    }
    //
    // обновить все пары [номер кнопки : имя класса] с заданным именем класса
    while(m_buttonClassLinks.values().contains(oldName))
    {
        // получить ключ по знчению
        int btnNum = m_buttonClassLinks.key(oldName);
        // удалить старое имя из коллекции
        m_buttonClassLinks.remove(btnNum);
        // добавить новое имя в коллекцию
        m_buttonClassLinks.insert(btnNum, newName);
    }
}


QList<IssoParamId> IssoModuleConfig::getRelayIdsByClassName(const QString &className)
{
    QList<IssoParamId> relayIds;
    // получить список Id реле, связанных с именем класса
    foreach (IssoParamId relayId, m_relayClassLinks.keys())
    {
        // если реле связано с целевым классом, добавить Id реле в список
        if (relayClassName(relayId) == className)
            relayIds << relayId;
    }
    // вернуть список реле
    return relayIds;
}


QList<IssoParamId> IssoModuleConfig::getRelayIdsByButtonNumber(int buttonNum)
{
    // получить имя класса, связанного с кнопкой
    QString className = buttonClassLink(buttonNum);
    // получить список Id реле, связанных с именем класса
    return getRelayIdsByClassName(className);
}


QList<int> IssoModuleConfig::getButtonNumbersByRelayId(IssoParamId relayId)
{
    QList<int> buttonNums;
    // определить имя класса, связанного с реле
    QString className = relayClassName(relayId);
    // получить список номеров кнопок, связанных с именем класса
    foreach (int buttonNum, m_buttonClassLinks.keys())
    {
        // если кнопка связана с целевым классом, добавить номер кнопки в список
        if (buttonClassLink(buttonNum) == className)
            buttonNums << buttonNum;
    }
    // вернуть список номеров кнопок
    return buttonNums;
}


QMap<int, QString> IssoModuleConfig::buttonClassLinks() const
{
    return m_buttonClassLinks;
}


void IssoModuleConfig::setButtonClassLinks(const QMap<int, QString> &buttonClassLinks)
{
    m_buttonClassLinks = buttonClassLinks;
}


void IssoModuleConfig::insertButtonClassLink(int buttonNum, const QString &buttonClassName)
{
    m_buttonClassLinks.insert(buttonNum, buttonClassName);
}


void IssoModuleConfig::removeButtonClassLink(int buttonNum)
{
    m_buttonClassLinks.remove(buttonNum);
}


QString IssoModuleConfig::buttonClassLink(int buttonNum)
{
    return m_buttonClassLinks.value(buttonNum, "");
}


QString IssoModuleConfig::scenarioNameByModuleState(IssoModuleState moduleState) const
{
    return m_scenarioLinks.value(moduleState, "");
}


void IssoModuleConfig::toString(QTextStream &stream)
{
    stream << "********************" << endl
           << QString("Module config name: %1").arg(m_name) << endl;
    stream << "--------------------" << endl
           << "Built-In sensors:" << endl
           << "--------------------" << endl;
    // вывести встроенные датчики
    foreach (IssoParamId id, sensorConfigs().keys())
    {
        // id встроенного датчика
        stream << QString("Sensor ID = %1")
                  .arg(IssoCommonData::stringByParamId(id)) << endl;
        // конфигурация встроенного датчика
        IssoAnalogChainConfig* sensorCfg = sensorConfig(id);
        sensorCfg->toString(stream);

        stream << "--------------------" << endl;

//        // вывести диапазоны значений состояний
//        foreach (IssoState state, sensorCfg->stateRanges().keys())
//        {
//            // получить диапазон
//            IssoStateRange range = sensorCfg->stateRange(state);
//            stream << QString("State = %1;   Range = [%2, %3]")
//                        .arg(state)
//                        .arg(range.min())
//                        .arg(range.max()) << endl;
//            stream << "----------" << endl;
//        }
    }
}


QDataStream& operator<<(QDataStream &out, IssoModuleConfig* config)
{
    config->saveToStream(out);
    return out;
}


QDataStream& operator>>(QDataStream &in, IssoModuleConfig* config)
{
    config->loadFromStream(in);
    return in;
}


IssoChainConfigLink::IssoChainConfigLink(IssoParamId id, const QString &chainCfgName, bool enabled)
    : m_paramId(id),
      m_chainCfgName(chainCfgName),
      m_enabled(enabled)
{
}


IssoParamId IssoChainConfigLink::paramId() const
{
    return m_paramId;
}


void IssoChainConfigLink::setId(const IssoParamId &id)
{
    m_paramId = id;
}


QString IssoChainConfigLink::chainCfgName() const
{
    return m_chainCfgName;
}


void IssoChainConfigLink::setChainCfgName(const QString &chainCfgName)
{
    m_chainCfgName = chainCfgName;
}


bool IssoChainConfigLink::enabled() const
{
    return m_enabled;
}


bool IssoChainConfigLink::isValid() const
{
    return ((m_paramId != PARAM_UNDEFINED) && (!m_chainCfgName.isEmpty()));
}


QDataStream& operator<<(QDataStream &out, const IssoChainConfigLink &link)
{
    out << link.m_paramId;
    out << link.m_chainCfgName;
    out << link.m_enabled;
    return out;
}


QDataStream& operator>>(QDataStream &in, IssoChainConfigLink &link)
{
    in >> link.m_paramId;
    in >> link.m_chainCfgName;
    in >> link.m_enabled;
    return in;
}
