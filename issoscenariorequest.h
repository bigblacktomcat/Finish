#ifndef ISSOSCENARIOREQUEST_H
#define ISSOSCENARIOREQUEST_H

#include "issomodulerequest.h"
#include "issoscenario.h"


class IssoScenarioRequest
{
    private:
        IssoScenarioInfo m_info;
        IssoModuleRequest m_request;
        bool m_terminal = false;

    public:

        IssoScenarioRequest();
        IssoScenarioRequest(IssoScenarioInfo info,
                            const IssoModuleRequest& request);
        IssoScenarioRequest(IssoScenarioInfo info,
                            bool terminal);
        IssoScenarioRequest(const IssoScenarioRequest& other);
        ~IssoScenarioRequest();

        IssoModuleRequest request() const;
        QString scenarioComplexName() const;
        IssoScenarioInfo info() const;

        bool isTerminal() const;
        void setTerminal(bool terminal);

        bool isValid();

        bool operator == (const IssoScenarioRequest& other);

        QString toString();
};


#endif // ISSOSCENARIOREQUEST_H
