#include "issodatabase.h"

IssoEventData::IssoEventData()
    : IssoEventData(QDateTime(), "", "", "", 0, -1)
{
}


IssoEventData::IssoEventData(const QDateTime &dateTime, const QString &moduleName,
                                 const QString &sensorName, const QString &sensorStateName,
                                 quint32 extValue, int objectId,
                                 const QString &location)
{
    this->dateTime = dateTime;
    this->moduleName = moduleName;
    this->sensorName = sensorName;
    this->sensorStateName = sensorStateName;
    this->extValue = extValue;
    this->objectId = objectId;
    this->location = location;
//    this->objectState = objectState;
//    this->id = id;
}


bool IssoEventData::operator ==(const IssoEventData &other)
{
    return (dateTime == other.dateTime) &&
            (moduleName == other.moduleName) &&
            (sensorName == other.sensorName) &&
            (sensorStateName == other.sensorStateName) &&
            (extValue == other.extValue) &&
            (objectId == other.objectId) &&
            (location == other.location) &&
            (objectState == other.objectState) ;
}





IssoDataBase::IssoDataBase(QString dbName, QObject *parent)
    : QObject(parent),
      m_dbName(dbName)
{
    // установить соединение с базой данных
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(dbName);
    if (m_db.open())
    {
        // включить поддержку внешних ключей
        foreignKeysOn();
        // создать таблицу охраняемых объектов
        createObjectsTable();
        // создать таблицу событий
        createEventsTable();
        // создать таблицу фотоснимков
        createPhotosTable();
        //
        initObjectsModel();
        initEventsModel();
    }
}


IssoDataBase::~IssoDataBase()
{
    qDebug() << "~IssoDataBase" << endl;
    //
    uninitEventsModel();
    uninitObjectsModel();
    //
    if (m_db.isOpen())
        m_db.close();
}


void IssoDataBase::initEventsModel()
{
    // создать SQL-модель (представление данных)
//    mp_eventsSqlModel = new QSqlQueryModel();
    mp_eventsSqlModel = new IssoEventQueryModel();
    //
    // создать прокси-модель (сортировка данных)
    mp_eventsProxyModel = new QSortFilterProxyModel();
    mp_eventsProxyModel->setSourceModel(mp_eventsSqlModel);
    // сортировка по убыванию
    mp_eventsProxyModel->sort(0, Qt::DescendingOrder);
}


void IssoDataBase::uninitEventsModel()
{
    if (mp_eventsProxyModel)
        delete mp_eventsProxyModel;
    if (mp_eventsSqlModel)
        delete mp_eventsSqlModel;
}


void IssoDataBase::initObjectsModel()
{
    // создать SQL-модель (представление данных)
//    mp_objectsSqlModel = new QSqlQueryModel();
    mp_objectsSqlModel = new IssoObjectQueryModel();
    // создать прокси-модель (сортировка данных)
    mp_objectsProxyModel = new QSortFilterProxyModel();
    mp_objectsProxyModel->setSourceModel(mp_objectsSqlModel);
    // сортировка по убыванию
    mp_objectsProxyModel->sort(0, Qt::AscendingOrder);
}


void IssoDataBase::uninitObjectsModel()
{
    if (mp_objectsProxyModel)
        delete mp_objectsProxyModel;
    if (mp_objectsSqlModel)
        delete mp_objectsSqlModel;
}


void IssoDataBase::populateEvents(const QSqlQuery& query)
{
    // выполнить запрос
    mp_eventsSqlModel->setQuery(query);
    // обработать все данные
    while(mp_eventsSqlModel->canFetchMore())
    {
        mp_eventsSqlModel->fetchMore();
    }
//    // заголовки столбцов
//    mp_eventsSqlModel->setHeaderData(1, Qt::Horizontal, tr("Дата"));
//    mp_eventsSqlModel->setHeaderData(2, Qt::Horizontal, tr("Время"));
//    mp_eventsSqlModel->setHeaderData(3, Qt::Horizontal, tr("Модуль"));
//    mp_eventsSqlModel->setHeaderData(4, Qt::Horizontal, tr("Состояние"));
//    mp_eventsSqlModel->setHeaderData(5, Qt::Horizontal, tr("Датчик"));
//    mp_eventsSqlModel->setHeaderData(7, Qt::Horizontal, tr("Детализация"));
}


