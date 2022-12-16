#include "issomodule.h"
#include "issocore.h"
#include "issoexchangemanager.h"
#include "issorequestsender.h"

IssoExchangeManager::IssoExchangeManager(const QHostAddress &ip, quint16 bindPort, quint16 alarmPort,
//                                         const QList<IssoModule *> &modules,
                                         QObject *parent)
    : QObject(parent),
      m_ip(ip),
      m_bindPort(bindPort)/*,
      m_modules(modules)*/
{
//    setModules(modules);
    // слушатель тревожных сообщений
    m_alarmReceiver = new IssoAlarmReceiver(alarmPort);
    // блокировка доступа к очереди запросов пользователя
    m_userLock = new QMutex();
    // блокировка доступа к номеру пакета
    m_packNumLock = new QMutex();
    // обработчик тревог
    connect(m_alarmReceiver,    SIGNAL(alarmReceived(IssoModuleReply)),
            this,               SIGNAL(alarmReceived(IssoModuleReply)));
    connect(m_alarmReceiver,    SIGNAL(dataReceived(QHostAddress,quint16,QByteArray)),
            this,               SIGNAL(alarmDataReceived(QHostAddress,quint16,QByteArray)));

//    // заполнить карту соответствия ID параметра датчика / шлейфа
//    // и ID параметра пороговых значений
//    initThresholdParamIds();
}


IssoExchangeManager::~IssoExchangeManager()
{
    qDebug() << "~IssoExchangeManager";
    //
    if (m_userLock)
    {
        delete m_userLock;
        m_userLock = nullptr;
    }
    if (m_packNumLock)
    {
        delete m_packNumLock;
        m_packNumLock = nullptr;
    }
    if (m_alarmReceiver)
    {
        delete m_alarmReceiver;
        m_alarmReceiver = nullptr;
    }
}

/**
 * @brief IssoExchangeManager::findModule найти модуль по Id
 * @param moduleId Id модуля
 * @return указатель на модуль с Id
 */
IssoModule *IssoExchangeManager::findModule(const int moduleId)
{
    return (m_object ? m_object->findModuleById(moduleId, true)
                     : nullptr);

//    foreach (IssoModule* module, m_modules)
//    {
//        if (module->uniqueId() == moduleId)
//            return module;
//    }
//    return nullptr;
}

/**
 * @brief IssoExchangeManager::findModuleAddressById найти IP и порт модуля по его Id
 * @param moduleId
 * @param addr
 * @param port
 * @return false если модуль не найден
 */
bool IssoExchangeManager::findModuleAddressById(const int moduleId,
                                                QHostAddress &addr, quint16 &port)
{
    // если широковещательный запрос
    if (moduleId == MODULE_ID_BROADCAST)
    {
        // адрес
        addr = QHostAddress::Broadcast;
        // порт отправки по умолчанию
        port = IssoConst::SEND_PORT;
        return true;
    }
    else
    {
        // найти модуль по id
        IssoModule* module = findModule(moduleId);
        // если не найден, вернуть неуспех
        if (!module)
            return false;

        // если модуль активирован, то вернуть его ip и порт
        if (module->isActivated())
        {
            // адрес
            addr = module->ip();
            // порт
            port = module->port();
        }
        // иначе заводские ip и порт
        else
        {
            // адрес
            addr = IssoConst::DEFAULT_MODULE_IP;
            // порт
            port = IssoConst::SEND_PORT;
        }
        return true;
    }
}
//bool IssoExchangeManager::findModuleAddressById(const int moduleId, QHostAddress &addr, quint16 &port)
//{
//    // если широковещательный запрос
//    if (moduleId == MODULE_ID_BROADCAST)
//    {
//        // если список модулей пуст, вернуть неуспех
//        if (m_modules.size() < 0)
//            return false;
//        // адрес
//        addr = QHostAddress::Broadcast;
//        // порт первого модуля в списке
//        // (номер порта должен быть один для всех модулей)
//        port = m_modules.first()->port();
//        return true;
//    }
//    else
//    {
//        // найти модуль по id
//        IssoModule* module = findModule(moduleId);
//        // если не найден, вернуть неуспех
//        if (!module)
//            return false;

//        // если модуль активирован, то вернуть его ip и порт
//        if (module->isActivated())
//        {
//            // адрес
//            addr = module->ip();
//            // порт
//            port = module->port();
//        }
//        // иначе заводские ip и порт
//        else
//        {
//            // адрес
//            addr = IssoConst::DEFAULT_MODULE_IP;
//            // порт
//            port = IssoConst::SEND_PORT;
//        }
//        return true;
//    }
//}


//IssoMultiSensorConfig *IssoExchangeManager::findMultiSensorCfg(int moduleId,
//                                                               quint8 multiSensorId)
//{
//    if (!m_object)
//        return nullptr;
//    // найти модуль
//    auto module = m_object->findModuleById(moduleId);
//    if (!module)
//        return nullptr;
//    // вернуть конфиг мультидатчика
//    return m_object->moduleMultiSensorConfig(module->moduleConfigName(), multiSensorId);
//}


//void IssoExchangeManager::setModules(const QList<IssoModule *> &modules)
//{
//    m_modules = modules;
//    // сортировать модули по ip
//    std::sort(m_modules.begin(), m_modules.end(),
//          [](const IssoModule* a, const IssoModule* b) ->
//                        bool { return a->ip().toIPv4Address() < b->ip().toIPv4Address(); });
//}


IssoObject *IssoExchangeManager::object() const
{
    return m_object;
}

void IssoExchangeManager::setObject(IssoObject *object)
{
    m_object = object;
}

/**
 * @brief IssoExchangeManager::testHandleActivationRequests проверить наличие запросов на активацию МД и обработать их
 * @param sender
 * @return
 */
bool IssoExchangeManager::testHandleActivationRequests(IssoRequestSender &sender)
{
    Q_UNUSED(sender);

    if (m_activationRequests.isEmpty())
        return true;

    bool result = true;
    // количество активирванных мультидатчиков
    int curCount = 0;
    // пока очередь запросов активации не пуста, обрабатывать запросы
    while(!m_activationRequests.isEmpty())
    {
        // блокировать доступ к очереди
        m_activationLock.lock();
        //
        // взять из очереди запрос существования активированного мультидатчика
        IssoModuleRequest existActivatedRequest = m_activationRequests.takeFirst();
        // взять из очереди запрос существования мультидатчика с заводским адресом
        IssoModuleRequest existNonactivatedRequest = m_activationRequests.takeFirst();
        // взять из очереди запрос изменения адреса мультидатчика
        IssoModuleRequest changeAddrRequest = m_activationRequests.takeFirst();
        // взять из очереди запрос существования мультидатчика
        IssoModuleRequest saveRequest = m_activationRequests.takeFirst();
        //
        // разблокировать очередь
        m_activationLock.unlock();
        // номера этапа активации
        int stage = 0;
        // пока активацию выполняется, посылать запросы
        while(m_running && m_activationRunning)
        {
            QSharedPointer<IssoModuleReply> replyPtr;

            // получить параметр ответа
            auto msAddrParam = dynamic_cast<IssoByteParam*>(
                                existActivatedRequest.command().paramById(MULTISENSOR_ADDR));
            if (!msAddrParam)
                break;
            // получить адрес (ID) мультидатчика
            quint8 msAddr = msAddrParam->byteValue();

            // определить наличие активированного мультидатчика
            if (stage == 0)
            {
                emit progressInfo(tr("Запрос наличия мультидатчика-%1").arg(msAddr));

                QThread::currentThread()->sleep(3);
                if (!m_activationRunning)
                    break;

                // пропустить на следующий этап
                stage = 1;
            }
            // определить наличие неактивированного мультидатчика
            if (stage == 1)
            {
                emit progressInfo(tr("Запрос наличия неактивированного мультидатчика"));

                // задержка
                QThread::currentThread()->sleep(3);
                if (!m_activationRunning)
                    break;

                // пропустить на следующий этап
                stage = 2;
            }
            // изменить адрес мультидатчика
            if (stage == 2)
            {
                emit progressInfo(tr("Запрос активации мультидатчика-%1").arg(msAddr));

                // задержка
                QThread::currentThread()->sleep(3);
                if (!m_activationRunning)
                    break;

                // пропустить на следующий этап
                stage = 3;
            }
            // сохранить данные в ПЗУ мультидатчика
            if (stage == 3)
            {
                emit progressInfo(tr("Запрос сохранения данных мультидатчика-%1").arg(msAddr));

                // задержка
                QThread::currentThread()->sleep(3);
                if (!m_activationRunning)
                    break;

                // известить об успешной активации мультидатчика
                emit multiSensorActivated(saveRequest.address(), msAddr);

                // завершить активацию текущего мультидатчика
                stage = 0;
                // обновить диалог прогресса
                emit progressCurrent(++curCount);
                break;
            }
        }
        // проверить активность
        if (!m_running)
        {
            result = false;
            break;
        }
    }
    emit progressFinished();
    return result;
}

