#ifndef ISSOLOGMESSAGEBUILDER_H
#define ISSOLOGMESSAGEBUILDER_H

#include "issoobject.h"

#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>



class IssoLogMessageBuilder
{
    private:
        QJsonObject m_json;

        QJsonObject createObjectData(const IssoObject* obj);
//        QJsonObject createObjectData(int objId,
//                                     const QString& objName,
//                                     const QString& objState,
//                                     const QString& address,
//                                     const QString& contactName,
//                                     const QString& phone);
        QJsonObject createModuleData(const IssoModule* module);
//        QJsonObject createModuleData(const QString& displayName,
//                                     int buildNum, int floorNum,
//                                     const QString& state);

    public:
        IssoLogMessageBuilder(const IssoObject* obj,
                              IssoEventType eventType,
                              const QString &dateTime);

        void addModuleData(const IssoModule *module);
        void addSensorData(const QString& sensorName,
                           const QString& sensorState,
                           int extValue, const QString &location);
        void addPhotoData(const QString& sensorName,
                          int cameraId, int cameraIp,
                          const QString& frameData);
        QString getMessage();
};

#endif // ISSOLOGMESSAGEBUILDER_H
