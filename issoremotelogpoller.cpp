#include "issoremotelogpoller.h"

/**
 * @brief IssoRemoteLogPoller::IssoRemoteLogPoller обработчик удаленного журнала событий
 * @param appType
 * @param parent
 */
IssoRemoteLogPoller::IssoRemoteLogPoller(IssoAppType appType, QObject *parent)
    : QObject(parent),
      m_appType(appType)
{
    // задать адрес веб-сервера
    m_remoteLogger.setUrl(IssoConst::LOG_SERVER_URL);
    // задать обработчики событий
    setEventHandlers();

    m_lastEventId = m_settings.value("lastEventId").toInt();
    qDebug() << "m_lastEventId = " << m_lastEventId;
}


IssoRemoteLogPoller::~IssoRemoteLogPoller()
{
    m_settings.setValue("lastEventId", m_lastEventId);
    qDebug() << "~IssoRemoteLogPoller" << endl;
}


void IssoRemoteLogPoller::start(int msec)
{
    // запросить обновления
    requestLogUpdates();
    // запустить таймер запроса обновлений
    m_timer.start(msec);
}


void IssoRemoteLogPoller::stop()
{
    m_timer.stop();
}


IssoAppType IssoRemoteLogPoller::appType() const
{
    return m_appType;
}

void IssoRemoteLogPoller::setAppType(const IssoAppType &appType)
{
    m_appType = appType;
}

void IssoRemoteLogPoller::setEventHandlers()
{
    connect(&m_remoteLogger, SIGNAL(requestCompleted(QString)),
            this,            SLOT(processLogRequestCompleted(QString)));
    connect(&m_remoteLogger, SIGNAL(requestFailed(QString)),
            this,            SLOT(processLogRequestFailed(QString)));
    connect(&m_timer,   SIGNAL(timeout()),
            this,       SLOT(requestLogUpdates()));
}


QList<IssoEventData> IssoRemoteLogPoller::parseLogReply(const QString &reply)
{
    QList<IssoEventData> events;
    //
    // распарсить документ JSON
    QJsonDocument doc = QJsonDocument::fromJson(reply.toUtf8());
    if (doc.isNull() || !doc.isArray())
        return events;
    // получить массив JSON
    QJsonArray jsonArray = doc.array();
    // обработать каждое JSON-сообщение
    foreach (auto jsonValue, jsonArray)
    {
        if (jsonValue.isObject())
        {
            // преобразовать JSON-сообщение в структуру
            events << parseEventObject(jsonValue.toObject());
        }
    }
    // отсортировать список по возрастанию ID события
    std::sort(events.begin(), events.end(),
              [](const IssoEventData& a, const IssoEventData& b) ->
                    bool { return a.id < b.id; });
    return events;
}