void IssoDataBase::populateObjects(const QSqlQuery &query)
{
    // выполнить запрос
    mp_objectsSqlModel->setQuery(query);
    // обработать все данные
    while(mp_objectsSqlModel->canFetchMore())
    {
        mp_objectsSqlModel->fetchMore();
    }
//    // заголовки столбцов
//    mp_objectsSqlModel->setHeaderData(1, Qt::Horizontal, tr("Наименование"));
//    mp_objectsSqlModel->setHeaderData(2, Qt::Horizontal, tr("Контактное лицо"));
//    mp_objectsSqlModel->setHeaderData(3, Qt::Horizontal, tr("Телефон"));
//    mp_objectsSqlModel->setHeaderData(4, Qt::Horizontal, tr("Адрес"));
//    mp_objectsSqlModel->setHeaderData(5, Qt::Horizontal, tr("Состояние"));
}


void IssoDataBase::selectAllEvents(int objectId)
{
    // строка запроса
    QString queryStr =
            "SELECT e.id, strftime('%d-%m-%Y', e.date_time) date, TIME(e.date_time) time, "
                   "e.module_name, e.event_status, e.sensor, e.comment, e.ext_value, p.image "
            "FROM events AS e "
                "LEFT JOIN photos AS p "
                "ON e.photo_id = p.id "
            "WHERE e.object_id = :object_id;";
    // подготовить запрос
    QSqlQuery query;
    query.prepare(queryStr);
    query.bindValue(":object_id", objectId);
    bool res = query.exec();
    //
//    qDebug() << "events filtered = " << res << endl;
    //
    // выполнить запрос и отобразить
    populateEvents(query);
}
//void IssoDataBase::selectAllEvents(int objectId)
//{
//    // строка запроса
//    QString queryStr =
//            "SELECT id, strftime('%d-%m-%Y', date_time) date, TIME(date_time) time, "
//                   "module_name, event_status, sensor, comment, ext_value, photo_id "
//            "FROM events "
//            "WHERE object_id = :object_id;";
//    // подготовить запрос
//    QSqlQuery query;
//    query.prepare(queryStr);
//    query.bindValue(":object_id", objectId);
//    bool res = query.exec();
//    //
////    qDebug() << "events filtered = " << res << endl;
//    //
//    // выполнить запрос и отобразить
//    populateEvents(query);
//}


void IssoDataBase::selectFilteredEvents(QDate date, QTime timeFrom, QTime timeTo,
                                        const QString eventType, int objectId)
{
    // строка запроса
    QString queryStr =
            "SELECT e.id, strftime('%d-%m-%Y', e.date_time) date, TIME(e.date_time) time, "
                   "e.module_name, e.event_status, e.sensor, e.comment, e.ext_value, p.image "
            "FROM events AS e "
                "LEFT JOIN photos AS p "
                "ON e.photo_id = p.id "
            "WHERE (e.object_id = :object_id) AND "
                  "(date(e.date_time) = :date) AND "
                  "(time(e.date_time) BETWEEN :timeFrom AND :timeTo) AND "
                  "(e.sensor LIKE :eventType);";
    // подготовить запрос
    QSqlQuery query;
    query.prepare(queryStr);
    query.bindValue(":object_id",   objectId);
    query.bindValue(":date",        date);
    query.bindValue(":timeFrom",    timeFrom);
    query.bindValue(":timeTo",      timeTo);
    query.bindValue(":eventType",   "%" + eventType + "%");
    bool res = query.exec();
    //
//    qDebug() << "events filtered = " << res << endl;
    //
    // выполнить запрос и отобразить
    populateEvents(query);
}
//void IssoDataBase::selectFilteredEvents(QDate date, QTime timeFrom, QTime timeTo,
//                                        const QString eventType, int objectId)
//{
//    // строка запроса
//    QString queryStr =
//            "SELECT id, strftime('%d-%m-%Y', date_time) date, TIME(date_time) time, "
//                   "module_name, event_status, sensor, comment, ext_value, photo_id "
//            "FROM events "
//            "WHERE (object_id = :object_id) AND"
//                  "(date(date_time) = :date) AND "
//                  "(time(date_time) BETWEEN :timeFrom AND :timeTo) AND "
//                  "(sensor LIKE :eventType);";
//    // подготовить запрос
//    QSqlQuery query;
//    query.prepare(queryStr);
//    query.bindValue(":object_id",   objectId);
//    query.bindValue(":date",        date);
//    query.bindValue(":timeFrom",    timeFrom);
//    query.bindValue(":timeTo",      timeTo);
//    query.bindValue(":eventType",   "%" + eventType + "%");
//    bool res = query.exec();
//    //
////    qDebug() << "events filtered = " << res << endl;
//    //
//    // выполнить запрос и отобразить
//    populateEvents(query);
//}