/**
 * @brief IssoExchangeManager::handleActivationRequests обработать запросы на активацию мультидатчиков
 * @param sender
 * @return
 */
bool IssoExchangeManager::handleActivationRequests(IssoRequestSender &sender)
{
    if (m_activationRequests.isEmpty())
        return true;

    bool result = true;
    bool ok = false;
    // количество активирванных мультидатчиков
    int curCount = 0;



    //
    // блокировать доступ к очереди
    m_activationLock.lock();
    // взять из очереди запрос существования активированного мультидатчика
    IssoModuleRequest setPollAddrsRequest = m_activationRequests.takeFirst();
    // разблокировать очередь
    m_activationLock.unlock();
    // пока активацию выполняется, посылать запросы
    while(m_running && m_activationRunning)
    {
        emit progressInfo(tr("Запрос установки адресов мультидатчиков для опроса"));
        //
        // отправить запрос
        sendRequest(sender, setPollAddrsRequest, ok);
        // если корректный ответ не получен, повторить
        if (!ok)
        {
            // задержка
            QThread::currentThread()->msleep(200);
            continue;
        }
        // обновить диалог прогресса
        emit progressCurrent(++curCount);
        break;
    }



    //
    // пока очередь запросов активации не пуста, обрабатывать запросы
    while(!m_activationRequests.isEmpty())
    {
        // блокировать доступ к очереди
        m_activationLock.lock();
        //
        // взять из очереди запрос существования активированного мультидатчика
        IssoModuleRequest existActivatedRequest = m_activationRequests.takeFirst();
        // взять из очереди запрос существования мультидатчика с заводским адресом
        IssoModuleRequest existNonactivatedRequest = m_activationRequests.takeFirst();
        // взять из очереди запрос изменения адреса мультидатчика
        IssoModuleRequest changeAddrRequest = m_activationRequests.takeFirst();
        // взять из очереди запрос существования мультидатчика
        IssoModuleRequest saveRequest = m_activationRequests.takeFirst();
        // взять из очереди запрос сигнала бипера
        IssoModuleRequest beepRequest = m_activationRequests.takeFirst();
        //
        // разблокировать очередь
        m_activationLock.unlock();
        // номера этапа активации
        int stage = 0;
        // пока активацию выполняется, посылать запросы
        while(m_running && m_activationRunning)
        {
            QSharedPointer<IssoModuleReply> replyPtr;
            //
            // получить параметр ответа
            auto msAddrParam = dynamic_cast<IssoByteParam*>(
                                existActivatedRequest.command().paramById(MULTISENSOR_ADDR));
            if (!msAddrParam)
                break;
            // получить адрес (ID) мультидатчика
            quint8 msAddr = msAddrParam->byteValue();
            //
            // определить наличие активированного мультидатчика
            if (stage == 0)
            {
                emit progressInfo(tr("Запрос наличия мультидатчика-%1").arg(msAddr));
                //
                // отправить запрос
                auto replyPtr = sendRequest(sender, existActivatedRequest, ok);
                // если корректный ответ не получен, повторить
                if (!ok)
                {
                    // задержка
                    QThread::currentThread()->msleep(200);
                    continue;
                }
                // получить параметр ответа
                IssoParam* param = replyPtr.data()->command().paramById(MULTISENSOR_EXIST_DATA);
                auto existParam = dynamic_cast<IssoMultiSensorExistDataParam*>(param);
                // если устройство обнаружено, перейти к следующему мультидатчику
                if (existParam && (existParam->address() != 0))
                {
                    // известить об успешной активации мультидатчика
                    emit multiSensorActivated(replyPtr.data()->address(),
                                              existParam->address());
                    // завершить активацию текущего мультидатчика
                    stage = 0;
                    // обновить диалог прогресса
                    emit progressCurrent(++curCount);
                    break;

                }
                // пропустить на следующий этап
                stage = 1;
            }
            //
            // определить наличие неактивированного мультидатчика
            if (stage == 1)
            {
                emit progressInfo(tr("Запрос наличия неактивированного мультидатчика"));
                //
                // отправить запрос
                auto replyPtr = sendRequest(sender, existNonactivatedRequest, ok);
                // если корректный ответ не получен, повторить
                if (!ok)
                {
                    // задержка
                    QThread::currentThread()->msleep(200);
                    continue;
                }
                // получить параметр ответа
                IssoParam* param = replyPtr.data()->command().paramById(MULTISENSOR_EXIST_DATA);
                auto existParam = dynamic_cast<IssoMultiSensorExistDataParam*>(param);
                // если устройство не обнаружено, повторить
                if (!existParam || (existParam->address() == 0))
                {
                    // задержка
                    QThread::currentThread()->msleep(200);
                    continue;
                }
                // пропустить на следующий этап
                stage = 2;
            }
            //
            // изменить адрес мультидатчика
            if (stage == 2)
            {
                emit progressInfo(tr("Запрос активации мультидатчика-%1").arg(msAddr));
                //
                // отправить запрос
                replyPtr = sendRequest(sender, changeAddrRequest, ok);
                // если корректный ответ не получен, повторить
                if (!ok)
                {
                    // задержка
                    QThread::currentThread()->msleep(200);
                    continue;
                }
                // пропустить на следующий этап
                stage = 3;
            }
            //
            // сохранить данные в ПЗУ мультидатчика
            if (stage == 3)
            {
                emit progressInfo(tr("Запрос сохранения данных мультидатчика-%1").arg(msAddr));
                //
                // отправить запрос
                replyPtr = sendRequest(sender, saveRequest, ok);
                // если корректный ответ не получен, повторить
                if (!ok)
                {
                    // задержка
                    QThread::currentThread()->msleep(200);
                    continue;
                }
                // пропустить на следующий этап
                stage = 4;
                //
                // задержка (длительная процедура)
                QThread::currentThread()->msleep(100);
            }
            //
            // воспроизвести звуковой сигнал
            if (stage == 4)
            {
                emit progressInfo(tr("Запрос звукового сигнала мультидатчика-%1").arg(msAddr));
                //
                // отправить запрос
                replyPtr = sendRequest(sender, beepRequest, ok);
                // если корректный ответ не получен, повторить
                if (!ok)
                {
                    // задержка
                    QThread::currentThread()->msleep(200);
                    continue;
                }
                // получить параметр ответа
                IssoParam* param = replyPtr.data()->command().paramById(SINGLE_REG_DATA);
                auto beepParam = dynamic_cast<IssoSingleRegInfoParam*>(param);
                // если параметр не тот, что ожидается, повторить
                if (!beepParam)
                {
                    // задержка
                    QThread::currentThread()->msleep(200);
                    continue;
                }
                // известить об успешной активации мультидатчика
                emit multiSensorActivated(replyPtr.data()->address(),
                                          beepParam->multiSensorId());
                // завершить активацию текущего мультидатчика
                stage = 0;
                // обновить диалог прогресса
                emit progressCurrent(++curCount);
                break;
            }
        }
        // проверить активность
        if (!m_running)
        {
            result = false;
            break;
        }
    }
    emit progressFinished();
    return result;
}

/**
 * @brief IssoExchangeManager::handleUserRequests обработать очередь запросов пользователя
 * @param sender
 * @return
 */
bool IssoExchangeManager::handleUserRequests(IssoRequestSender &sender)
{
    bool ok = false;
    // пока очередь запросов пользователя не пуста, обрабатывать запросы
    while(!m_userRequests.isEmpty())
    {
        // блокировать доступ к очереди
        m_userLock->lock();
        // взять первый запрос из очереди
        IssoModuleRequest userRequest = m_userRequests.takeFirst();
        // разблокировать очередь
        m_userLock->unlock();
        //
        // отправить запрос и получить ответ
        auto replyPtr = sendRequest(sender, userRequest, ok);


        // проанализировать ответ
        sleepIfNeed(replyPtr);

//        if (replyPtr)
//        {
//            switch(replyPtr.data()->command().id())
//            {
//                // если сброс состояния мультидатчика, выдержать паузу
//                case CMD_MS_SINGLE_HOLD_REG_SET:
//                {
//                    auto setRegParam =
//                            dynamic_cast<IssoSingleRegInfoParam*>(
//                                        replyPtr.data()->command().paramById(SINGLE_REG_DATA));
//                    if (setRegParam && setRegParam->regAddress() == 24)
//                    {
//                        qDebug() << "Задержка опроса" << endl;
//                        QThread::currentThread()->msleep(1600);
//                    }
//                    break;
//                }
//                case CMD_MS_POLL_ADDRS:
//                {
//                    QThread::currentThread()->msleep(100);
//                    break;
//                }
//                default:
//                    break;
//            }
//        }


        //
        // проверить активность
        if (!m_running)
            return false;
    }
    return true;
}

/**
 * @brief IssoExchangeManager::handleScenarioRequests обработать запросы по сценарию
 * @param sender
 * @return
 */