//QList<IssoEventData> IssoRemoteLogPoller::parseEventObject(const QJsonObject &eventObj)
//{
//    QList<IssoEventData> events;
//    IssoEventData pingEvent;
//    //
//    // сформировать структуру сообщения о событии из строки:
//    //
//    // тип события
//    if (eventObj.contains("eventType") && eventObj["eventType"].isDouble())
//    {
//        // получить тип события
//        pingEvent.eventType = static_cast<IssoEventType>(eventObj["eventType"].toInt());
//    }
//    // дата и время
//    if (eventObj.contains("dateTime") && eventObj["dateTime"].isString())
//    {
//        // получить строку из JSON
//        QString dateTimeStr = eventObj["dateTime"].toString();
//        // преобразовать в дату / время
//        pingEvent.dateTime = QDateTime::fromString(dateTimeStr, "dd.MM.yyyy HH:mm:ss");
//    }
//    // ID события
//    if (eventObj.contains("id") && eventObj["id"].isDouble())
//    {
//        // получить ID события
//        pingEvent.id = eventObj["id"].toInt();
//    }
//    // данные объекта
//    if (eventObj.contains("object") && eventObj["object"].isObject())
//    {
//        // получить объект из JSON
//        QJsonObject obj = eventObj["object"].toObject();
//        // проверить наличие поля "ID объекта"
//        if (obj.contains("objectId") && obj["objectId"].isDouble())
//        {
//            // получить ID объекта
//            pingEvent.objectId = obj["objectId"].toInt();
//        }
//        // проверить наличие поля "состояние объекта"
//        if (obj.contains("objectState") && obj["objectState"].isString())
//        {
//            // получить состояние объекта
//            pingEvent.objectState = obj["objectState"].toString();
//        }
//        // сводные данные:
//        // общее число модулей
//        if (obj.contains("totalModules") && obj["totalModules"].isDouble())
//        {
//            pingEvent.totalModules = obj["totalModules"].toInt();
//        }
//        // общее число мультидатчиков
//        if (obj.contains("totalMultisensors") && obj["totalMultisensors"].isDouble())
//        {
//            pingEvent.totalMs = obj["totalMultisensors"].toInt();
//        }
//        // общее число каналов
//        if (obj.contains("totalChannels") && obj["totalChannels"].isDouble())
//        {
//            pingEvent.totalChannels = obj["totalChannels"].toInt();
//        }
//        // число тревожных модулей
//        if (obj.contains("alarmModules") && obj["alarmModules"].isDouble())
//        {
//            pingEvent.alarmModules = obj["alarmModules"].toInt();
//        }
//        // число тревожных мультидатчиков
//        if (obj.contains("alarmMultisensors") && obj["alarmMultisensors"].isDouble())
//        {
//            pingEvent.alarmMs = obj["alarmMultisensors"].toInt();
//        }
//        // число тревожных каналов
//        if (obj.contains("alarmChannels") && obj["alarmChannels"].isDouble())
//        {
//            pingEvent.alarmChannels = obj["alarmChannels"].toInt();
//        }
//    }
//    //
//    // задать тип события
//    pingEvent.eventType = EVENT_PING;
//    // добавить пинг в список
//    events << pingEvent;
//    //
//    // имя модуля
//    if (eventObj.contains("module") && eventObj["module"].isObject())
//    {
//        // получить модуль из JSON
//        QJsonObject module = eventObj["module"].toObject();
//        // проверить наличие поля "имя модуля"
//        if (module.contains("moduleName") && module["moduleName"].isString())
//        {
//            // получить имя модуля
//            pingEvent.moduleName = module["moduleName"].toString();
//        }
//    }
////    //
////    // добавить пинг в список
////    events << pingEvent;
//    //
//    // если присутствуют состояния датчиков, обработать
//    if (eventObj.contains("sensors") && eventObj["sensors"].isArray())
//    {
//        // получить массив датчиков из JSON
//        QJsonArray sensors = eventObj["sensors"].toArray();
//        // обойти все датчики
//        foreach (auto sensorValue, sensors)
//        {
//            if (!sensorValue.isObject())
//                continue;
//            // получить датчик
//            QJsonObject sensorObj = sensorValue.toObject();
//            // копировать событие (нужны заполненные поля)
//            IssoEventData sensorEvent = pingEvent;
//            //
//            // проверить наличие у датчика поля "Имя датчика"
//            if (sensorObj.contains("sensorName") && sensorObj["sensorName"].isString())
//            {
//                sensorEvent.sensorName = sensorObj["sensorName"].toString();
//            }
//            // проверить наличие у датчика поля "Состояние датчика"
//            if (sensorObj.contains("sensorState") && sensorObj["sensorState"].isString())
//            {
//                sensorEvent.sensorStateName = sensorObj["sensorState"].toString();
//            }
//            // проверить наличие у датчика поля "Расширенное значение"
//            if (sensorObj.contains("extValue") && sensorObj["extValue"].isDouble())
//            {
//                sensorEvent.extValue = sensorObj["extValue"].toInt();
//            }
//            // проверить наличие у датчика поля "Расположение"
//            if (sensorObj.contains("location") && sensorObj["location"].isString())
//            {
//                sensorEvent.location = sensorObj["location"].toString();
//            }
//            //
//            // задать тип события
//            sensorEvent.eventType = EVENT_SENSORS;
//            // добавить событие датчика в список
//            events << sensorEvent;
//        }
//    }
//    //
//    // если присутствуют данные фото, обработать
//    else if (eventObj.contains("photo") && eventObj["photo"].isObject())
//    {
//        // получить датчик
//        QJsonObject photoObj = eventObj["photo"].toObject();
//        // копировать событие (нужны заполненные поля)
//        IssoEventData photoEvent = pingEvent;
//        //
//        // проверить наличие поля "Имя датчика"
//        if (photoObj.contains("sensorName") && photoObj["sensorName"].isString())
//        {
//            photoEvent.sensorName = photoObj["sensorName"].toString();
//        }
//        // проверить наличие поля "ID камеры"
//        if (photoObj.contains("cameraId") && photoObj["cameraId"].isDouble())
//        {
//            photoEvent.cameraId = photoObj["cameraId"].toInt();
//        }
//        // проверить наличие поля "IP-адрес камеры"
//        if (photoObj.contains("cameraIp") && photoObj["cameraIp"].isDouble())
//        {
//            photoEvent.cameraIp = photoObj["cameraIp"].toInt();
//        }
//        // проверить наличие поля "Данные кадра"
//        if (photoObj.contains("frameData") && photoObj["frameData"].isString())
//        {
//            photoEvent.frameData = photoObj["frameData"].toString();
//        }
//        //
//        // задать тип события
//        photoEvent.eventType = EVENT_PHOTO;
//        // добавить событие датчика в список
//        events << photoEvent;
//    }
//    // вернуть структуру
//    return events;
//}