void IssoDataBase::selectAllObjects()
{
    // строка запроса
//    QString queryStr =
//            "SELECT id, name, contact_name, phone, address, state, "
//                    "modules_total, modules_alarm, ms_total, ms_alarm, "
//                    "channels_total, channels_alarm "
//            "FROM objects;";
    QString queryStr =
            "SELECT id, id || '-' || city AS name, contact_name, phone, "
                    "city || ', ' || region || ', ' || street || ', ' || building AS address, state, "
                    "modules_total, modules_alarm, ms_total, ms_alarm, "
                    "channels_total, channels_alarm "
            "FROM objects;";
    // подготовить запрос
    QSqlQuery query;
    query.prepare(queryStr);
    bool res = query.exec();
    //
//    qDebug() << "objects selected = " << res << endl;
    //
    // выполнить запрос и отобразить
    populateObjects(query);
}


bool IssoDataBase::insertPhoto(const QDateTime &dateTime, const QString &moduleName,
                               const QString &sensorName,  int cameraId, int cameraIp,
                               const QString &imageData)
{
    // запрос добавления фото в таблицу
    QString queryStr =
            "INSERT INTO photos (date_time, camera_id, camera_ip, image) "
            "VALUES (:dateTime, :cameraId, :cameraIp, :imageData);";
    // подготовить запрос
    QSqlQuery insertQuery;
    insertQuery.prepare(queryStr);
    // привязать значения
    insertQuery.bindValue(":dateTime",    dateTime);
    insertQuery.bindValue(":cameraId",    cameraId);
    insertQuery.bindValue(":cameraIp",    cameraIp);
    insertQuery.bindValue(":imageData",   imageData);
    // выполнить запрос
    bool res = insertQuery.exec();
    //
    qDebug() << "Photo inserted = " << res << endl;
    if (!res)
        return false;
    //
    // id добавленной записи
    int lastPhotoId = insertQuery.lastInsertId().toInt();
    //
    // запрос обновления photo_id события
    queryStr =
            "UPDATE events "
            "SET photo_id = :photoId "
            "WHERE (date_time = :dateTime) AND "
                  "(module_name = :moduleName) AND "
                  "(sensor = :sensorName);";
    QSqlQuery updateQuery;
    updateQuery.prepare(queryStr);
    updateQuery.bindValue(":dateTime",    dateTime);
    updateQuery.bindValue(":moduleName",  moduleName);
    updateQuery.bindValue(":sensorName",  sensorName);
    updateQuery.bindValue(":photoId",     lastPhotoId);
    res = updateQuery.exec();
    //
    qDebug() << "Event photo_id updated = " << res << "; photoId = " << lastPhotoId << endl;
    //
    return res;
}


QByteArray IssoDataBase::readObjectConfig(int objectId)
{
    QByteArray result;
    // строка запроса
    QString queryStr =
            "SELECT config "
            "FROM objects "
            "WHERE id = :object_id;";
    // подготовить запрос
    QSqlQuery query;
    query.prepare(queryStr);
    query.bindValue(":object_id", objectId);
    // выполнить запрос
    if (query.exec() && query.first() && query.isValid())
    {
        // получить массив данных конфигурации объекта
        result = query.value("config").toByteArray();
    }
    return result;
}


QList<int> IssoDataBase::readObjectIds()
{
    QList<int> ids;
    // строка запроса
    QString queryStr =
            "SELECT id "
            "FROM objects;";
    // подготовить запрос
    QSqlQuery query;
    query.prepare(queryStr);
    // выполнить запрос
    if (query.exec())
    {
        // проход по всем записям
        while(query.next() && query.isValid())
        {
            // добавить в коллекцию
            ids << query.value("id").toInt();
        }
    }
    return ids;
}


//int IssoDataBase::readFirstObjectId()
//{
//    int result = -1;
//    // строка запроса
//    QString queryStr =
//            "SELECT id "
//            "FROM objects;";
//    // подготовить запрос
//    QSqlQuery query;
//    query.prepare(queryStr);
//    // выполнить запрос
//    if (query.exec() && query.first() && query.isValid())
//    {
//        // получить id первого объекта
//        result = query.value("id").toInt();
//    }
//    return result;
//}