bool IssoExchangeManager::handleScenarioRequests(IssoRequestSender &sender)
{
    bool ok = false;
    // пока очередь запросов сценариев не пуста, обрабатывать запросы
    while(!m_scenarioRequests.isEmpty())
    {
//        // DEBUG
//        qDebug() << "scenarioRequests.size() = " << m_scenarioRequests.count() << endl;

        // взять запрос сценария из очереди
        IssoScenarioRequest scenarioRequest = m_scenarioRequests.take();
        if (!scenarioRequest.isValid())
            continue;
        //
        // DEBUG
        qDebug() << "Handle scenario request: " << scenarioRequest.toString() << endl;
        //
        // если запрос сценария терминальный
        if (scenarioRequest.isTerminal())
            // известить о завершении сценария
            emit scenarioStopped(scenarioRequest.info());
        else
            // иначе отправить запрос и получить ответ
            auto replyPtr = sendRequest(sender, scenarioRequest.request(), ok);
        //
        // проверить активность
        if (!m_running)
            return false;
    }
    return true;
}


void IssoExchangeManager::sleepIfNeed(QSharedPointer<IssoModuleReply> replyPtr)
{
    if (replyPtr.isNull())
        return;
    // проанализировать
    switch(replyPtr.data()->command().id())
    {
//        // команда записи одиночного регистра
//        case CMD_MS_SINGLE_HOLD_REG_SET:
//        {
//            // параметр записи регистра
//            auto setRegParam = dynamic_cast<IssoSingleRegInfoParam*>(
//                                        replyPtr.data()->command().paramById(SINGLE_REG_DATA));
//            // если задано состояние мультидатчика, выдержать паузу
//            if (setRegParam && setRegParam->regAddress() == IssoHoldReg::MS_STATE)
//            {
//                QThread::currentThread()->msleep(1600);
//            }
//            break;
//        }

        // команда записи одиночного регистра
        case CMD_MS_SINGLE_HOLD_REG_SET:
        // команда записи блока регистров
        case CMD_MS_BLOCK_HOLD_REG_SET:
        {
            // выдержать паузу
            QThread::currentThread()->msleep(200);
            break;
        }
        // команда задания адресов мультидатчиков для опроса
        case CMD_MS_POLL_ADDRS:
        {
            // выдержать паузу
            QThread::currentThread()->msleep(100);
            break;
        }
        default:
            break;
    }
}

/**
 * @brief IssoExchangeManager::addToUserQueue добавить запрос в очередь пользовательских запросов
 * @param request
 */
void IssoExchangeManager::addToUserQueue(const IssoModuleRequest &request)
{
    // заблокировать доступ к очереди
    m_userLock->lock();
    // добавить запрос в очередь пользовательских запросов
    m_userRequests.append(request);
    //
    qDebug() << "User command added: " << (request).bytes();
    //
    // разблокировать доступ к очереди
    m_userLock->unlock();
}


void IssoExchangeManager::addToUserQueue(const QList<IssoModuleRequest> &requests)
{
    // заблокировать доступ к очереди
    m_userLock->lock();
    // добавить запрос в очередь пользовательских запросов
    m_userRequests.append(requests);
    // разблокировать доступ к очереди
    m_userLock->unlock();
}

/**
 * @brief IssoExchangeManager::addToUserQueueAsync Ставим асинхронный запрос в очередь
 * @param request
 */
void IssoExchangeManager::addToUserQueueAsync(const IssoModuleRequest &request)
{
    // выполнить асинхронный вызов добавления запроса в очередь
    QtConcurrent::run(this, QOverload<const IssoModuleRequest&>::of(
                          &IssoExchangeManager::addToUserQueue), request);
}

/**
 * @brief IssoExchangeManager::addToUserQueueAsync Ставим набор асинхронных запросов в очередь
 * @param requests
 */
void IssoExchangeManager::addToUserQueueAsync(const QList<IssoModuleRequest> &requests)
{
    // выполнить асинхронный вызов добавления запроса в очередь
    QtConcurrent::run(this, QOverload<const QList<IssoModuleRequest> &>::of(
                                &IssoExchangeManager::addToUserQueue), requests);
}


void IssoExchangeManager::addToActivationQueue(const QList<IssoModuleRequest> &requests)
{
    QtConcurrent::run([=]()
        {
            // заблокировать доступ к очереди
            m_activationLock.lock();
            // добавить запросы в очередь запросов активации
            m_activationRequests.append(requests);
            // разблокировать доступ к очереди
            m_activationLock.unlock();
    });
}


void IssoExchangeManager::clearActivationQueue()
{
    QtConcurrent::run([=]()
        {
            // заблокировать доступ к очереди
            m_activationLock.lock();
            // очистить очередь запросов активации
            m_activationRequests.clear();
            // разблокировать доступ к очереди
            m_activationLock.unlock();
    });
}


QSharedPointer<IssoModuleReply> IssoExchangeManager::sendRequest(IssoRequestSender& sender,
                                                                 const IssoModuleRequest& request,
                                                                 bool& ok)
{
    // отправить запрос и получить ответ
    IssoModuleReply* reply = sender.sendRequestRepeatedly(request, ok, IssoConst::SEND_TRY_COUNT);
    // сформировать ответ
    auto result = (reply ? QSharedPointer<IssoModuleReply>(reply) :
                           QSharedPointer<IssoModuleReply>());
    //
    // сгенерировать события:
    //
    if (!ok)
    {
        // если корректный ответ не получен, то известить о выходе таймаута
        emit replyTimedOut(request);
    }
    else
    {
        // известить о приеме ответа
        if (reply)
            emit replyReceived(*reply, ok);
    }
    //
    // вернуть ответ
    return result;
}

/**
 * @brief IssoExchangeManager::requestModuleInfo команды INFO и MASG
 * @param moduleId
 */
void IssoExchangeManager::requestModuleInfo(int moduleId)
{
    // получаем инфо о модуле
    requestStateAsync(moduleId, CMD_INFO);
    // получаем инфо о присоединенных датчиках
    // сканирования шины не происходит
    requestStateAsync(moduleId, CMD_MS_INFO);
    //requestStateAsync(moduleId, CMD_MULTISENSOR_EXIST_GET);
    requestAllMVSG(moduleId); // CHA 2020-09-17
 }

/**
 * @brief IssoExchangeManager::requestMsRawInfo  запрос общего состояния мультидатчика CMD_MS_INFO
 * запрос сырого состояния мультидатчика CMD_MS_INFO_RAW
 * @param moduleId
 * @param multiSensorId
 */

void IssoExchangeManager::requestMsRawInfo(int moduleId, quint8 multiSensorId)
{
    QHostAddress ip;
    quint16 port;
    // получить ip и порт целевого модуля
    if (!findModuleAddressById(moduleId, ip, port))
        return;
    // список запросов
    QList<IssoModuleRequest> requests;
//    //
//    // команда 1: запрос сырого состояния мультидатчика
//    //
//    // создать буфер параметров запроса
//    IssoParamBuffer params;
//    // добавить параметр адреса мультидатчика
//    params.insertParam(new IssoByteParam(MULTISENSOR_ADDR, multiSensorId));
//    // сформировать запрос
//    requests << IssoModuleRequest(ip, port, nextPackNum(), CMD_MULTISENSOR_INFO_RAW, params);
//    //
//    // команда 2: запрос общего состояния мультидатчика
//    //
//    // сформировать и добавить запрос
//    requests << IssoModuleRequest(ip, port, nextPackNum(), CMD_MULTISENSORS_INFO);
//    //


    //
    // команда 1: запрос общего состояния мультидатчика
    //
    // сформировать и добавить запрос
    requests << IssoModuleRequest(ip, port, nextPackNum(), CMD_MS_INFO);
    //
    //
    // команда 2: запрос сырого состояния мультидатчика
    //
    // создать буфер параметров запроса
    IssoParamBuffer params;
    // добавить параметр адреса мультидатчика
    params.insertParam(new IssoByteParam(MULTISENSOR_ADDR, multiSensorId));
    // сформировать запрос
    requests << IssoModuleRequest(ip, port, nextPackNum(), CMD_MS_INFO_RAW, params);
    //

    // асинхронно добавить запросы в очередь
    addToUserQueueAsync(requests);
}
/**
 * @brief IssoExchangeManager::requestAllMVSG формирует пакет запросов для проверки
 * состояния мультидатчиков
 * @param moduleId Id выбранный модуль
 */

