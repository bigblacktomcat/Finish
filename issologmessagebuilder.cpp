#include "issologmessagebuilder.h"



IssoLogMessageBuilder::IssoLogMessageBuilder(const IssoObject *obj,
                                             IssoEventType eventType,
                                             const QString &dateTime)
{
    m_json["eventType"] = eventType;
    m_json["dateTime"]  = dateTime;
    m_json["object"]    = createObjectData(obj);
}


void IssoLogMessageBuilder::addModuleData(const IssoModule *module)
{
    m_json["module"] = createModuleData(module);
}


void IssoLogMessageBuilder::addSensorData(const QString &sensorName,
                                          const QString &sensorState,
                                          int extValue,
                                          const QString& location)
{
    // сформировать данные датчика
    QJsonObject sensorObj;
    sensorObj["sensorName"]  = sensorName;
    sensorObj["sensorState"] = sensorState;
    sensorObj["extValue"]    = extValue;
    sensorObj["location"]    = location;
    // добавить в массив датчиков
    QJsonArray sensors;
    if (m_json.contains("sensors") && m_json["sensors"].isArray())
    {
        sensors = m_json["sensors"].toArray();
    }
    else
    {
        sensors = QJsonArray();
    }
    sensors.append(sensorObj);
    m_json["sensors"] = sensors;
}


void IssoLogMessageBuilder::addPhotoData(const QString &sensorName,
                                         int cameraId, int cameraIp,
                                         const QString &frameData)
{
    // сформировать данные датчика
    QJsonObject photoObj;
    photoObj["sensorName"]  = sensorName;
    photoObj["cameraId"]    = cameraId;
    photoObj["cameraIp"]    = cameraIp;
    photoObj["frameData"]   = frameData;
    // добавить в корневой объект
    m_json["photo"] = photoObj;
}


QJsonObject IssoLogMessageBuilder::createObjectData(const IssoObject *obj)
{
    if (!obj)
        return QJsonObject();
    // сформировать данные охраняемого объекта
    QJsonObject objData;
    // общие данные объекта
    objData["objectId"]    = obj->id();
    objData["objectName"]  = obj->displayName();    //obj->objectCard().name();
    objData["objectState"] = IssoCommonData::stringByModuleState(obj->state());
    objData["address"]     = obj->objectCard().address();
    objData["contactName"] = obj->objectCard().contactName();
    objData["phone"]       = obj->objectCard().phone();
    // получить сводные данные о тревогах
    int alarmModules, alarmMs, alarmChannels;
    obj->calcAlarmSummary(alarmModules, alarmMs, alarmChannels);
    // сводные данные о тревогах
    objData["totalModules"]      = obj->totalModules();
    objData["alarmModules"]      = alarmModules;
    objData["totalMultisensors"] = obj->totalMs();
    objData["alarmMultisensors"] = alarmMs;
    objData["totalChannels"]     = obj->totalChannels();
    objData["alarmChannels"]     = alarmChannels;
    //
    // вернуть объект JSON
    return objData;
}


QJsonObject IssoLogMessageBuilder::createModuleData(const IssoModule *module)
{
    if (!module)
        return QJsonObject();
    // сформировать данные модуля
    QJsonObject moduleData;
    // данные модуля
    moduleData["moduleName"] = module->displayName();
    moduleData["buildNumber"] = module->buildNum();
    moduleData["floorNumber"] = module->floorNum();
    moduleData["moduleState"] = IssoCommonData::stringByModuleState(module->moduleState());
    // вернуть объект JSON
    return moduleData;
}


QString IssoLogMessageBuilder::getMessage()
{
    QJsonDocument doc(m_json);
    return QString(doc.toJson());
}