QMap<int, QString> IssoDataBase::readObjectNames()
{
    QMap<int, QString> map;
    // строка запроса
//    QString queryStr =
//            "SELECT id, name "
//            "FROM objects;";
    QString queryStr =
            "SELECT id, id || '-' || city AS name "
            "FROM objects;";
    // подготовить запрос
    QSqlQuery query;
    query.prepare(queryStr);
    // выполнить запрос
    if (query.exec())
    {
        // проход по всем записям
        while(query.next() && query.isValid())
        {
            // ID объекта
            int id = query.value("id").toInt();
            // имя объекта
            QString name = query.value("name").toString();
            // добавить в коллекцию
            map.insert(id, name);
//            objNames << name;
        }
    }
    return map;
}


QMap<int, QString> IssoDataBase::readObjectStates()
{
    QMap<int, QString> map;
    // строка запроса
    QString queryStr =
            "SELECT id, state "
            "FROM objects;";
    // подготовить запрос
    QSqlQuery query;
    query.prepare(queryStr);
    // выполнить запрос
    if (query.exec())
    {
        // проход по всем записям
        while(query.next() && query.isValid())
        {
            // id
            int id = query.value("id").toInt();
            // состояние
            QString state = query.value("state").toString();
            // добавить в коллекцию
            map.insert(id, state);
        }
    }
    return map;
}


QMap<int, IssoObjectCard> IssoDataBase::readObjectCards()
{
    QMap<int, IssoObjectCard> map;
    // строка запроса
//    QString queryStr =
//            "SELECT id, name, contact_name, phone, address "
//            "FROM objects;";
    QString queryStr =
            "SELECT id, city, region, street, building, contact_name, phone "
            "FROM objects;";
    // подготовить запрос
    QSqlQuery query;
    query.prepare(queryStr);
    // выполнить запрос
    if (query.exec())
    {
        // проход по всем записям
        while(query.next() && query.isValid())
        {
            IssoObjectCard card;
            // id
            card.setId(query.value("id").toInt());
            // город
            card.setCity(query.value("city").toString());
            // регион
            card.setRegion(query.value("region").toString());
            // улица
            card.setStreet(query.value("street").toString());
            // строение
            card.setBuilding(query.value("building").toString());
            // контакт
            card.setContactName(query.value("contact_name").toString());
            // телефон
            card.setPhone(query.value("phone").toString());
            // добавить в коллекцию
            map.insert(card.id(), card);
        }
    }
    return map;
}


bool IssoDataBase::updateObjectCard(const IssoObjectCard &card)
{
    // строка запроса
    QString queryStr =
            "UPDATE objects "
            "SET city = :city, "
                "region = :region, "
                "street = :street, "
                "building = :building,"
                "contact_name = :contact_name, "
                "phone = :phone "
            "WHERE id = :object_id;";
    // подготовить запрос
    QSqlQuery query;
    query.prepare(queryStr);
    // привязать значения
    query.bindValue(":object_id",       card.id());
    query.bindValue(":city",            card.city());
    query.bindValue(":region",          card.region());
    query.bindValue(":street",          card.street());
    query.bindValue(":building",        card.building());
    query.bindValue(":contact_name",    card.contactName());
    query.bindValue(":phone",           card.phone());
    // выполнить запрос
    bool res = query.exec();
    //
    qDebug() << "Object card updated = " << res << endl;
    //
    return res;
}



//void IssoDataBase::readLastEvents(int objectId,
//                                  const QDate &date,
//                                  const QTime &timeFrom,
//                                  const QTime &timeTo,
//                                  QList<IssoEventData>& events)
//{
////    // строка запроса
////    QString queryStr =
////            "SELECT id, date_time, module_name, event_status, sensor, ext_value "
////            "FROM events "
////            "WHERE (object_id = :object_id) AND"
////                  "(date(date_time) = :date) AND "
////                  "(time(date_time) BETWEEN :timeFrom AND :timeTo);";
////    // подготовить запрос
////    QSqlQuery query;
////    query.prepare(queryStr);
////    query.bindValue(":object_id",   objectId);
////    query.bindValue(":date",        date);
////    query.bindValue(":timeFrom",    timeFrom);
////    query.bindValue(":timeTo",      timeTo);