QList<IssoEventData> IssoRemoteLogPoller::parseEventObject(const QJsonObject &eventObj)
{
    QList<IssoEventData> events;
    IssoEventData event;
    //
    // сформировать структуру сообщения о событии из строки:
    //
    // разобрать общую часть всех событий
    parseCommonInfo(eventObj, event);
    // разобрать сообщение согласно типу события
    switch(event.eventType)
    {
        case EVENT_PING:
        {
            // добавить событие в список
            events << event;
            break;
        }
        case EVENT_SENSORS:
        {
//            // добавить событие в список как ping
//            IssoEventData ping = event;
//            ping.eventType = EVENT_PING;
//            events << ping;
//            //
            // разобрать данные модуля
            parseModuleInfo(eventObj, event);
            // разобрать данные датчиков, изменивших состояния
            events << parseSensorsInfo(eventObj, event);
            break;
        }
        case EVENT_PHOTO:
        {
//            // добавить событие в список как ping
//            IssoEventData ping = event;
//            ping.eventType = EVENT_PING;
//            events << ping;
//            //
            // разобрать данные модуля
            parseModuleInfo(eventObj, event);
            // разобрать данные фотоснимка
            parsePhotoInfo(eventObj, event);
            events << event;
            break;
        }
        default:
            break;
    }
    // вернуть список событий
    return events;
}