void IssoExchangeManager::requestAllMVSG(int moduleId)
{
    QHostAddress ip;
    quint16 port;
    auto module = m_object->findModuleById(moduleId, true);
    // получить ip и порт целевого модуля
    if (!findModuleAddressById(moduleId, ip, port)) return;
    // список запросов
    QList<IssoModuleRequest> requests;
    // список запросов
    //
    // запрос установки адресов мультидатчиков для опроса модулем:
    QByteArray idArray;
    QByteArray multiSensorIds = moduleMultiSensorAddrs(module);
    requests.clear();
   // foreach (quint8 id,  multiSensorIds)
   // {
   //     idArray.append(id);
   // }
   // requests << makeMultiSensorPollingAddrsRequest(ip, port, idArray);
    //
    //
    // для каждого мультидатчика добавить запрос его существования в сети
    //bool ok;
    //IssoModuleReply* reply = nullptr;
    foreach (quint8 id,  multiSensorIds)
    {
        IssoParamBuffer params;

        //
        // запрос существования активированного датчика (со следующим по порядку адресом):
        //
        // сформировать запрос
        // добавить параметр адреса мультидатчика
        params.insertParam(new IssoByteParam(MULTISENSOR_ADDR, id));
        // сформировать запрос
        requests << IssoModuleRequest(ip, port, nextPackNum(), CMD_MULTISENSOR_EXIST_GET, params);
       // reply = sendRequest(requests,ok);

    }
    //

    // добавить запросы в очередь для обработки
    addToUserQueueAsync(requests);
}


void IssoExchangeManager::stopMsActivation()
{
    // остановить активацию
    m_activationRunning = false;
    // очистить очередь запросов активации мультидатчиков
    clearActivationQueue();
}


void IssoExchangeManager::updateModuleConfig(int moduleId)
{
    if (!m_object)
        return;
    IssoModule* module = findModule(moduleId);
    if (!module)
        return;
//    // получить конфиг модуля
//    IssoModuleConfig* moduleCfg = m_object->moduleConfig(module->getModuleConfigName());
//    if (!moduleCfg)
//        return;

//    // ВРЕМЕННО ЗАКОММЕНТИРОВАНО
////    // установить пороговые значения состояний для встроенных датчиков
////    setupSensorsThresholds(module->ip(), module->port(), moduleCfg);
////    // установить пороговые значения состояний для аналоговых шлейфов
////    setupAnalogChainsThresholds(module->ip(), module->port(), moduleCfg);
//    //
//    // задать список адресов мультидатчиков для опроса модулем
//    setupMultiSensorPollingAddrs(module->ip(), module->port(), moduleCfg);

    setupMultiSensorPollingAddrs(module);
}


/**
 * @brief IssoExchangeManager::readMsInputRegisterSingle команда MIRG CMD_MS_SINGLE_INPUT_REG_GET
 * @param moduleId
 * @param multiSensorId
 * @param regAddress
 */

void IssoExchangeManager::readMsInputRegisterSingle(int moduleId,
                                                       quint8 multiSensorId,
                                                       quint8 regAddress)
{
    QHostAddress ip;
    quint16 port;
    // получить ip и порт целевого модуля
    if (!findModuleAddressById(moduleId, ip, port))
        return;
    // создать буфер параметров запроса
    IssoParamBuffer params;
    // добавить параметр чтения регистра
    params.insertParam(new IssoSingleRegAddrParam(SINGLE_REG_ADDR, multiSensorId, regAddress));
    // сформировать запрос
    IssoModuleRequest request(ip, port, nextPackNum(), CMD_MS_SINGLE_INPUT_REG_GET, params);
    // асинхронно добавить запрос в очередь
    addToUserQueueAsync(request);
}

/**
 * @brief IssoExchangeManager::readMsHoldRegisterSingle команда MHMG CMD_MS_SINGLE_HOLD_REG_GET
 * @param moduleId
 * @param multiSensorId
 * @param regAddress
 */
void IssoExchangeManager::readMsHoldRegisterSingle(int moduleId,
                                                         quint8 multiSensorId,
                                                         quint8 regAddress)
{
    QHostAddress ip;
    quint16 port;
    // получить ip и порт целевого модуля
    if (!findModuleAddressById(moduleId, ip, port))
        return;
    // создать буфер параметров запроса
    IssoParamBuffer params;
    // добавить параметр чтения регистра
    params.insertParam(new IssoSingleRegAddrParam(SINGLE_REG_ADDR, multiSensorId, regAddress));
    // сформировать запрос
    IssoModuleRequest request(ip, port, nextPackNum(), CMD_MS_SINGLE_HOLD_REG_GET, params);
    // асинхронно добавить запрос в очередь
    addToUserQueueAsync(request);
}

/**
 * @brief IssoExchangeManager::writeMsHoldRegisterSingle команда MHMS
 * @param moduleId
 * @param multiSensorId
 * @param regAddress
 * @param regValue
 */
void IssoExchangeManager::writeMsHoldRegisterSingle(int moduleId, quint8 multiSensorId,
                                                    quint8 regAddress, quint16 regValue)
{
    QHostAddress ip;
    quint16 port;
    // получить ip и порт целевого модуля
    if (!findModuleAddressById(moduleId, ip, port))
        return;
    // создать буфер параметров запроса
    IssoParamBuffer params;
    // добавить параметр записи регистра
    params.insertParam(new IssoSingleRegInfoParam(SINGLE_REG_DATA, multiSensorId,
                                                 regAddress, regValue));
    // сформировать запрос
    IssoModuleRequest request(ip, port, nextPackNum(), CMD_MS_SINGLE_HOLD_REG_SET, params);
    // асинхронно добавить запрос в очередь
    addToUserQueueAsync(request);
}

/**
 * @brief IssoExchangeManager::readMsHoldRegisterBlock команда MHMG
 * @param moduleId
 * @param multiSensorId
 * @param regAddress
 * @param regCount
 */
void IssoExchangeManager::readMsHoldRegisterBlock(int moduleId, quint8 multiSensorId,
                                                  quint8 regAddress, quint8 regCount)
{
    QHostAddress ip;
    quint16 port;
    // получить ip и порт целевого модуля
    if (!findModuleAddressById(moduleId, ip, port))
        return;
    // создать буфер параметров запроса
    IssoParamBuffer params;
    // добавить параметр чтения блока регистров
    params.insertParam(new IssoBlockRegAddrParam(BLOCK_REG_ADDR, multiSensorId,
                                                 regAddress, regCount));
    // сформировать запрос
    IssoModuleRequest request(ip, port, nextPackNum(), CMD_MS_BLOCK_HOLD_REG_GET, params);
    //
    // асинхронно добавить запрос в очередь
    addToUserQueueAsync(request);
}

/**
 * @brief IssoExchangeManager::writeMsHoldRegisterBlock команда MHMS
 * @param moduleId
 * @param multiSensorId
 * @param regAddress
 * @param regArray
 * @param romValue
 */
void IssoExchangeManager::writeMsHoldRegisterBlock(int moduleId, quint8 multiSensorId,
                                                   quint8 regAddress, const QByteArray &regArray,
                                                   quint16 romValue)
{
    QHostAddress ip;
    quint16 port;
    // получить ip и порт целевого модуля
    if (!findModuleAddressById(moduleId, ip, port))
        return;
    //
    // список запросов
    QList<IssoModuleRequest> requests;
    //
    // создать буфер параметров запроса
    IssoParamBuffer params;
    // добавить параметр записи блока регистров
    params.insertParam(new IssoBlockRegInfoParam(BLOCK_REG_DATA,
                                                 multiSensorId, regAddress, regArray));
    // запрос 1 (записать блок регистров)
    requests << IssoModuleRequest(ip, port, nextPackNum(), CMD_MS_BLOCK_HOLD_REG_SET, params);
//    IssoModuleRequest request(ip, port, nextPackNum(), CMD_MS_BLOCK_HOLD_REG_SET, params);
    //
    // если нужно сохранить в ПЗУ, то добавить запрос
    switch(romValue)
    {
        case IssoRomCmdValue::WRITE_IDENT:
        case IssoRomCmdValue::WRITE_THRES:
        case IssoRomCmdValue::WRITE_UINFO:
        {
            // запрос 2 (сохранить в ПЗУ)
            //
            // очистить буфер параметров
            params.clearParams();
            // добавить параметр записи в ПЗУ
            params.insertParam(new IssoSingleRegInfoParam(SINGLE_REG_DATA, multiSensorId,
                                                          IssoHoldReg::ROM_CMD, romValue));
            // добавить в список
            requests << IssoModuleRequest(ip, port, nextPackNum(),
                                          CMD_MS_SINGLE_HOLD_REG_SET, params);
            break;
        }
        default:
            break;
    }
    // асинхронно добавить запрос в очередь
    addToUserQueueAsync(requests);
//    addToUserQueueAsync(request);
}

/**
 * @brief IssoExchangeManager::startMsActivation активация MS
 * @param moduleId
 * @param msIds
 */