//    // строка запроса
//    QString queryStr =
//            "SELECT e.id, strftime('%d-%m-%Y', e.date_time) date, TIME(e.date_time) time, "
//                   "e.module_name, e.event_status, e.sensor, e.comment, e.ext_value, p.image "
//            "FROM events AS e "
//                "LEFT JOIN photos AS p "
//                "ON e.photo_id = p.id "
//            "WHERE (e.object_id = :object_id) AND "
//                  "(date(e.date_time) = :date) AND "
//                  "(time(e.date_time) BETWEEN :timeFrom AND :timeTo) AND "
//                  "(e.sensor LIKE :eventType);";
//    // подготовить запрос
//    QSqlQuery query;
//    query.prepare(queryStr);
//    query.bindValue(":object_id",   objectId);
//    query.bindValue(":date",        date);
//    query.bindValue(":timeFrom",    timeFrom);
//    query.bindValue(":timeTo",      timeTo);
//    query.bindValue(":eventType",   "%" + eventType + "%");
//    // выполнить запрос
//    if (!query.exec())
//        return;
//    while(query.next() && query.isValid())
//    {
//        IssoEventData event;
//        // дата / время
//        QString dtString = query.value("date_time").toString();
//        event.dateTime = QDateTime::fromString(dtString, Qt::ISODateWithMs);
//        // имя модуля
//        event.moduleName = query.value("module_name").toString();
//        // имя датчика
//        event.sensorName = query.value("sensor").toString();
//        // состояние датчика
//        event.sensorStateName = query.value("event_status").toString();
//        // расширенное значение
//        event.extValue = query.value("ext_value").toInt();
//        // добавить структуру в список
//        events << event;
//    }
//}
void IssoDataBase::readLastEvents(int objectId,
                                  const QDate &date,
                                  const QTime &timeFrom,
                                  const QTime &timeTo,
                                  QList<IssoEventData>& events)
{
    // строка запроса
    QString queryStr =
            "SELECT id, date_time, module_name, event_status, sensor, ext_value "
            "FROM events "
            "WHERE (object_id = :object_id) AND"
                  "(date(date_time) = :date) AND "
                  "(time(date_time) BETWEEN :timeFrom AND :timeTo);";
    // подготовить запрос
    QSqlQuery query;
    query.prepare(queryStr);
    query.bindValue(":object_id",   objectId);
    query.bindValue(":date",        date);
    query.bindValue(":timeFrom",    timeFrom);
    query.bindValue(":timeTo",      timeTo);
    // выполнить запрос
    if (!query.exec())
        return;
    while(query.next() && query.isValid())
    {
        IssoEventData event;
        // дата / время
        QString dtString = query.value("date_time").toString();
        event.dateTime = QDateTime::fromString(dtString, Qt::ISODateWithMs);
        // имя модуля
        event.moduleName = query.value("module_name").toString();
        // имя датчика
        event.sensorName = query.value("sensor").toString();
        // состояние датчика
        event.sensorStateName = query.value("event_status").toString();
        // расширенное значение
        event.extValue = query.value("ext_value").toInt();
        // добавить структуру в список
        events << event;
    }
}


bool IssoDataBase::updateObjectConfig(int objectId, const QByteArray &configData)
{
    // строка запроса
    QString queryStr =
            "UPDATE objects "
            "SET config = :config "
            "WHERE id = :object_id;";
    // подготовить запрос
    QSqlQuery query;
    query.prepare(queryStr);
    query.bindValue(":object_id",   objectId);
    query.bindValue(":config",      configData);
    bool res = query.exec();
    //
//    qDebug() << "Config updated = " << res << endl;
    //
    return res;
}


bool IssoDataBase::isOpened()
{
    return m_db.isOpen();
}


bool IssoDataBase::foreignKeysOn()
{
    QSqlQuery query;
    bool res = query.exec("PRAGMA foreign_keys = ON;");
    //
//    qDebug() << "Foreign keys support = " << res << endl;
    //
    return res;
}


bool IssoDataBase::beginTransaction()
{
    return m_db.transaction();
}


bool IssoDataBase::endTransaction()
{
    return m_db.commit();
}