void IssoRemoteLogPoller::parseCommonInfo(const QJsonObject &eventObj, IssoEventData &event)
{
    // тип события
    if (eventObj.contains("eventType") && eventObj["eventType"].isDouble())
    {
        // получить тип события
        event.eventType = static_cast<IssoEventType>(eventObj["eventType"].toInt());
    }
    // дата и время
    if (eventObj.contains("dateTime") && eventObj["dateTime"].isString())
    {
        // получить строку из JSON
        QString dateTimeStr = eventObj["dateTime"].toString();
        // преобразовать в дату / время
        event.dateTime = QDateTime::fromString(dateTimeStr, "dd.MM.yyyy HH:mm:ss");
    }
    // ID события
    if (eventObj.contains("id") && eventObj["id"].isDouble())
    {
        // получить ID события
        event.id = eventObj["id"].toInt();
    }
    // данные объекта
    if (eventObj.contains("object") && eventObj["object"].isObject())
    {
        // получить объект из JSON
        QJsonObject obj = eventObj["object"].toObject();
        // проверить наличие поля "ID объекта"
        if (obj.contains("objectId") && obj["objectId"].isDouble())
        {
            // получить ID объекта
            event.objectId = obj["objectId"].toInt();
        }
        // проверить наличие поля "состояние объекта"
        if (obj.contains("objectState") && obj["objectState"].isString())
        {
            // получить состояние объекта
            event.objectState = obj["objectState"].toString();
        }
        // сводные данные:
        // общее число модулей
        if (obj.contains("totalModules") && obj["totalModules"].isDouble())
        {
            event.totalModules = obj["totalModules"].toInt();
        }
        // общее число мультидатчиков
        if (obj.contains("totalMultisensors") && obj["totalMultisensors"].isDouble())
        {
            event.totalMs = obj["totalMultisensors"].toInt();
        }
        // общее число каналов
        if (obj.contains("totalChannels") && obj["totalChannels"].isDouble())
        {
            event.totalChannels = obj["totalChannels"].toInt();
        }
        // число тревожных модулей
        if (obj.contains("alarmModules") && obj["alarmModules"].isDouble())
        {
            event.alarmModules = obj["alarmModules"].toInt();
        }
        // число тревожных мультидатчиков
        if (obj.contains("alarmMultisensors") && obj["alarmMultisensors"].isDouble())
        {
            event.alarmMs = obj["alarmMultisensors"].toInt();
        }
        // число тревожных каналов
        if (obj.contains("alarmChannels") && obj["alarmChannels"].isDouble())
        {
            event.alarmChannels = obj["alarmChannels"].toInt();
        }
    }
}


void IssoRemoteLogPoller::parseModuleInfo(const QJsonObject &eventObj, IssoEventData &event)
{
    // данные модуля
    if (eventObj.contains("module") && eventObj["module"].isObject())
    {
        // получить модуль из JSON
        QJsonObject module = eventObj["module"].toObject();
        // проверить наличие поля "имя модуля"
        if (module.contains("moduleName") && module["moduleName"].isString())
        {
            // получить имя модуля
            event.moduleName = module["moduleName"].toString();
        }
    }
}

/**
 * @brief IssoRemoteLogPoller::parseSensorsInfo разбор состояний датчиков
 * @param eventObj
 * @param event
 * @return
 */

QList<IssoEventData> IssoRemoteLogPoller::parseSensorsInfo(const QJsonObject &eventObj,
                                                           const IssoEventData &event)
{
    QList<IssoEventData> events;
    // если присутствуют состояния датчиков, обработать
    if (eventObj.contains("sensors") && eventObj["sensors"].isArray())
    {
        // получить массив датчиков из JSON
        QJsonArray sensors = eventObj["sensors"].toArray();
        // обойти все датчики
        foreach (auto sensorValue, sensors)
        {
            if (!sensorValue.isObject())
                continue;
            // получить датчик
            QJsonObject sensorObj = sensorValue.toObject();
            // копировать событие (нужны заполненные поля)
            IssoEventData sensorEvent = event;
            //
            // проверить наличие у датчика поля "Имя датчика"
            if (sensorObj.contains("sensorName") && sensorObj["sensorName"].isString())
            {
                sensorEvent.sensorName = sensorObj["sensorName"].toString();
            }
            // проверить наличие у датчика поля "Состояние датчика"
            if (sensorObj.contains("sensorState") && sensorObj["sensorState"].isString())
            {
                sensorEvent.sensorStateName = sensorObj["sensorState"].toString();
            }
            // проверить наличие у датчика поля "Расширенное значение"
            if (sensorObj.contains("extValue") && sensorObj["extValue"].isDouble())
            {
                sensorEvent.extValue = sensorObj["extValue"].toInt();
            }
            // проверить наличие у датчика поля "Расположение"
            if (sensorObj.contains("location") && sensorObj["location"].isString())
            {
                sensorEvent.location = sensorObj["location"].toString();
            }
            // добавить событие датчика в список
            events << sensorEvent;
        }
    }
    return events;
}


