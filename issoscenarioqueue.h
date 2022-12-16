#ifndef ISSOSCENARIOQUEUE_H
#define ISSOSCENARIOQUEUE_H

#include "issomodulerequest.h"
#include "issoscenariorequest.h"
#include <QMutex>
#include <QQueue>
#include <QString>


class IssoScenarioQueue
{
    private:
        QMutex m_lock;
        QQueue<IssoScenarioRequest> m_queue;

    public:
        IssoScenarioQueue();

        void append(const IssoScenarioRequest& scenarioRequest);
        void append(const QList<IssoScenarioRequest>& scenarioRequests);
        IssoScenarioRequest take();
        void removeAll(const QString& scenarioName);

        bool containsRequestsFor(const QString& scenarioName);

        bool isEmpty();
        int count();
};

#endif // ISSOSCENARIOQUEUE_H
