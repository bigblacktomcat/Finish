#include "issoscenarioexecutor.h"


IssoScenarioExecutor::IssoScenarioExecutor(IssoExchangeManager *exchanger, QObject *parent)
    : QObject(parent),
      m_exchanger(exchanger)
{
    connect(m_exchanger, SIGNAL(scenarioStopped(IssoScenarioInfo)),
            this,        SLOT(processScenarioStopped(IssoScenarioInfo)));
}

IssoScenarioExecutor::~IssoScenarioExecutor()
{
    if (m_exchanger)
    {
        disconnect(m_exchanger, 0, this, 0);
        m_exchanger = nullptr;
    }
}


IssoExchangeManager *IssoScenarioExecutor::getExchanger() const
{
    return m_exchanger;
}


void IssoScenarioExecutor::setObject(IssoObject *object)
{
    m_object = object;
}


void IssoScenarioExecutor::processScenarioStopped(IssoScenarioInfo scenarioInfo)
{
    // удалить сценарий из коллекции
    removeExecScenario(scenarioInfo.complexName());
    // известить об остановке сценария данного здания / этажа
    emit scenarioStateChanged(scenarioInfo, false);
}


QList<IssoModule *> IssoScenarioExecutor::getModulesByScope(IssoScenarioScope scope,
                                                            int buildNum, int floorNum)
{
    QList<IssoModule*> modules;
    switch(scope)
    {
        // область действия - здание
        case IssoScenarioScope::BUILDING:
        {
            // найти здание
            IssoBuilding* build = m_object->findBuildingByNumber(buildNum);
            if (!build)
                break;
            // получить все модули здания
            modules = build->modules();
            break;
        }
        // область действия - этаж
        case IssoScenarioScope::FLOOR:
        {
            // найти этаж
            IssoFloorPlan* floor = m_object->findFloor(buildNum, floorNum);
            if (!floor)
                break;
            // получить все модули здания
            modules = floor->modules();
            break;
        }
    }
    return modules;
}


void IssoScenarioExecutor::executeScenario(IssoScenario *scenario,
                                        IssoScenarioInfo scenarioInfo,
                                        const QList<IssoModule *> &modules)
{
    // поочередно активировать каждый класс сценария
    foreach (QString className, scenario->relayClassNames())
    {
        // получить класс по имени
        IssoRelayClass relayClass = m_object->relayClass(className);
        if (!relayClass.isValid())
            continue;
        // выполнить задержку пуска
        if (!delay(scenarioInfo.complexName(), relayClass.delay * 1000))
            break;
        // активировать класс для всех модулей
        activateRelayClass(scenarioInfo, className, modules);
    }
    // добавить терминальный запрос сценария (признак завершения)
    m_exchanger->addTerminalRequest(scenarioInfo);
}


void IssoScenarioExecutor::activateRelayClass(IssoScenarioInfo scenarioInfo,
                                              const QString &className,
                                              const QList<IssoModule *> &modules)
{
    if (!m_object || !m_exchanger)
        return;
    // список пар [moduleId : relayId]
    QList<QPair<int, IssoParamId> > moduleToRelayPairs;
    // для каждого модуля определить реле, связанные с классом
    foreach (IssoModule* module, modules)
    {
        // если модуль неактивен, не обрабатывать
        if (module->moduleState() == INACTIVE)
            continue;
        // получить конфиг модуля
        IssoModuleConfig* cfg = m_object->moduleConfig(module->moduleConfigName());
        if (!cfg)
            continue;
        // получить список Id реле, связанных с классом
        QList<IssoParamId> relaysIds = cfg->getRelayIdsByClassName(className);
        if (relaysIds.isEmpty())
            continue;
        // добавить пары [moduleId : relayId] в список
        foreach (IssoParamId relaysId, relaysIds)
            moduleToRelayPairs.append(QPair<int, IssoParamId>(module->uniqueId(), relaysId));
    }
    // добавить команды активации реле по сценарию
    m_exchanger->turnRelaysByScenario(scenarioInfo, moduleToRelayPairs, true);
}


bool IssoScenarioExecutor::delay(const QString &scenarioName, int msec)
{
    QTime time;
    time.start();
    // выдержать паузу с периодической проверкой прерывания
    while(time.elapsed() < msec)
    {
        // если сценарий остановлен, вернуть false
        if (!scenarioRunning(scenarioName))
            return false;
        // приостановить поток на 200 мс
        QThread::msleep(200);
    }
    return true;
}


bool IssoScenarioExecutor::scenarioRunning(const QString &scenarioName)
{
    return m_execScenarios.contains(scenarioName);
}


void IssoScenarioExecutor::insertExecScenario(const QString &scenarioName,
                                              const QFuture<void> &future)
{
    QMutexLocker locker(&m_mutex);
    m_execScenarios.insert(scenarioName, future);
}


void IssoScenarioExecutor::removeExecScenario(const QString &scenarioName)
{
    QMutexLocker locker(&m_mutex);
    m_execScenarios.remove(scenarioName);
}


QFuture<void> IssoScenarioExecutor::getScenarioTask(const QString &scenarioName)
{
    QMutexLocker locker(&m_mutex);
    return m_execScenarios.value(scenarioName);
}


void IssoScenarioExecutor::start(IssoScenarioInfo scenarioInfo)
{
    // сформировать имя сценария для данного этажа и здания
    // (имя-сценария_номер-здания_номер-этажа)
    QString specificName = scenarioInfo.complexName();
    // если сценарий уже запущен, выход
    if (scenarioRunning(specificName))
        return;
    //
    // запустить в отдельном потоке
    QtConcurrent::run([=]()
    {
        qDebug() << QString("Enqueue scenario [%1]").arg(specificName) << endl;
        //
        if (!m_object)
            return;
        // получить сценарий
        IssoScenario* scenario = m_object->scenario(scenarioInfo.name);
        if (!scenario)
            return;
        // получить список модулей согласно области действия сценария
        QList<IssoModule*> modules = getModulesByScope(scenario->scope(),
                                                       scenarioInfo.buildNum,
                                                       scenarioInfo.floorNum);
        // для списка модулей выполнить сценарий
        QFuture<void> future =
                QtConcurrent::run(this, &IssoScenarioExecutor::executeScenario,
                                                scenario, scenarioInfo, modules);
        // добавить в коллекцию
        insertExecScenario(specificName, future);
        // известить о запуске сценария данного здания / этажа
        emit scenarioStateChanged(scenarioInfo, true);
    });
}


void IssoScenarioExecutor::stop(IssoScenarioInfo scenarioInfo)
{
    // сформировать имя сценария для данного этажа и здания
    // (имя-сценария_номер-здания_номер-этажа)
    QString specificName = scenarioInfo.complexName();
    // если сценарий не запущен, выход
    if (!scenarioRunning(specificName))
        return;
    // запустить в отдельном потоке
    QtConcurrent::run([=]()
    {
        if (!m_exchanger)
            return;
        // получить QFuture
        auto future = getScenarioTask(specificName);
        // удалить сценарий из коллекции
        removeExecScenario(specificName);
        // удалить все команды, данного сценария из очереди на отправку
        m_exchanger->stopScenario(specificName);
        // дождаться завершения потока
        if (future.isRunning())
            future.waitForFinished();
        // известить об остановке сценария данного здания / этажа
        emit scenarioStateChanged(scenarioInfo, false);
    });
}