void IssoRemoteLogPoller::parsePhotoInfo(const QJsonObject &eventObj, IssoEventData &event)
{
    // если присутствуют данные фото, обработать
    if (eventObj.contains("photo") && eventObj["photo"].isObject())
    {
        // получить датчик
        QJsonObject photoObj = eventObj["photo"].toObject();
        //
        // проверить наличие поля "Имя датчика"
        if (photoObj.contains("sensorName") && photoObj["sensorName"].isString())
        {
            event.sensorName = photoObj["sensorName"].toString();
        }
        // проверить наличие поля "ID камеры"
        if (photoObj.contains("cameraId") && photoObj["cameraId"].isDouble())
        {
            event.cameraId = photoObj["cameraId"].toInt();
        }
        // проверить наличие поля "IP-адрес камеры"
        if (photoObj.contains("cameraIp") && photoObj["cameraIp"].isDouble())
        {
            event.cameraIp = photoObj["cameraIp"].toInt();
        }
        // проверить наличие поля "Данные кадра"
        if (photoObj.contains("frameData") && photoObj["frameData"].isString())
        {
            event.frameData = photoObj["frameData"].toString();
        }
    }
}

//IssoDBEventData IssoRemoteLogPoller::parseJsonMessage(QString msg)
//{
//    IssoDBEventData rec;
//    // распарсить документ JSON
//    QJsonDocument doc = QJsonDocument::fromJson(msg.toUtf8());
//    if (doc.isNull())
//        return rec;
//    //
//    // получить объект JSON
//    QJsonObject json = doc.object();
//    //
//    // сформировать структуру сообщения о событии из строки:
//    //
//    // дата и время
//    if (json.contains("dateTime") && json["dateTime"].isString())
//    {
//        // получить строку из JSON
//        QString dateTimeStr = json["dateTime"].toString();
//        // преобразовать в дату / время
//        rec.dateTime = QDateTime::fromString(dateTimeStr, "dd.MM.yyyy HH:mm:ss");
//    }
//    // ID объекта
//    if (json.contains("object") && json["object"].isObject())
//    {
//        // получить объект из JSON
//        QJsonObject obj = json["object"].toObject();
//        // проверить наличие поля "ID объекта"
//        if (obj.contains("objectId") && obj["objectId"].isDouble())
//        {
//            // получить имя модуля
//            rec.objectId = obj["objectId"].toInt();
//        }
//        // проверить наличие поля "состояние объекта"
//        if (obj.contains("objectState") && obj["objectState"].isString())
//        {
//            // получить состояние объекта
//            rec.objectState = obj["objectState"].toString();
//        }
//    }
//    // имя модуля
//    if (json.contains("module") && json["module"].isObject())
//    {
//        // получить модуль из JSON
//        QJsonObject module = json["module"].toObject();
//        // проверить наличие поля "имя модуля"
//        if (module.contains("displayName") && module["displayName"].isString())
//        {
//            // получить имя модуля
//            rec.moduleName = module["displayName"].toString();
//        }
//    }
//    // имя датчика
//    if (json.contains("sensors") && json["sensors"].isArray())
//    {
//        // получить массив датчиков из JSON
//        QJsonArray sensors = json["sensors"].toArray();
//        // если количество датчиков > 0
//        if ((sensors.count() > 0) && (sensors[0].isObject()))
//        {
//            // получить датчик
//            QJsonObject sensor = sensors[0].toObject();
//            // проверить наличие у первого датчика поля "Имя датчика"
//            if (sensor.contains("sensorName") && sensor["sensorName"].isString())
//            {
//                rec.sensorName = sensor["sensorName"].toString();
//            }
//            // проверить наличие у первого датчика поля "Состояние датчика"
//            if (sensor.contains("sensorState") && sensor["sensorState"].isString())
//            {
//                rec.sensorStateName = sensor["sensorState"].toString();
//            }
//            // проверить наличие у первого датчика поля "Расширенное значение"
//            if (sensor.contains("extValue") && sensor["extValue"].isDouble())
//            {
//                rec.extValue = sensor["extValue"].toInt();
//            }
//            // проверить наличие у первого датчика поля "Расположение"
//            if (sensor.contains("location") && sensor["location"].isString())
//            {
//                rec.location = sensor["location"].toString();
//            }
//        }
//    }
//    // вернуть структуру
//    return rec;
//}


