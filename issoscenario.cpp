#include "issoscenario.h"


IssoScenario::IssoScenario(const QString& name, IssoScenarioScope scope)
    : m_name(name),
      m_scope(scope)
{
}


void IssoScenario::loadFromStream(QDataStream &stream)
{
    stream >> m_name;
    stream >> m_scope;
    stream >> m_relayClassNames;
}


void IssoScenario::saveToStream(QDataStream &stream)
{
    stream << m_name;
    stream << m_scope;
    stream << m_relayClassNames;
}


QString IssoScenario::name() const
{
    return m_name;
}


void IssoScenario::setName(const QString &name)
{
    m_name = name;
}


IssoScenarioScope IssoScenario::scope() const
{
    return m_scope;
}


void IssoScenario::setScope(const IssoScenarioScope &scope)
{
    m_scope = scope;
}


QStringList IssoScenario::relayClassNames() const
{
    return m_relayClassNames;
}


void IssoScenario::setRelayClassNames(const QStringList &relayClassNames)
{
    m_relayClassNames = relayClassNames;
}


void IssoScenario::addRelayClassName(const QString &className)
{
    m_relayClassNames.append(className);
}


void IssoScenario::removeRelayClassName(const QString &className)
{
    m_relayClassNames.removeAll(className);
}


void IssoScenario::replaceRelayClassName(const QString &oldName, const QString &newName)
{
    // если имя не изменилось, выход
    if (oldName == newName)
        return;

    int index = m_relayClassNames.indexOf(oldName);
    if (index != -1)
        m_relayClassNames.replace(index, newName);
}


bool IssoScenario::containsRelayClassName(const QString &className)
{
    return m_relayClassNames.contains(className);
}


void IssoScenario::clearRelayClassNames()
{
    m_relayClassNames.clear();
}


bool IssoScenario::isValid() const
{
    return !m_name.isEmpty();
}