bool IssoDataBase::createObjectsTable()
{
    QSqlQuery query;
//    QString createObjectsStr =
//            "CREATE TABLE IF NOT EXISTS objects ("
////                "id INTEGER PRIMARY KEY AUTOINCREMENT, "
//                "id INTEGER PRIMARY KEY, "
//                "name TEXT NOT NULL, "
//                "address TEXT NOT NULL, "
//                "contact_name TEXT NOT NULL, "
//                "phone TEXT NOT NULL, "
//                "state TEXT NOT NULL, "
//                "modules_total INTEGER NOT NULL, "
//                "modules_alarm INTEGER NOT NULL, "
//                "ms_total INTEGER NOT NULL, "
//                "ms_alarm INTEGER NOT NULL, "
//                "channels_total INTEGER NOT NULL, "
//                "channels_alarm INTEGER NOT NULL, "
//                "config BLOB"
//            ");";
    QString createObjectsStr =
            "CREATE TABLE IF NOT EXISTS objects ("
//                "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                "id INTEGER PRIMARY KEY, "
                "city TEXT NOT NULL, "
                "region TEXT NOT NULL, "
                "street TEXT NOT NULL, "
                "building TEXT NOT NULL, "
                "contact_name TEXT NOT NULL, "
                "phone TEXT NOT NULL, "
                "state TEXT NOT NULL, "
                "modules_total INTEGER NOT NULL, "
                "modules_alarm INTEGER NOT NULL, "
                "ms_total INTEGER NOT NULL, "
                "ms_alarm INTEGER NOT NULL, "
                "channels_total INTEGER NOT NULL, "
                "channels_alarm INTEGER NOT NULL, "
                "config BLOB"
            ");";
    bool res = query.exec(createObjectsStr);
    //
    qDebug() << "Objects table created = " << res << endl;
    //
    return res;
}
//bool IssoDataBase::createObjectsTable()
//{
//    QSqlQuery query;
//    QString createObjectsStr =
//            "CREATE TABLE IF NOT EXISTS objects ("
////                "id INTEGER PRIMARY KEY AUTOINCREMENT, "
//                "id INTEGER PRIMARY KEY, "
//                "name TEXT NOT NULL, "
//                "address TEXT NOT NULL, "
//                "contact_name TEXT NOT NULL, "
//                "phone TEXT NOT NULL, "
//                "state TEXT NOT NULL, "
//                "config BLOB"
//            ");";
//    bool res = query.exec(createObjectsStr);
//    //
////    qDebug() << "Objects table created = " << res << endl;
//    //
//    return res;
//}



bool IssoDataBase::createEventsTable()
{
    QSqlQuery query;
    QString createEventsStr =
            "CREATE TABLE IF NOT EXISTS events ("
                "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                "date_time TEXT NOT NULL, "
                "module_name TEXT NOT NULL, "
                "event_status TEXT NOT NULL, "
                "sensor TEXT NOT NULL, "
                "comment TEXT, "
                "ext_value INTEGER NOT NULL, "
                "object_id INTEGER NOT NULL, "
                "photo_id INTEGER, "
                "CONSTRAINT fk_object_id "
                    "FOREIGN KEY (object_id) "
                    "REFERENCES objects(id) "
                    "ON DELETE CASCADE, "
                "CONSTRAINT fk_photo_id "
                    "FOREIGN KEY (photo_id) "
                    "REFERENCES photos(id) "
                    "ON DELETE CASCADE"
            ");";
    bool res = query.exec(createEventsStr);
    //
    qDebug() << "Events table created = " << res << endl;
    //
    return res;
}
//bool IssoDataBase::createEventsTable()
//{
//    QSqlQuery query;
//    QString createEventsStr =
//            "CREATE TABLE IF NOT EXISTS events ("
//                "id INTEGER PRIMARY KEY AUTOINCREMENT, "
//                "date_time TEXT NOT NULL, "
//                "module_name TEXT NOT NULL, "
//                "event_status TEXT NOT NULL, "
//                "sensor TEXT NOT NULL, "
//                "comment TEXT, "
//                "ext_value INTEGER NOT NULL, "
//                "object_id INTEGER NOT NULL, "
//                "CONSTRAINT fk_object_id "
//                    "FOREIGN KEY (object_id) "
//                    "REFERENCES objects(id) "
//                    "ON DELETE CASCADE"
//            ");";
//    bool res = query.exec(createEventsStr);
//    //
////    qDebug() << "Events table created = " << res << endl;
//    //
//    return res;
//}


bool IssoDataBase::createPhotosTable()
{
    QSqlQuery query;
    QString createPhotosStr =
            "CREATE TABLE IF NOT EXISTS photos ("
                "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                "date_time TEXT NOT NULL, "
                "camera_id INTEGER NOT NULL, "
                "camera_ip INTEGER NOT NULL, "
                "image TEXT"
//                "image BLOB"
            ");";
    bool res = query.exec(createPhotosStr);
    //
    qDebug() << "Photos table created = " << res << endl;
    //
    return res;
}


bool IssoDataBase::dropObjectsTable()
{
    QSqlQuery query;
    bool res = query.exec("DROP TABLE objects;");
    //
    qDebug() << "Objects table dropped = " << res << endl;
    //
    return res;
}


