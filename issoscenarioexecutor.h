#ifndef ISSOSCENARIOEXECUTOR_H
#define ISSOSCENARIOEXECUTOR_H

#include "issoexchangemanager.h"
#include "issoobject.h"
#include <QObject>
#include <QtConcurrent/QtConcurrent>


class IssoScenarioExecutor : public QObject
{
        Q_OBJECT
    private:
        IssoExchangeManager* m_exchanger = nullptr;
        IssoObject* m_object = nullptr;
        QMap<QString, QFuture<void>> m_execScenarios;

        QMutex m_mutex;

        QList<IssoModule*> getModulesByScope(IssoScenarioScope scope, int buildNum, int floorNum);

        void executeScenario(IssoScenario* scenario,
                           IssoScenarioInfo scenarioInfo,
                           const QList<IssoModule*>& modules);
        void activateRelayClass(IssoScenarioInfo scenarioInfo,
                                const QString& className,
                                const QList<IssoModule*>& modules);
        bool delay(const QString& scenarioName, int msec);
        bool scenarioRunning(const QString& scenarioName);

        void insertExecScenario(const QString& scenarioName, const QFuture<void>& future);
        void removeExecScenario(const QString& scenarioName);
        QFuture<void> getScenarioTask(const QString& scenarioName);

    public:
        explicit IssoScenarioExecutor(IssoExchangeManager* exchanger = nullptr,
                                      QObject *parent = nullptr);
        ~IssoScenarioExecutor();

        IssoExchangeManager *getExchanger() const;
        void start(IssoScenarioInfo scenarioInfo);
        void stop(IssoScenarioInfo scenarioInfo);

        void setObject(IssoObject *object);

    signals:
        void scenarioStateChanged(IssoScenarioInfo scenarioInfo, bool active);

    private slots:
        void processScenarioStopped(IssoScenarioInfo scenarioInfo);
};

#endif // ISSOSCENARIOEXECUTOR_H
