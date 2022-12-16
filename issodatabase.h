#ifndef ISSODATABASE_H
#define ISSODATABASE_H

#include <QObject>
#include <QtSql>
#include <QSortFilterProxyModel>
#include "issocommondata.h"
#include "issoeventquerymodel.h"
#include "issoobjectquerymodel.h"
#include "issoparam.h"



struct IssoEventData
{
    // дата / время
    QDateTime dateTime = QDateTime();
    // имя модуля
    QString moduleName = "";
    // имя датчика
    QString sensorName = "";
    // состояние датчика
    QString sensorStateName = "";
    // расширенное значение
    quint32 extValue = 0;
    // ID охраняемого объекта
    int objectId = -1;
    // расположение
    QString location = "";
    // состояние охраняемого объекта
    QString objectState = "";
    // ID
    int id = -1;
    // сводные данные объекта
    int totalModules = 0;
    int totalMs = 0;
    int totalChannels = 0;
    int alarmModules = 0;
    int alarmMs = 0;
    int alarmChannels = 0;
    //
    // ID камеры
    int cameraId = -1;
    // IP-адрес камеры
    int cameraIp = 0;
    // данные фото в формате base64
    QString frameData = "";
    //
    // тип события
    IssoEventType eventType = EVENT_UNKNOWN;


    IssoEventData();
    IssoEventData(const QDateTime& dateTime,
                    const QString& moduleName,
                    const QString& sensorName,
                    const QString& sensorStateName,
                    quint32 extValue,
                    int objectId,
                    const QString& location = ""/*,
                    const QString& objectState = "",
                    int id = -1*/);

    bool operator ==(const IssoEventData& other);

    bool isValid() const;
    bool hasEventData() const;
//    IssoEventType eventType() const;
};
Q_DECLARE_METATYPE(IssoEventData)



class IssoDataBase : public QObject
{
        Q_OBJECT
    private:
        QSqlDatabase m_db;
        QString m_dbName;

//        QSqlQueryModel* mp_eventsSqlModel;
        IssoEventQueryModel* mp_eventsSqlModel;
        QSortFilterProxyModel* mp_eventsProxyModel;

//        QSqlQueryModel* mp_objectsSqlModel;
        IssoObjectQueryModel* mp_objectsSqlModel;
        QSortFilterProxyModel* mp_objectsProxyModel;

        void initEventsModel();
        void uninitEventsModel();

        void initObjectsModel();
        void uninitObjectsModel();

        void populateEvents(const QSqlQuery &query);
        void populateObjects(const QSqlQuery &query);

    public:
        explicit IssoDataBase(QString dbName, QObject *parent = nullptr);
        ~IssoDataBase();

        bool isOpened();
        bool foreignKeysOn();
        bool beginTransaction();
        bool endTransaction();
        bool createObjectsTable();
        bool createEventsTable();
        bool createPhotosTable();
        bool dropObjectsTable();
        bool dropEventsTable();
        bool dropPhotosTable();
        bool insertObject(int id, /*const QString& name, const QString& address,*/
                          const QString& city, const QString& region,
                          const QString& street, const QString& building,
                          const QString& contactName, const QString& phone,
                          int modulesTotal, int msTotal, int channelsTotal,
                          const QByteArray &config);
        bool updateObjectState(int id, QString state);
        bool updateObjectInfo(int id, QString state, int modulesTotal, int modulesAlarmed,
                              int msTotal, int msAlarmed, int channelsTotal, int channelsAlarmed);
        bool deleteObject(int objectId);
        bool insertEvent(const QDateTime &dateTime, const QString &moduleName,
                         const QString &sensorName, const QString &stateName, bool alarmed,
                         int objectId, int extValue);
//        bool checkEventExists(const QDateTime &dateTime, const QString &moduleName,
//                              const QString &sensorName, const QString &stateName,
//                              int extValue, int objectId);
        bool updateEventComment(int eventId, const QString& comment);
        void selectAllEvents(int objectId);
        void selectFilteredEvents(QDate date, QTime timeFrom, QTime timeTo,
                                  const QString eventType, int objectId);
        void selectAllObjects();

        bool insertPhoto(const QDateTime &dateTime, const QString &moduleName,
                         const QString &sensorName, int cameraId,  int cameraIp,
                         const QString& imageData /*const QByteArray& imageData*/);

        QByteArray readObjectConfig(int objectId);
//        int readFirstObjectId();
        QList<int> readObjectIds();
        QMap<int, QString> readObjectNames();
        QMap<int, QString> readObjectStates();
        QMap<int, IssoObjectCard> readObjectCards();
        bool updateObjectCard(const IssoObjectCard& card);
        void readLastEvents(int objectId, const QDate &date,
                            const QTime &timeFrom, const QTime &timeTo,
                             QList<IssoEventData> &events);
        bool updateObjectConfig(int objectId, const QByteArray& configData);

        QSqlQueryModel *eventsSqlModel() const;
        QSortFilterProxyModel* eventsProxyModel() const;

        QSqlQueryModel *objectsSqlModel() const;
        QSortFilterProxyModel* objectsProxyModel() const;

        void update(IssoObject *object);
};

#endif // ISSODATABASE_H