void IssoExchangeManager::startMsActivation(int moduleId, const QList<quint8> &msIds)
{
    QHostAddress ip;
    quint16 port;
    // получить ip и порт целевого модуля
    if (!findModuleAddressById(moduleId, ip, port))
        return;

    // заводской адрес мультидатчика
//    quint8 defaultAddr = 247;
    quint8 defaultAddr = IssoConst::DEFAULT_MS_ADDR;
    // список запросов
    QList<IssoModuleRequest> requests;
    //


    //
    // запрос установки адресов мультидатчиков для опроса модулем:
    QByteArray idArray;
    foreach (quint8 id, msIds)
    {
        idArray.append(id);
    }
    requests << makeMultiSensorPollingAddrsRequest(ip, port, idArray);
    //


    //
    // для каждого мультидатчика добавить запрос его существования в сети
    foreach (quint8 id, msIds)
    {
        IssoParamBuffer params;
        //
        // запрос существования активированного датчика (со следующим по порядку адресом):
        //
        // добавить параметр адреса мультидатчика
        params.insertParam(new IssoByteParam(MULTISENSOR_ADDR, id));
        // сформировать запрос
        IssoModuleRequest existActivatedRequest =
                IssoModuleRequest(ip, port, nextPackNum(), CMD_MULTISENSOR_EXIST_GET, params);
        //
        // запрос существования неактивированного датчика (с заводским адресом = 247):
        //
        // добавить параметр адреса мультидатчика
        params.insertParam(new IssoByteParam(MULTISENSOR_ADDR, defaultAddr));
        // сформировать запрос
        IssoModuleRequest existNonactivatedRequest =
                IssoModuleRequest(ip, port, nextPackNum(), CMD_MULTISENSOR_EXIST_GET, params);
        //
        // запрос изменения адреса мультидатчика:
        //
        params.clearParams();
        // добавить параметр записи регистра
//        params.insertParam(new IssoSingleRegInfoParam(SINGLE_REG_DATA, defaultAddr, 2, id));
        params.insertParam(new IssoSingleRegInfoParam(SINGLE_REG_DATA, defaultAddr,
                                                      IssoHoldReg::MS_ADDR, id));
        // сформировать запрос
        IssoModuleRequest changeAddrRequest =
                IssoModuleRequest(ip, port, nextPackNum(), CMD_MS_SINGLE_HOLD_REG_SET, params);
        //
        // запрос сохранения данных мультидатчика в ПЗУ:
        //
        params.clearParams();
        // добавить параметр записи регистра
//        params.insertParam(new IssoSingleRegInfoParam(SINGLE_REG_DATA, id, 22, 2));
        params.insertParam(new IssoSingleRegInfoParam(SINGLE_REG_DATA, id,
                                                      IssoHoldReg::ROM_CMD,
                                                      IssoRomCmdValue::WRITE_IDENT));
        // сформировать запрос
        IssoModuleRequest saveRequest =
                IssoModuleRequest(ip, port, nextPackNum(), CMD_MS_SINGLE_HOLD_REG_SET, params);
        //
        // запрос воспроизведения сигнала бипера:
        //
        params.clearParams();
        // добавить параметр записи регистра
//        params.insertParam(new IssoSingleRegInfoParam(SINGLE_REG_DATA, id, 25, 0));
        params.insertParam(new IssoSingleRegInfoParam(SINGLE_REG_DATA, id,
                                                      IssoHoldReg::MS_ACTION,
                                                      IssoMsActionValue::SHORT_BEEP));
        // сформировать запрос
        IssoModuleRequest beepRequest =
                IssoModuleRequest(ip, port, nextPackNum(), CMD_MS_SINGLE_HOLD_REG_SET, params);
        // добавить запросы в список
        requests << existActivatedRequest;
        requests << existNonactivatedRequest;
        requests << changeAddrRequest;
        requests << saveRequest;
        requests << beepRequest;
    }
    //
    // вывести диалог прогресса операции
    showActivationProgressDialog(msIds.size() + 1);
//    showActivationProgressDialog(multiSensorIds.size());
    //
    // запустить активацию
    m_activationRunning = true;
    // добавить запросы в очередь для обработки
    addToActivationQueue(requests);
}



void IssoExchangeManager::processExchangeLoopFinished()
{
//    qDebug() << "Exchanging loop finished";
}


quint16 IssoExchangeManager::nextPackNum()
{
    // заблокировать доступ
    m_packNumLock->lock();
    // инкрементировать номер пакета
    quint16 num = ++m_packNum;
    // разблокировать доступ
    m_packNumLock->unlock();
    return num;
}


IssoModuleRequest IssoExchangeManager::makeRelayRequest(int moduleId,
                                                        IssoParamId relayId,
                                                        bool state, bool &ok)
{
    QHostAddress ip;
    quint16 port;
    // получить ip и порт целевого модуля
    if (!findModuleAddressById(moduleId, ip, port))
    {
        // неуспех
        ok = false;
        return IssoModuleRequest();
    }
    // создать буфер параметров запроса
    IssoParamBuffer params;
    // подготовить значение параметра
    IssoState paramState = state ? RELAY_ON : RELAY_OFF;
    // добавить параметр, определяющий целевое реле
    params.insertParam(new IssoSensorStateParam(relayId, paramState));
    // успех
    ok = true;
    // сформировать запрос
    IssoModuleRequest request(ip, port, nextPackNum(), CMD_RELAY, params);
    return request;
}


void IssoExchangeManager::requestStateAsync(int moduleId, IssoCmdId cmdId)
{
    QHostAddress ip;
    quint16 port;
    // получить ip и порт целевого модуля
    if (!findModuleAddressById(moduleId, ip, port))
        return;

    // сформировать запрос
    IssoModuleRequest request(ip, port, nextPackNum(), cmdId);
    // асинхронно добавить запрос в очередь
    addToUserQueueAsync(request);

    // !!!
    // Необходимо добавить команду enqueueRequestAsync,
    // принимающую список запросов, чтобы гарантировать
    // порядок их отправки
    // !!!
}



void IssoExchangeManager::turnRelay(int moduleId, IssoParamId relayId, bool state)
{
//    QHostAddress ip;
//    quint16 port;
//    // получить ip и порт целевого модуля
//    if (!findModuleAddressById(moduleId, ip, port))
//        return;

//    // создать буфер параметров запроса
//    IssoParamBuffer params;
//    // подготовить значение параметра
//    IssoState paramState = state ? RELAY_ON : RELAY_OFF;
//    // добавить параметр, определяющий целевое реле
//    params.insertParam(new IssoSensorStateParam(relayId, paramState));
//    // сформировать запрос
//    IssoModuleRequest request(ip, port, nextPackNum(), CMD_RELAY, params);

    bool ok;
    IssoModuleRequest request = makeRelayRequest(moduleId, relayId, state, ok);
    // добавить запрос в очередь
    if (ok)
        addToUserQueueAsync(request);
}


void IssoExchangeManager::turnRelayAsync(int moduleId, IssoParamId relayId, bool state)
{
    QtConcurrent::run(this, &IssoExchangeManager::turnRelay, moduleId, relayId, state);
}


void IssoExchangeManager::turnRelayBroadcast(IssoParamId relayId, bool state)
{
    turnRelay(MODULE_ID_BROADCAST, relayId, state);
}


void IssoExchangeManager::turnRelayBroadcastAsync(IssoParamId relayId, bool state)
{
    QtConcurrent::run(this, &IssoExchangeManager::turnRelayBroadcast, relayId, state);
}


void IssoExchangeManager::resetMsState(int moduleId, quint8 multiSensorId)
{
//    writeMsHoldRegisterSingle(moduleId, multiSensorId, IssoHoldReg::MS_STATE, 0);
    writeMsHoldRegisterSingle(moduleId, multiSensorId,
                              IssoHoldReg::MS_STATE, IssoMsStateValue::MS_STATE_NORMAL);
}
//void IssoExchangeManager::resetMultiSensorState(int moduleId, quint8 multiSensorId)
//{
//    QHostAddress ip;
//    quint16 port;
//    // получить ip и порт целевого модуля
//    if (!findModuleAddressById(moduleId, ip, port))
//        return;
//    // список запросов
//    QList<IssoModuleRequest> requests;
//    //
//    // команда 1: сброс состояния мультидатчика
//    //
//    // создать буфер параметров запроса
//    IssoParamBuffer params;
//    // добавить параметр записи регистра
//    params.insertParam(new IssoSingleRegInfoParam(SINGLE_REG_DATA, multiSensorId, 24, 0));
//    // сформировать и добавить запрос
//    requests << IssoModuleRequest(ip, port, nextPackNum(), CMD_MS_SINGLE_HOLD_REG_SET, params);
//    //
//    // команда 2: запрос общего состояния мультидатчика
//    //
//    // сформировать и добавить запрос
//    requests << IssoModuleRequest(ip, port, nextPackNum(), CMD_MS_INFO);
//    //
//    // асинхронно добавить запросы в очередь
//    addToUserQueueAsync(requests);
//}