void IssoRemoteLogPoller::requestLogUpdates()
{
    m_remoteLogger.getLogUpdates(m_appType, m_lastEventId);
}


void IssoRemoteLogPoller::processLogRequestCompleted(const QString &replyMsg)
{
    QMap<int, IssoEventData> pings;   // map нужен для кэширования ping-событий
    QList<IssoEventData> sensorEvents;
    QList<IssoEventData> photoEvents;
    int lastEventId = m_lastEventId;
    //
    // получить список событий
    auto newEvents = parseLogReply(replyMsg);
    // обойти все события
    foreach (auto event, newEvents)
    {
        // если структура некорректна, пропустить
        if (!event.isValid())
            continue;

        // обработать согласно типу события
        switch(event.eventType)
        {
            case EVENT_PING:
            {
                // добавить в коллекцию ping
                pings.insert(event.objectId, event);
                break;
            }
            case EVENT_SENSORS:
            {
                // добавить ping-событие
                IssoEventData ping = event;
                ping.eventType = EVENT_PING;
                // добавить в коллекцию ping
                pings.insert(ping.objectId, ping);
                //
                // добавить в коллекцию событий датчиков
                sensorEvents << event;
                break;
            }
            case EVENT_PHOTO:
            {
                // добавить ping-событие
                IssoEventData ping = event;
                ping.eventType = EVENT_PING;
                // добавить в коллекцию ping
                pings.insert(ping.objectId, ping);
                //
                // добавить в коллекцию событий датчиков
                photoEvents << event;
                break;
            }
            default:
                break;
        }

//        // если запись - сообщение о событии
//        if (event.hasEventData())
//            // добавить в коллекцию событий
//            events << event;
//        else
//            // добавить в коллекцию ping
//            pings.insert(event.objectId, event);

        // определить ID последнего события
        lastEventId = (event.id > lastEventId ? event.id : lastEventId);
    }
    // сохранить ID последнего события
    m_lastEventId = lastEventId;

//    // известить о новых событиях
//    if (!sensorEvents.isEmpty())
//        emit remoteEventsReceived(sensorEvents);
//    // известить о состояниях объектов
//    emit remotePingReceived(pings.values());


    // известить о состояниях объектов
    emit pingEventsReceived(pings.values());
    // известить о новых событиях датчиков
    if (!sensorEvents.isEmpty())
        emit sensorEventsReceived(sensorEvents);
    // известить о получении новых фото
    if (!photoEvents.isEmpty())
        emit photoEventsReceived(photoEvents);
}

//void IssoRemoteLogPoller::processLogRequestCompleted(const QString &replyMsg)
//{
//    QList<IssoDBEventData> eventRecords;
//    QMap<int, QString> pingMap;
//    // получить список JSON-сообщений из строки
//    auto jsonMessages = replyMsg.split("<br>", QString::SkipEmptyParts);
//    // обработать каждое JSON-сообщение
//    foreach (QString msg, jsonMessages)
//    {
//        // преобразовать JSON-сообщение в структуру
//        IssoDBEventData rec = parseJsonMessage(msg);
//        // если структура некорректна, пропустить
//        if (!rec.isValid())
//            continue;
//        // если запись - сообщение о событии
//        if (rec.isEventMessage())
//            eventRecords << rec;

//        // добавить состояние объекта в коллекцию ping'а
//        pingMap.insert(rec.objectId, rec.objectState);
//    }
//    // известить о новых событиях
//    if (!eventRecords.isEmpty())
//        emit remoteEventsReceived(eventRecords);
//    // известить о состояниях объектов
//    emit remotePingReceived(pingMap);
//}


void IssoRemoteLogPoller::processLogRequestFailed(const QString &errorMsg)
{
    qDebug() << errorMsg << endl;
    emit remoteRequestFailed(errorMsg);
}