bool IssoDataBase::dropEventsTable()
{
    QSqlQuery query;
    bool res = query.exec("DROP TABLE events;");
    //
    qDebug() << "Events table dropped = " << res << endl;
    //
    return res;
}


bool IssoDataBase::dropPhotosTable()
{
    QSqlQuery query;
    bool res = query.exec("DROP TABLE photos;");
    //
    qDebug() << "Photos table dropped = " << res << endl;
    //
    return res;
}


QSqlQueryModel *IssoDataBase::eventsSqlModel() const
{
    return mp_eventsSqlModel;
}


QSortFilterProxyModel *IssoDataBase::eventsProxyModel() const
{
    return mp_eventsProxyModel;
}


QSqlQueryModel *IssoDataBase::objectsSqlModel() const
{
    return mp_objectsSqlModel;
}


QSortFilterProxyModel *IssoDataBase::objectsProxyModel() const
{
    return mp_objectsProxyModel;
}


void IssoDataBase::update(IssoObject* object)
{
    if (!object || !mp_eventsSqlModel)
        return;
    // обновить данные модели
    mp_eventsSqlModel->update(object);
}


bool IssoDataBase::insertObject(int id, const QString &city, const QString &region,
                                const QString &street, const QString &building,
                                const QString &contactName, const QString &phone,
                                int modulesTotal, int msTotal, int channelsTotal,
                                const QByteArray &config)
{
//    QString queryStr =
//            "INSERT INTO objects (id, name, address, contact_name, phone, state, "
//                                "modules_total, modules_alarm, ms_total, ms_alarm, "
//                                "channels_total, channels_alarm, config) "
//            "VALUES (:id, :name, :address, :contact_name, :phone, :state, "
//                    ":modules_total, :modules_alarm, :ms_total, :ms_alarm, "
//                    ":channels_total, :channels_alarm, :config);";
    QString queryStr =
            "INSERT INTO objects (id, city, region, street, building, "
                                "contact_name, phone, state, "
                                "modules_total, modules_alarm, ms_total, ms_alarm, "
                                "channels_total, channels_alarm, config) "
            "VALUES (:id, :city, :region, :street, :building, "
                    ":contact_name, :phone, :state, "
                    ":modules_total, :modules_alarm, :ms_total, :ms_alarm, "
                    ":channels_total, :channels_alarm, :config);";
    // подготовить запрос
    QSqlQuery query;
    query.prepare(queryStr);
    // привязать значения
    query.bindValue(":id",              id);
    query.bindValue(":city",            city);
    query.bindValue(":region",          region);
    query.bindValue(":street",          street);
    query.bindValue(":building",        building);
    query.bindValue(":contact_name",    contactName);
    query.bindValue(":phone",           phone);
    query.bindValue(":state",           IssoCommonData::stringByModuleState(INACTIVE));
    query.bindValue(":modules_total",   modulesTotal);
    query.bindValue(":modules_alarm",   0);
    query.bindValue(":ms_total",        msTotal);
    query.bindValue(":ms_alarm",        0);
    query.bindValue(":channels_total",  channelsTotal);
    query.bindValue(":channels_alarm",  0);
    query.bindValue(":config",          config);
    // выполнить запрос
    bool res = query.exec();
    //
    qDebug() << "Object inserted = " << res << endl;
    //
    return res;
}


bool IssoDataBase::updateObjectState(int id, QString state)
{
    QString queryStr =
            "UPDATE objects "
            "SET state = :state "
            "WHERE id = :id;";
    QSqlQuery query;
    query.prepare(queryStr);
    query.bindValue(":id",      id);
    query.bindValue(":state",   state);
    //
    bool res = query.exec();
    //
//    qDebug() << "Object state updated = " << res << "; id = " << state << endl;
    //
    return res;
}


bool IssoDataBase::updateObjectInfo(int id, QString state,
                                     int modulesTotal, int modulesAlarmed,
                                     int msTotal, int msAlarmed,
                                     int channelsTotal, int channelsAlarmed)
{
    QString queryStr =
            "UPDATE objects "
            "SET state = :state, "
                "modules_total = :modules_total, modules_alarm = :modules_alarm, "
                "ms_total = :ms_total, ms_alarm = :ms_alarm, "
                "channels_total = :channels_total, channels_alarm = :channels_alarm "
            "WHERE id = :id;";
//    QString queryStr =
//            "UPDATE objects "
//            "SET state = :state "
//            "WHERE id = :id;";
    QSqlQuery query;
    query.prepare(queryStr);
    query.bindValue(":id",      id);
    query.bindValue(":state",   state);

    query.bindValue(":modules_total",   modulesTotal);
    query.bindValue(":modules_alarm",   modulesAlarmed);
    query.bindValue(":ms_total",        msTotal);
    query.bindValue(":ms_alarm",        msAlarmed);
    query.bindValue(":channels_total",  channelsTotal);
    query.bindValue(":channels_alarm",  channelsAlarmed);

    bool res = query.exec();
    //
    qDebug() << "Object info updated = " << res << "; id = " << id << endl;
    //
    return res;
}