void IssoExchangeManager::writeMsThresholds(int moduleId, quint8 multiSensorId)
{
    // получить конфиг мультидатчика
//    auto msCfg = findMultiSensorCfg(moduleId, multiSensorId);
    auto msCfg = (m_object ? m_object->findMultiSensorCfgByModuleId(moduleId, multiSensorId)
                           : nullptr);
    if (!msCfg)
        return;
    // получить массив данных конфига мультидатчика
    QByteArray regData = msCfg->getThresholdData();
    // записать настройки мультидатчика в контроллер
    writeMsHoldRegisterBlock(moduleId, multiSensorId,
                             IssoHoldReg::SENSOR_MASK, regData,
                             IssoRomCmdValue::WRITE_THRES);
}
//void IssoExchangeManager::writeMultiSensorConfig(int moduleId, quint8 multiSensorId)
//{
//    QHostAddress ip;
//    quint16 port;
//    // получить ip и порт целевого модуля
//    if (!findModuleAddressById(moduleId, ip, port))
//        return;
//    // получить конфиг мультидатчика
//    auto msCfg = findMultiSensorCfg(moduleId, multiSensorId);
//    if (!msCfg)
//        return;
//    // получить массив данных конфига мультидатчика
//    QByteArray regData = msCfg->getRegisterData();
//    // создать буфер параметров запроса
//    IssoParamBuffer params;
//    // добавить параметр записи блока регистров
//    // (включение встроенных датчиков, настройка их порогов и прочих характеристик)
//    params.insertParam(new IssoBlockRegInfoParam(BLOCK_REG_DATA, multiSensorId, 3, regData));
//    // сформировать запрос
//    IssoModuleRequest request(ip, port, nextPackNum(), CMD_MS_BLOCK_HOLD_REG_SET, params);
//    //
//    // асинхронно добавить запрос в очередь
//    addToUserQueueAsync(request);
//}


void IssoExchangeManager::readMsThresholds(int moduleId, quint8 multiSensorId)
{
    // считать настройки мультидатчика из контроллера
    readMsHoldRegisterBlock(moduleId, multiSensorId,
                            IssoHoldReg::SENSOR_MASK,
                            IssoConst::THRESHOLD_REG_COUNT);
}


void IssoExchangeManager::shortBeep(int moduleId, quint8 multiSensorId)
{
    writeMsHoldRegisterSingle(moduleId, multiSensorId,
                              IssoHoldReg::MS_ACTION,
                              IssoMsActionValue::SHORT_BEEP);
}


void IssoExchangeManager::startCalibration(int moduleId, quint8 multiSensorId)
{
    writeMsHoldRegisterSingle(moduleId, multiSensorId,
                              IssoHoldReg::MS_ACTION,
                              IssoMsActionValue::START_CALIBRATION);
}


void IssoExchangeManager::turnRelaysByScenario(IssoScenarioInfo scenarioInfo,
                                               const QList<QPair<int, IssoParamId> > &moduleToRelayPairs,
                                               bool state)
{
    // сформировать список запросов сценария
    QList<IssoScenarioRequest> scenarioRequests;
    // для каждой пары [moduleId : relayId]
    foreach (auto pair, moduleToRelayPairs)
    {
//        bool ok;
        QHostAddress ip;
        quint16 port;
        // получить ip и порт целевого модуля
        if (!findModuleAddressById(pair.first, ip, port))
            return;
        //
        // создать буфер параметров запроса
        IssoParamBuffer params;
        // подготовить значение параметра
        IssoState paramState = state ? RELAY_ON : RELAY_OFF;
        // добавить параметр, определяющий целевое реле
        params.insertParam(new IssoSensorStateParam(pair.second, paramState));
        // сформировать запрос
        IssoModuleRequest request(ip, port, nextPackNum(), CMD_RELAY, params);
        // создать и добавить в список запрос сценария
        scenarioRequests << IssoScenarioRequest(scenarioInfo, request);
    }
    // если список не пуст, добавить в очередь запросов сценария
    m_scenarioRequests.append(scenarioRequests);

    //
    qDebug() << "Relay request appended" << endl;
}


void IssoExchangeManager::addTerminalRequest(IssoScenarioInfo scenarioInfo)
{
    m_scenarioRequests.append(IssoScenarioRequest(scenarioInfo, true));

    //
    qDebug() << "Terminal request appended" << endl;
}


void IssoExchangeManager::stopScenario(const QString &scenarioName)
{
    if (!m_scenarioRequests.isEmpty())
        m_scenarioRequests.removeAll(scenarioName);
}


//IssoArrayParam *IssoExchangeManager::makeAnalogThresholdsParam(IssoParamId analogParamId,
//                                                              IssoAnalogChainConfig *analogCfg)
//{
//    // получить ID параметра пороговых значений
//    // по ID параметра датчика / шлейфа
//    IssoParamId thresholdParamId = m_analogThresholdIds.value(analogParamId, PARAM_UNDEFINED);
//    // если соответствие не найдено, вернуть пустой указатель
//    if (thresholdParamId == PARAM_UNDEFINED)
//        return nullptr;
//    //
//    QByteArray arr;
//    // статус подключения
//    arr.append(analogCfg->enabled() ? 1 : 0);
//    // пороги состояния Норма
//    arr.append((quint8)(analogCfg->stateRange(STATE_NORMAL).min() * 10));
//    arr.append((quint8)(analogCfg->stateRange(STATE_NORMAL).max() * 10));
//    // пороги состояния Внимание
//    arr.append((quint8)(analogCfg->stateRange(STATE_WARNING).min() * 10));
//    arr.append((quint8)(analogCfg->stateRange(STATE_WARNING).max() * 10));
//    // пороги состояния Тревога
//    arr.append((quint8)(analogCfg->stateRange(STATE_ALARM).min() * 10));
//    arr.append((quint8)(analogCfg->stateRange(STATE_ALARM).max() * 10));
//    // если id параметра определяет аналоговый шлейф, то добавить КЗ и Обрыв
//    if (IssoCommonData::paramIdDefinesAnalogChainThresholds(thresholdParamId))
//    {
//        // пороги состояния КЗ
//        arr.append((quint8)(analogCfg->stateRange(STATE_SHORT_CIRCUIT).min() * 10));
//        arr.append((quint8)(analogCfg->stateRange(STATE_SHORT_CIRCUIT).max() * 10));
//        // пороги состояния Обрыв
//        arr.append((quint8)(analogCfg->stateRange(STATE_BREAK).min() * 10));
//        arr.append((quint8)(analogCfg->stateRange(STATE_BREAK).max() * 10));
//    }
//    return new IssoArrayParam(thresholdParamId, arr);
//}


//void IssoExchangeManager::initThresholdParamIds()
//{
//    m_analogThresholdIds.insert(BTN_FIRE, THRESHOLD_BTN_FIRE);
//    m_analogThresholdIds.insert(BTN_SECURITY, THRESHOLD_BTN_SECURITY);
//    m_analogThresholdIds.insert(SENSOR_SMOKE, THRESHOLD_SMOKE);
//    m_analogThresholdIds.insert(SENSOR_CO, THRESHOLD_CO);
//    m_analogThresholdIds.insert(VOLT_BATTERY, THRESHOLD_VOLT_BATTERY);
//    m_analogThresholdIds.insert(VOLT_BUS, THRESHOLD_VOLT_BUS);
//    m_analogThresholdIds.insert(VOLT_SENSORS, THRESHOLD_VOLT_SENSORS);
//    m_analogThresholdIds.insert(SUPPLY_EXTERNAL, THRESHOLD_SUPPLY_EXTERNAL);
//    m_analogThresholdIds.insert(SUPPLY_INTERNAL, THRESHOLD_SUPPLY_INTERNAL);
//    m_analogThresholdIds.insert(TEMP_OUT, THRESHOLD_TEMP);
//    m_analogThresholdIds.insert(VIBRATION, THRESHOLD_VIBRATION);
//    m_analogThresholdIds.insert(SENSOR_OPEN, THRESHOLD_SENSOR_OPEN);
//    m_analogThresholdIds.insert(ANALOG_CHAIN_1, THRESHOLD_ANALOG_CHAIN_1);
//    m_analogThresholdIds.insert(ANALOG_CHAIN_2, THRESHOLD_ANALOG_CHAIN_2);
//    m_analogThresholdIds.insert(ANALOG_CHAIN_3, THRESHOLD_ANALOG_CHAIN_3);
//    m_analogThresholdIds.insert(ANALOG_CHAIN_4, THRESHOLD_ANALOG_CHAIN_4);
//    m_analogThresholdIds.insert(ANALOG_CHAIN_5, THRESHOLD_ANALOG_CHAIN_5);
//    m_analogThresholdIds.insert(ANALOG_CHAIN_6, THRESHOLD_ANALOG_CHAIN_6);
//}


