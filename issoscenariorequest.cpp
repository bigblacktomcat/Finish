#include "issoscenariorequest.h"


IssoScenarioRequest::IssoScenarioRequest()
{
}


IssoScenarioRequest::IssoScenarioRequest(IssoScenarioInfo info,
                                         const IssoModuleRequest &request)
    : m_info(info),
      m_request(request),
      m_terminal(false)
{
}

IssoScenarioRequest::IssoScenarioRequest(IssoScenarioInfo info, bool terminal)
    : m_info(info),
      m_terminal(terminal)
{
}


IssoScenarioRequest::IssoScenarioRequest(const IssoScenarioRequest &other)
    : m_info(other.m_info),
      m_request(other.m_request),
      m_terminal(other.m_terminal)
{
}


IssoScenarioRequest::~IssoScenarioRequest()
{
    qDebug() << "~IssoScenarioRequest";
}


IssoModuleRequest IssoScenarioRequest::request() const
{
    return m_request;
}


QString IssoScenarioRequest::scenarioComplexName() const
{
    return m_info.complexName();
}


IssoScenarioInfo IssoScenarioRequest::info() const
{
    return m_info;
}


bool IssoScenarioRequest::isTerminal() const
{
    return m_terminal;
}


void IssoScenarioRequest::setTerminal(bool terminal)
{
    m_terminal = terminal;
}


bool IssoScenarioRequest::isValid()
{
    return m_info.isValid();
}


bool IssoScenarioRequest::operator ==(const IssoScenarioRequest &other)
{
    return (this->scenarioComplexName() == other.scenarioComplexName()) &&
            (this->m_request.number() == other.m_request.number());
}


QString IssoScenarioRequest::toString()
{
    return QString("Запрос-[%1]_Пакет-[%2]")
            .arg(m_info.complexName())
            .arg(m_request.number());
}