bool IssoDataBase::deleteObject(int objectId)
{
    QString queryStr =
            "DELETE FROM objects "
            "WHERE id = :object_id;";
    // подготовить запрос
    QSqlQuery query;
    query.prepare(queryStr);
    // привязать значения
    query.bindValue(":object_id", objectId);
    // выполнить запрос
    bool res = query.exec();
    //
//    qDebug() << "Object deleted = " << res << endl;
    //
    return res;
}


bool IssoDataBase::insertEvent(const QDateTime &dateTime,
                               const QString &moduleName,
                               const QString &sensorName,
                               const QString &stateName,
                               bool alarmed,  int objectId, int extValue)
{
    QString queryStr =
            "INSERT INTO events (date_time, module_name, event_status, sensor, comment, ext_value, object_id) "
            "VALUES (:dateTime, :moduleName, :eventStatus, :sensor, :comment, :extValue, :object_id);";
    // подготовить запрос
    QSqlQuery query;
    query.prepare(queryStr);
    // привязать значения
    query.bindValue(":dateTime",        dateTime);
    query.bindValue(":moduleName",      moduleName);
    query.bindValue(":eventStatus",     stateName);
    query.bindValue(":sensor",          sensorName);
    // если событие тревожное, в комментарий записать NULL, иначе пустую строку
    // (необработанная тревога в таблице выделяется цветом)
    query.bindValue(":comment",         alarmed ? NULL : "");
    query.bindValue(":extValue",        extValue);
    query.bindValue(":object_id",       objectId);
    // выполнить запрос
    bool res = query.exec();
    //
    qDebug() << "Event inserted = " << res << endl;
    //
    return res;
}


//bool IssoDataBase::checkEventExists(const QDateTime &dateTime, const QString &moduleName,
//                                    const QString &sensorName, const QString &stateName,
//                                    int extValue, int objectId)
//{
//    QString queryStr =
//            "SELECT COUNT(*) count "
//            "FROM events "
//            "WHERE (date_time = :dateTime) AND "
//                  "(module_name = :moduleName) AND "
//                  "(sensor = :sensorName) AND "
//                  "(event_status = :eventStatus) AND "
//                  "(ext_value = :extValue) AND "
//                  "(object_id = :objectId);";
//    // подготовить запрос
//    QSqlQuery query;
//    query.prepare(queryStr);
//    // привязать значения
//    query.bindValue(":dateTime",        dateTime);
//    query.bindValue(":moduleName",      moduleName);
//    query.bindValue(":sensorName",      sensorName);
//    query.bindValue(":eventStatus",     stateName);
//    query.bindValue(":extValue",        extValue);
//    query.bindValue(":objectId",        objectId);
//    // выполнить запрос
//    if (query.exec() && query.first() && query.isValid())
//    {
////        qDebug() << "count = " << query.value("count").toInt() << endl;
//        return (query.value("count").toInt() > 0);
//    }
//    return false;
//}


bool IssoDataBase::updateEventComment(int eventId, const QString &comment)
{
    QString queryStr =
            "UPDATE events "
            "SET comment = :comment "
            "WHERE id = :id;";
    QSqlQuery query;
    query.prepare(queryStr);
    query.bindValue(":id",      eventId);
    query.bindValue(":comment", comment);
    bool res = query.exec();
    //
//    qDebug() << "Comment updated = " << res << "; id = " << eventId << endl;
    //
    return res;
}


bool IssoEventData::isValid() const
{
    return (dateTime.isValid() && (objectId != -1));
}


bool IssoEventData::hasEventData() const
{
    return !moduleName.isEmpty() && !sensorName.isEmpty();
}


//IssoEventType IssoEventData::eventType() const
//{
//    if (!dateTime.isValid() || (objectId == -1) || (objectState.isEmpty()))
//        return EVENT_UNKNOWN;
//    if (moduleName.isEmpty() || sensorName.isEmpty())
//        return EVENT_PING;
//    if ()

//}