//void IssoExchangeManager::setupSensorsThresholds(const QHostAddress &ip, quint16 port,
//                                                 IssoModuleConfig *moduleCfg)
//{
//    // создать буфер параметров запроса
//    IssoParamBuffer params;
//    // заполнить буфер параметров
//    foreach (IssoParamId sensorParamId, moduleCfg->sensorConfigs().keys())
//    {
//        // получить конфиг датчика
//        IssoAnalogChainConfig* sensorCfg = moduleCfg->sensorConfig(sensorParamId);
//        if (!sensorCfg)
//            continue;

//        // сформировать параметр пороговых значений
//        IssoArrayParam* sensorParam = makeAnalogThresholdsParam(sensorParamId, sensorCfg);
//        // добавить параметр в буфер параметров
//        if (sensorParam)
//            params.insertParam(sensorParam);
//    }
//    // сформировать запрос
//    IssoModuleRequest request(ip, port, nextPackNum(), CMD_THRESHOLD_SENSORS, params);
//    // асинхронно добавить запрос в очередь
//    addToUserQueueAsync(request);
//}


//void IssoExchangeManager::setupAnalogChainsThresholds(const QHostAddress &ip, quint16 port,
//                                                      IssoModuleConfig *moduleCfg)
//{
//    if (!m_object)
//        return;
//    // создать буфер параметров запроса
//    IssoParamBuffer params;
//    // заполнить буфер параметров
//    foreach (IssoParamId chainParamId, moduleCfg->chainConfigLinks().keys())
//    {
//        // получить ссылку на конфиг шлейфа
//        IssoChainConfigLink link = moduleCfg->chainConfigLink(chainParamId);
//        // получить конфиг шлейфа по имени
//        IssoChainConfig* chainCfg = m_object->chainConfig(link.chainCfgName());
//        // если конфиг шлейфа не найден, либо шлейф не аналоговый
//        if (!chainCfg || (chainCfg->type() != SMOKE_CHAIN))
//            continue;
//        // привести к типу аналогового шлейфа
//        IssoAnalogChainConfig* analogCfg = dynamic_cast<IssoAnalogChainConfig*>(chainCfg);
//        if (!analogCfg)
//            continue;

//        // сформировать параметр пороговых значений
//        IssoArrayParam* analogChainParam = makeAnalogThresholdsParam(chainParamId, analogCfg);
//        // добавить параметр в буфер параметров
//        if (analogChainParam)
//            params.insertParam(analogChainParam);
//    }
//    // сформировать запрос
//    IssoModuleRequest request(ip, port, nextPackNum(), CMD_THRESHOLD_ANALOG, params);
//    // асинхронно добавить запрос в очередь
//    addToUserQueueAsync(request);
//}

/**
 * @brief IssoExchangeManager::setupMultiSensorPollingAddrs сформировать очередь MS для опроса
 * @param module
 */
void IssoExchangeManager::setupMultiSensorPollingAddrs(IssoModule *module)
{
    if (!module)
        return;
    // получить список мультидатчиков модуля
    QByteArray multiSensorIds = moduleMultiSensorAddrs(module);
    // сформировать запрос установки адресов мультидатчика для опроса
    auto request = makeMultiSensorPollingAddrsRequest(module->ip(), module->port(), multiSensorIds);
    // асинхронно добавить запрос в очередь
    addToUserQueueAsync(request);
}


IssoModuleRequest IssoExchangeManager::makeMultiSensorPollingAddrsRequest(const QHostAddress &ip, quint16 port,
                                                                          const QByteArray &multiSensorIds)
{
    // создать буфер параметров запроса
    IssoParamBuffer params;
    // добавить параметр адреса мультидатчика
    params.insertParam(new IssoSizedArrayParam(MULTISENSOR_ADDRS, multiSensorIds));
    // сформировать запрос
    return IssoModuleRequest(ip, port, nextPackNum(), CMD_MS_POLL_ADDRS, params);
}

/**
 * @brief IssoExchangeManager::moduleMultiSensorAddrs получить конфиг адресного шлейфа
 * @param module
 * @return
 */
QByteArray IssoExchangeManager::moduleMultiSensorAddrs(IssoModule *module)
{
    if (!m_object || !module)
        return QByteArray();

    // получить конфиг адресного шлейфа
    auto msChainCfg = m_object->moduleAddressChainConfig(module->moduleConfigName());
    // вернуть массив ID мультидатчиков конфига адресного шлейфа,
    // либо пустой массив, если конфиг не найден
    return (msChainCfg ? msChainCfg->multiSensorIdArray() : QByteArray());

//    // получить конфиг адресного шлейфа, содержащегося в конфиге модуля
//    IssoChainConfig* chainCfg =
//            m_object->moduleChainConfig(module->getModuleConfigName(), MULTISENSOR);
//    // привести к типу адресного шлейфа
//    IssoDigitalChainConfig* digitalCfg = dynamic_cast<IssoDigitalChainConfig*>(chainCfg);
//    // вернуть массив ID мультидатчиков конфига адресного шлейфа,
//    // либо пустой массив, если конфиг не найден
//    return (digitalCfg ? digitalCfg->multiSensorIds() : QByteArray());
}


void IssoExchangeManager::showActivationProgressDialog(int stepCount)
{
    // создать диалог прогресса операции
    QProgressDialog* dlg = IssoCommonData::makeProgressDialog(
                                tr("Активация мультидатчиков"),
                                tr("Подготовка к активации..."),
                                0, stepCount, qApp->activeWindow());
    // настроить доступ к диалогу из другого потока
    connect(this, SIGNAL(progressCurrent(int)),
            dlg,  SLOT(setValue(int)));
    connect(this, SIGNAL(progressInfo(QString)),
            dlg,  SLOT(setLabelText(QString)));
    connect(this, SIGNAL(progressFinished()),
            dlg,  SLOT(close()));
    connect(dlg,  SIGNAL(destroyed(QObject*)),
            this, SLOT(stopMsActivation()));
    // вывести диалог прогресса
    dlg->show();
}


void IssoExchangeManager::startExchanging(bool polling)
{
    // включить прием тревог
    m_alarmReceiver->startReceiving();
    // обработчики завершения цикла обмена командами
    m_watcher = new QFutureWatcher<void>();
    // запустить цикл обмена командами в отдельном потоке
    QFuture<void> loopFuture;
    if (polling)
        // запустить цикл с опросом
        loopFuture = QtConcurrent::run(this, &IssoExchangeManager::runExchangeLoopWithPolling);
    else
        // запустить цикл без опроса
        loopFuture = QtConcurrent::run(this, &IssoExchangeManager::runExchangeLoop);
    // установить наблюдателя за потоком
    m_watcher->setFuture(loopFuture);
}


void IssoExchangeManager::stopExchanging()
{
    // отключить прием тревог
    m_alarmReceiver->stopReceiving();
    // завершить опрос
    m_running = false;
    // если наблюдатель был установлен
    if (m_watcher)
    {
        // если задача запущена, ожидать завершения
        if (m_watcher->isRunning())
            m_watcher->future().waitForFinished();
        // уничтожить наблюдателя и обнулить указатель
        delete m_watcher;
        m_watcher = nullptr;
    }
}

void IssoExchangeManager::readDescriptor(const int moduleId)
{
    requestStateAsync(moduleId, CMD_DESC_GET);
}


void IssoExchangeManager::writeDescriptor(const int moduleId,
                                          const IssoModuleDescriptor &desc)
{
    QHostAddress ip;
    quint16 port;
    // получить ip и порт целевого модуля
    if (!findModuleAddressById(moduleId, ip, port))
        return;
    // создать буфер параметров запроса
    IssoParamBuffer params;
    // заполнить буфер параметров:
    // IP-адрес
    params.insertParam(new IssoIpParam(DESC_IP, desc.ip()));
    // МАС
    params.insertParam(new IssoArrayParam(DESC_MAC, desc.mac()));
    // порт
    params.insertParam(new IssoShortParam(DESC_PORT, desc.port()));
    // IP-адрес сервера
    params.insertParam(new IssoIpParam(DESC_SERVER_IP, desc.serverIp()));
    // порт сервера
    params.insertParam(new IssoShortParam(DESC_SERVER_PORT, desc.serverPort()));
    // номер телефона
    params.insertParam(new IssoArrayParam(DESC_PHONE, desc.phone()));
    // разрешение использовать телефон
    params.insertParam(new IssoBoolParam(DESC_PHONE_ENABLED, desc.phoneEnabled()));
    // CRC
    params.insertParam(new IssoByteParam(DESC_CRC, desc.crc()));
    //
    // сформировать запрос
    IssoModuleRequest request(ip, port, nextPackNum(), CMD_DESC_SET, params);
    // асинхронно добавить запрос в очередь
    addToUserQueueAsync(request);
}


