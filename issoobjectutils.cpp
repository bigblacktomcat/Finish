#include "issoobjectutils.h"

#include <QJsonArray>
#include <QJsonObject>


QJsonObject IssoObjectUtils::makeObjectScheme(IssoObject* obj)
{
    if (!obj)
        return QJsonObject();
    // структура охраняемого объекта
    QJsonObject jsonObject;
    // карточка объекта
    jsonObject["objectId"]    = obj->id();
    jsonObject["objectName"]  = obj->displayName();    //obj->objectCard().name();
    jsonObject["address"]     = obj->objectCard().address();
    jsonObject["contactName"] = obj->objectCard().contactName();
    jsonObject["phone"]       = obj->objectCard().phone();
    // массив структур зданий
    jsonObject["buildings"]   = makeBuildingArray(obj);
    //
    return jsonObject;
}


QJsonArray IssoObjectUtils::makeBuildingArray(IssoObject *obj)
{
    if (!obj)
        return QJsonArray();
    // массив структур зданий объекта
    QJsonArray jsonBuildings;
    foreach (IssoBuilding* build, obj->buildings())
    {
        // добавить структуру здания
        jsonBuildings.append(makeBuildingObject(build));
    }
    return jsonBuildings;
}


QJsonObject IssoObjectUtils::makeBuildingObject(IssoBuilding *build)
{
    if (!build)
        return QJsonObject();
    // структура здания
    QJsonObject jsonBuilding;
    jsonBuilding["buildingNumber"] = build->number();
    // массив структур этажей здания
    jsonBuilding["floors"]         = makeFloorArray(build);
    //
    return jsonBuilding;
}


QJsonArray IssoObjectUtils::makeFloorArray(IssoBuilding *build)
{
    if (!build)
        return QJsonArray();
    // массив структур этажей здания
    QJsonArray jsonFloors;
    foreach (IssoFloorPlan* floor, build->floors())
    {
        // добавить структуру этажа
        jsonFloors.append(makeFloorObject(floor));
    }
    //
    return jsonFloors;
}


QJsonObject IssoObjectUtils::makeFloorObject(IssoFloorPlan *floor)
{
    if (!floor)
        return QJsonObject();
    // структура этажа
    QJsonObject jsonFloor;
    jsonFloor["floorNumber"]      = floor->number();
    jsonFloor["backgroundWidth"]  = floor->backgroundSize().width();
    jsonFloor["backgroundHeight"] = floor->backgroundSize().height();
    // массив структур модулей
    jsonFloor["modules"]          = makeModuleArray(floor);
    // массив структур камер
    jsonFloor["cameras"]          = makeCameraArray(floor);
    //
    return jsonFloor;
}


QJsonArray IssoObjectUtils::makeModuleArray(IssoFloorPlan *floor)
{
    if (!floor)
        return QJsonArray();
    // массив структур модулей
    QJsonArray jsonModules;
    foreach (IssoModule* module, floor->modules())
    {
        // добавить структуру модуля
        jsonModules.append(makeModuleObject(module));
    }
    return jsonModules;
}


QJsonObject IssoObjectUtils::makeModuleObject(IssoModule *module)
{
    if (!module)
        return QJsonObject();
    // структура модуля
    QJsonObject jsonModule;
    jsonModule["moduleId"]     = module->uniqueId();
    jsonModule["moduleName"]   = module->displayName();
    jsonModule["x"]            = module->x();
    jsonModule["y"]            = module->y();
    // массив структур шлейфов
    jsonModule["sensorChains"] = makeSensorChainArray(module);
    //
    return jsonModule;
}


QJsonArray IssoObjectUtils::makeSensorChainArray(IssoModule *module)
{
    if (!module)
        return QJsonArray();
    // массив структур шлейфов
    QJsonArray jsonChains;
    foreach (auto id, module->sensorChains().keys())
    {
        // получить шлейф по ID
        IssoSensorChain* chain = module->sensorChain(id);
        // добавить структуру шлейфа
        jsonChains.append(makeSensorChainObject(chain));
    }
    return jsonChains;
}


QJsonObject IssoObjectUtils::makeSensorChainObject(IssoSensorChain *chain)
{
    if (!chain)
        return QJsonObject();
    // структура шлейфа
    QJsonObject jsonChain;
    jsonChain["chainId"]   = chain->id();
    jsonChain["chainType"] = chain->type();
    // массив структур датчиков шлейфа
    jsonChain["sensors"]   = makeSensorArray(chain);
    return jsonChain;
}


QJsonArray IssoObjectUtils::makeSensorArray(IssoSensorChain *chain)
{
    if (!chain)
        return QJsonArray();
    // массив структур датчиков
    QJsonArray jsonSensors;
    foreach (IssoSensor* sensor, chain->sensors())
    {
        // добавить структуру датчика
        jsonSensors.append(makeSensorObject(sensor));
    }
    return jsonSensors;
}


QJsonObject IssoObjectUtils::makeSensorObject(IssoSensor *sensor)
{
    if (!sensor)
        return QJsonObject();
    // структура датчика
    QJsonObject jsonSensor;
    jsonSensor["sensorId"] = sensor->id();
    jsonSensor["x"]        = sensor->x();
    jsonSensor["y"]        = sensor->y();
    jsonSensor["location"] = sensor->locationInfo();
    //
    return jsonSensor;
}


QJsonArray IssoObjectUtils::makeCameraArray(IssoFloorPlan *floor)
{
    if (!floor)
        return QJsonArray();
    // массив структур камер
    QJsonArray jsonCameras;
    foreach (IssoCamera* camera, floor->cameras())
    {
        // добавить структуру камеры
        jsonCameras.append(makeCameraObject(camera));
    }
    return jsonCameras;
}


QJsonObject IssoObjectUtils::makeCameraObject(IssoCamera *camera)
{
    if (!camera)
        return QJsonObject();
    // структура камеры
    QJsonObject jsonCamera;
    jsonCamera["x"] = camera->x();
    jsonCamera["y"] = camera->y();
    return jsonCamera;
}


QJsonDocument IssoObjectUtils::objectSchemeAsJson(IssoObject *object)
{
    if (!object)
        return QJsonDocument();
    QJsonObject jsonRoot;
    jsonRoot["unixTime"] = QDateTime::currentSecsSinceEpoch();
    jsonRoot["object"]   = makeObjectScheme(object);
    //
    // вернуть документ JSON
    return QJsonDocument(jsonRoot);
}


QString IssoObjectUtils::objectSchemeAsString(IssoObject *object)
{
    // вернуть документ JSON в виде строки
    return QString(objectSchemeAsJson(object).toJson());
}
