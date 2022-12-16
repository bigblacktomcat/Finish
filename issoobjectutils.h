#ifndef ISSOOBJECTUTILS_H
#define ISSOOBJECTUTILS_H

#include "issoobject.h"

#include <QJsonDocument>



class IssoObjectUtils
{
    private:
        static QJsonObject makeObjectScheme(IssoObject *obj);
        static QJsonArray makeBuildingArray(IssoObject *obj);
        static QJsonObject makeBuildingObject(IssoBuilding *build);
        static QJsonArray makeFloorArray(IssoBuilding *build);
        static QJsonObject makeFloorObject(IssoFloorPlan *floor);
        static QJsonArray makeModuleArray(IssoFloorPlan *floor);
        static QJsonObject makeModuleObject(IssoModule* module);
        static QJsonArray makeSensorChainArray(IssoModule* module);
        static QJsonObject makeSensorChainObject(IssoSensorChain* chain);
        static QJsonArray makeSensorArray(IssoSensorChain* chain);
        static QJsonObject makeSensorObject(IssoSensor* sensor);
        static QJsonArray makeCameraArray(IssoFloorPlan *floor);
        static QJsonObject makeCameraObject(IssoCamera* camera);
    public:
        static QJsonDocument objectSchemeAsJson(IssoObject* object);
        static QString objectSchemeAsString(IssoObject* object);
};

#endif // ISSOOBJECTUTILS_H