void IssoExchangeManager::runExchangeLoop()
{
    // обменник
    IssoRequestSender sender(m_ip, m_bindPort);
    // обработчики
    connect(&sender, SIGNAL(requestSent(IssoModuleRequest)),
           this,     SIGNAL(requestSent(IssoModuleRequest)));
    connect(&sender, SIGNAL(dataReceived(const QHostAddress&, quint16, const QByteArray&)),
            this,    SIGNAL(replyDataReceived(QHostAddress,quint16,QByteArray)));
    //
    // цикл обмена командами
    m_running = true;
    while (m_running)
    {
//        if (!testHandleActivationRequests(sender))
//            return;
        // обработать очередь запросов активации
        if (!handleActivationRequests(sender))
            return;
        // обработать очередь запросов пользователя
        if (!handleUserRequests(sender))
            return;
        // обработать очередь запросов сценариев
        if (!handleScenarioRequests(sender))
            return;
    }
}


void IssoExchangeManager::runExchangeLoopWithPolling()
{
    if (!m_object)
        return;
    // обменник
    IssoRequestSender sender(m_ip, m_bindPort);
    // обработчики
    connect(&sender, SIGNAL(requestSent(IssoModuleRequest)),
           this,     SIGNAL(requestSent(IssoModuleRequest)));
    connect(&sender, SIGNAL(dataReceived(const QHostAddress&, quint16, const QByteArray&)),
            this,    SIGNAL(replyDataReceived(QHostAddress,quint16,QByteArray)));

    // список модулей
    auto modules = m_object->locatedModules();
    // цикл обмена командами
    bool ok = false;
    m_running = true;
    while (m_running)
    {
        // опросить каждый модуль из списка
        foreach (IssoModule* module, modules)
        {
            // проверить активность
            if (!m_running)
                break;
            // обработать очередь запросов пользователя
            if (!handleUserRequests(sender))
                return;
            // обработать очередь запросов сценариев
            if (!handleScenarioRequests(sender))
                return;
            //
            //
            // сформировать запрос состояний встроенных датчиков модуля
            IssoModuleRequest request1(module->ip(), module->port(),
                                      nextPackNum(), CMD_INFO);
            // отправить запрос, дождаться ответа
//            sender.sendRequestRepeatedly(request1, ok, IssoCommonData::SEND_TRY_COUNT);
            auto replyPtr = sendRequest(sender, request1, ok);

            //
            // сформировать запрос состояний мультидатчиков
            IssoModuleRequest request2(module->ip(), module->port(),
                                      nextPackNum(), CMD_MS_INFO);
            // отправить запрос, дождаться ответа
//            sender.sendRequestRepeatedly(request2, ok, IssoCommonData::SEND_TRY_COUNT);
            replyPtr = sendRequest(sender, request2, ok);
            //
            // проверить активность
            if (!m_running)
                return;
            // пауза
            QThread::currentThread()->msleep(IssoConst::POLL_STATE_INTERVAL);
        }
    }
}
//void IssoExchangeManager::runExchangeLoopWithPolling()
//{
//    // обменник
//    IssoRequestSender sender(m_ip, m_bindPort);
//    // обработчики
//    connect(&sender, SIGNAL(requestSent(IssoModuleRequest)),
//           this,     SIGNAL(requestSent(IssoModuleRequest)));
//    connect(&sender, SIGNAL(dataReceived(const QHostAddress&, quint16, const QByteArray&)),
//            this,    SIGNAL(replyDataReceived(QHostAddress,quint16,QByteArray)));

//    //
//    // цикл обмена командами
//    bool ok = false;
//    m_running = true;
//    while (m_running)
//    {
//        // опросить каждый модуль из списка
//        foreach (IssoModule* module, m_modules)
//        {
//            // проверить активность
//            if (!m_running)
//                break;
//            // обработать очередь запросов пользователя
//            if (!handleUserRequests(sender))
//                return;
//            // обработать очередь запросов сценариев
//            if (!handleScenarioRequests(sender))
//                return;
//            //
//            //
//            // сформировать запрос состояний встроенных датчиков модуля
//            IssoModuleRequest request1(module->ip(), module->port(),
//                                      nextPackNum(), CMD_INFO);
//            // отправить запрос, дождаться ответа
////            sender.sendRequestRepeatedly(request1, ok, IssoCommonData::SEND_TRY_COUNT);
//            auto replyPtr = sendRequest(sender, request1, ok);

//            //
//            // сформировать запрос состояний мультидатчиков
//            IssoModuleRequest request2(module->ip(), module->port(),
//                                      nextPackNum(), CMD_MS_INFO);
//            // отправить запрос, дождаться ответа
////            sender.sendRequestRepeatedly(request2, ok, IssoCommonData::SEND_TRY_COUNT);
//            replyPtr = sendRequest(sender, request2, ok);
//            //
//            // проверить активность
//            if (!m_running)
//                return;
//            // пауза
//            QThread::currentThread()->msleep(IssoConst::POLL_STATE_INTERVAL);
//        }
//    }
//}

// cha
/* функция для проверки активности подключенных датчиков
 * нужно убрать всё что связано с их активацией
 */
void IssoExchangeManager::checkMsValid(int moduleId, const QList<quint8> &msIds)
{
     // const QList<quint8> &msIds - нужно убрать
     QHostAddress ip;
     quint16 port;
     // получить ip и порт целевого модуля
     if (!findModuleAddressById(moduleId, ip, port)) return;
    // заводской адрес мультидатчика
    //    quint8 defaultAddr = 247;
        quint8 defaultAddr = IssoConst::DEFAULT_MS_ADDR;
        // список запросов
        QList<IssoModuleRequest> requests;
        // запрос установки адресов мультидатчиков для опроса модулем:
        QByteArray idArray;
        foreach (quint8 id, msIds)
        {
            idArray.append(id);
        }
        requests << makeMultiSensorPollingAddrsRequest(ip, port, idArray);
        //
        // для каждого мультидатчика добавить запрос его существования в сети
        foreach (quint8 id, msIds)
        {
            IssoParamBuffer params;
            //
            // запрос существования активированного датчика (со следующим по порядку адресом):
            // добавить параметр адреса мультидатчика
            params.insertParam(new IssoByteParam(MULTISENSOR_ADDR, id));
            // сформировать запрос
//
            IssoModuleRequest existActivatedRequest =
                    IssoModuleRequest(ip, port, nextPackNum(), CMD_MULTISENSOR_EXIST_GET, params);
            //
            // запрос существования неактивированного датчика (с заводским адресом = 247):
            //
            // добавить параметр адреса мультидатчика
            params.insertParam(new IssoByteParam(MULTISENSOR_ADDR, defaultAddr));
            // сформировать запрос
            IssoModuleRequest existNonactivatedRequest =
                    IssoModuleRequest(ip, port, nextPackNum(), CMD_MULTISENSOR_EXIST_GET, params);
            //
            // запрос изменения адреса мультидатчика:
            //
            params.clearParams();
            // добавить параметр записи регистра
    //        params.insertParam(new IssoSingleRegInfoParam(SINGLE_REG_DATA, defaultAddr, 2, id));
            params.insertParam(new IssoSingleRegInfoParam(SINGLE_REG_DATA, defaultAddr,
                                                          IssoHoldReg::MS_ADDR, id));
            // сформировать запрос
            IssoModuleRequest changeAddrRequest =
                    IssoModuleRequest(ip, port, nextPackNum(), CMD_MS_SINGLE_HOLD_REG_SET, params);
            //
            // запрос сохранения данных мультидатчика в ПЗУ:
            //
            params.clearParams();
            // добавить параметр записи регистра
    //        params.insertParam(new IssoSingleRegInfoParam(SINGLE_REG_DATA, id, 22, 2));
            params.insertParam(new IssoSingleRegInfoParam(SINGLE_REG_DATA, id,
                                                          IssoHoldReg::ROM_CMD,
                                                          IssoRomCmdValue::WRITE_IDENT));
            // сформировать запрос
            IssoModuleRequest saveRequest =
                    IssoModuleRequest(ip, port, nextPackNum(), CMD_MS_SINGLE_HOLD_REG_SET, params);
            //
            // запрос воспроизведения сигнала бипера:
            //
            params.clearParams();
            // добавить параметр записи регистра
    //        params.insertParam(new IssoSingleRegInfoParam(SINGLE_REG_DATA, id, 25, 0));
            params.insertParam(new IssoSingleRegInfoParam(SINGLE_REG_DATA, id,
                                                          IssoHoldReg::MS_ACTION,
                                                          IssoMsActionValue::SHORT_BEEP));
            // сформировать запрос
            IssoModuleRequest beepRequest =
                    IssoModuleRequest(ip, port, nextPackNum(), CMD_MS_SINGLE_HOLD_REG_SET, params);
            // добавить запросы в список
            requests << existActivatedRequest;
            requests << existNonactivatedRequest;
            requests << changeAddrRequest;
            requests << saveRequest;
            requests << beepRequest;
        }
        //
        // вывести диалог прогресса операции
        showActivationProgressDialog(msIds.size() + 1);
    //    showActivationProgressDialog(multiSensorIds.size());
        //
        // запустить активацию
        m_activationRunning = true;
        // добавить запросы в очередь для обработки
        addToActivationQueue(requests);
}
