#include "issodigitalsensorconfig.h"


IssoDigitalSensorConfig::IssoDigitalSensorConfig(IssoDigitalSensorId id) :
    m_id(id),
    m_condition(CONDITION_OR),
    m_enabled(true)
{
    // задать значения состояний по умолчанию
    setDefaultStateValues();
}


IssoDigitalSensorConfig::IssoDigitalSensorConfig(const IssoDigitalSensorConfig &other)
    : m_id(other.id()),
      m_stateValues(other.stateValues()),
      m_condition(other.condition()),
      m_enabled(other.enabled())
{
}


IssoStateCondition IssoDigitalSensorConfig::condition() const
{
    return m_condition;
}


void IssoDigitalSensorConfig::setCondition(const IssoStateCondition &condition)
{
    m_condition = condition;
}


void IssoDigitalSensorConfig::toString(QTextStream &stream)
{
    stream << "    DigitalSensorId = " << IssoCommonData::stringByDigitalSensorId(m_id) << endl;
    stream << "    condition = " << (m_condition == CONDITION_OR ? "OR" : "AND") << endl;
    stream << "    enabled: " << (m_enabled ? "true" : "false") << endl;
    foreach (IssoState state, m_stateValues.keys())
    {
        stream << QString("        State = %1;   Value = %2")
                    .arg(IssoCommonData::stringBySensorState(state))
                    .arg(m_stateValues.value(state)) << endl;
    }
    stream << endl;
}


void IssoDigitalSensorConfig::setDefaultStateValues()
{
//    setStateValue(STATE_NORMAL, DEFAULT_NORMAL_VALUE);
    setStateValue(STATE_WARNING, DEFAULT_WARNING_VALUE);
    setStateValue(STATE_ALARM, DEFAULT_ALARM_VALUE);
}


QMap<IssoState, int> IssoDigitalSensorConfig::stateValues() const
{
    return m_stateValues;
}


void IssoDigitalSensorConfig::setStateValues(const QMap<IssoState, int> &stateValues)
{
    m_stateValues = stateValues;
}


void IssoDigitalSensorConfig::setStateValues(/*int normalValue,*/ int warningValue, int alarmValue)
{
//    setStateValue(STATE_NORMAL, normalValue);
    setStateValue(STATE_WARNING, warningValue);
    setStateValue(STATE_ALARM, alarmValue);
}


IssoDigitalSensorId IssoDigitalSensorConfig::id() const
{
    return m_id;
}


void IssoDigitalSensorConfig::setId(const IssoDigitalSensorId &id)
{
    m_id = id;
}


quint16 IssoDigitalSensorConfig::stateValue(IssoState state)
{
    return m_stateValues.value(state, -1);
}


void IssoDigitalSensorConfig::setStateValue(IssoState state, int value)
{
    m_stateValues.insert(state, value);
}


bool IssoDigitalSensorConfig::enabled() const
{
    return m_enabled;
}


void IssoDigitalSensorConfig::setEnabled(bool enabled)
{
    m_enabled = enabled;
}


//QDataStream &IssoDigitalSensorConfig::operator<<(QDataStream &out)
//{
//    out << m_id;
//    out << m_stateValues;
//    out << m_enabled;
//    return out;
//}


//QDataStream &IssoDigitalSensorConfig::operator>>(QDataStream &in)
//{
//    in >> m_id;
//    in >> m_stateValues;
//    in >> m_enabled;
//    return in;
//}


//QDataStream &operator<<(QDataStream &out, const IssoDigitalSensorConfig& config)
//{
//    out << config;
//    return out;
//}


//QDataStream &operator>>(QDataStream &in, IssoDigitalSensorConfig& config)
//{
//    in >> config;
//    return in;
//}




QDataStream& operator<<(QDataStream &out, const IssoDigitalSensorConfig &config)
{
    out << config.m_id;
    out << config.m_stateValues;
    out << config.m_condition;
    out << config.m_enabled;
    return out;
}


QDataStream& operator>>(QDataStream &in, IssoDigitalSensorConfig &config)
{
    in >> config.m_id;
    in >> config.m_stateValues;
    in >> config.m_condition;
    in >> config.m_enabled;
    return in;
}
