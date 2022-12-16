#include "issoscenarioqueue.h"


IssoScenarioQueue::IssoScenarioQueue()
{
}


void IssoScenarioQueue::append(const IssoScenarioRequest &scenarioRequest)
{
//    // заблокировать доступ к очереди
//    m_lock.lock();
//    // добавить запрос в очередь пользовательских запросов
//    m_queue.append(scenarioRequest);
//    // разблокировать доступ к очереди
//    m_lock.unlock();

    // заблокировать доступ к очереди до выхода из метода
    QMutexLocker locker(&m_lock);
    // добавить запрос в очередь пользовательских запросов
    m_queue.append(scenarioRequest);
}


void IssoScenarioQueue::append(const QList<IssoScenarioRequest> &scenarioRequests)
{
//    // заблокировать доступ к очереди
//    m_lock.lock();
//    // добавить запрос в очередь пользовательских запросов
//    m_queue.append(scenarioRequests);
//    // разблокировать доступ к очереди
//    m_lock.unlock();

    // заблокировать доступ к очереди до выхода из метода
    QMutexLocker locker(&m_lock);
    // добавить запрос в очередь пользовательских запросов
    m_queue.append(scenarioRequests);
}


IssoScenarioRequest IssoScenarioQueue::take()
{
//    IssoScenarioRequest request;
//    // блокировать доступ к очереди
//    m_lock.lock();
//    // взять первый запрос из очереди
//    if (!m_queue.isEmpty())
//        request = m_queue.takeFirst();
//    // разблокировать очередь
//    m_lock.unlock();
//    // вернуть результат
//    return request;

    IssoScenarioRequest request;
    // заблокировать доступ к очереди до выхода из метода
    QMutexLocker locker(&m_lock);
    // взять первый запрос из очереди
    if (!m_queue.isEmpty())
        request = m_queue.takeFirst();
    // вернуть результат
    return request;
}


void IssoScenarioQueue::removeAll(const QString &scenarioName)
{
//    // блокировать доступ к очереди
//    m_lock.lock();
//    // удалить все запросы заданного сценария
//    foreach (IssoScenarioRequest request, m_queue)
//    {
//        if (request.scenarioName() == scenarioName)
//            m_queue.removeOne(request);
//    }
//    // разблокировать очередь
//    m_lock.unlock();


    // заблокировать доступ к очереди до выхода из метода
    QMutexLocker locker(&m_lock);
    // удалить все запросы заданного сценария
    foreach (IssoScenarioRequest request, m_queue)
    {
        if (request.scenarioComplexName() == scenarioName)
            m_queue.removeOne(request);
    }
}


bool IssoScenarioQueue::containsRequestsFor(const QString &scenarioName)
{
    // заблокировать доступ к очереди до выхода из метода
    QMutexLocker locker(&m_lock);
    // удалить все запросы заданного сценария
    foreach (IssoScenarioRequest request, m_queue)
    {
        if (request.scenarioComplexName() == scenarioName)
            return true;
    }
    return false;
}


bool IssoScenarioQueue::isEmpty()
{
    return m_queue.isEmpty();
}


int IssoScenarioQueue::count()
{
    return m_queue.size();
}

