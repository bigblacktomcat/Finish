#include "addguardedobjectdialog.h"
#include "issoconfigutils.h"


const QString IssoConfigUtils::AUTORUN_FILENAME = ".autostart";


int IssoConfigUtils::createConfig(IssoDataBase *db)
{
    if (!db)
        return -1;
    AddGuardedObjectDialog dlg(db->readObjectIds());
    int result = dlg.exec();
    if (result != QDialog::Accepted)
        return -1;

    // создать охраняемый объект
//    IssoObject* obj = new IssoObject(dlg.objectId());
    IssoObject* obj = new IssoObject();
    // задать карточку охраняемого объекта
    obj->setObjectCard(dlg.objectCard());
    // задать стартовые IP и MAC
    obj->setCurrentIp(dlg.startIp());
    obj->setCurrentMac(dlg.startMac());
    // задать IP сервера
    obj->setServerIp(dlg.serverIp());
    //
    // проход по всем виджетам инициализации зданий
    for(int iBuild = 1; iBuild <= dlg.buildCount(); iBuild++)
    {
        // получить виджет инициализации здания
        BuildInitWidget* wgt = dlg.findWidgetByNumber(iBuild);
        if (!wgt)
            continue;
        // добавить здание в систему
        obj->addBuilding(wgt->buildNumber(),
                         wgt->lowerFloorNumber(),
                         wgt->floorCount(),
                         wgt->base64ImageMap(),
                         wgt->moduleCountMap(),
                         wgt->checkableParams());
    }
    // сохранить конфигурацию охраняемого объекта в файл
    QByteArray configData = obj->save(QString("%1.issocfg").arg(obj->displayName()));
    //
//    // удалить таблицу фотоснимков
//    db->dropPhotosTable();
//    // удалить таблицу событий
//    db->dropEventsTable();
//    // удалить таблицу объектов
//    db->dropObjectsTable();
//    // создать таблицу объектов
//    db->createObjectsTable();
//    // создать таблицу событий
//    db->createEventsTable();
//    // создать таблицу фотоснимков
//    db->createPhotosTable();
    //
    // рассчитать сводные данные
    obj->calcTotalSummary();

    // добавить охраняемый объект в БД
    db->insertObject(obj->objectCard().id(),
                     obj->objectCard().city(),
                     obj->objectCard().region(),
                     obj->objectCard().street(),
                     obj->objectCard().building(),
                     obj->objectCard().contactName(),
                     obj->objectCard().phone(),
                     obj->totalModules(),
                     obj->totalMs(),
                     obj->totalChannels(),
                     configData);
//    db->insertObject(obj->id(),
//                     obj->displayName(),
//                     obj->objectCard().address(),
//                     obj->objectCard().contactName(),
//                     obj->objectCard().phone(),
//                     obj->totalModules(),
//                     obj->totalMs(),
//                     obj->totalChannels(),
//                     configData);

    // уничтожить объект
    delete obj;
    // вернуть успех
    return dlg.objectId();
}
//int IssoConfigUtils::createConfig(IssoDataBase *db)
//{
//    if (!db)
//        return -1;
//    AddGuardedObjectDialog dlg;
//    int result = dlg.exec();
//    if (result != QDialog::Accepted)
//        return -1;

//    // создать охраняемый объект
////    IssoObject* obj = new IssoObject(dlg.objectId());
//    IssoObject* obj = new IssoObject();
//    // задать карточку охраняемого объекта
//    obj->setObjectCard(dlg.objectCard());
//    // задать стартовые IP и MAC
//    obj->setCurrentIp(dlg.startIp());
//    obj->setCurrentMac(dlg.startMac());
//    // задать IP сервера
//    obj->setServerIp(dlg.serverIp());
//    //
//    // проход по всем виджетам инициализации зданий
//    for(int iBuild = 1; iBuild <= dlg.buildCount(); iBuild++)
//    {
//        // получить виджет инициализации здания
//        BuildInitWidget* wgt = dlg.findWidgetByNumber(iBuild);
//        if (!wgt)
//            continue;
//        // добавить здание в систему
//        obj->addBuilding(wgt->buildNumber(),
//                         wgt->lowerFloorNumber(),
//                         wgt->floorCount(),
//                         wgt->base64ImageMap(),
//                         wgt->moduleCountMap(),
//                         wgt->checkableParams());
//    }
//    // сохранить конфигурацию охраняемого объекта в файл
//    QByteArray configData = obj->save(QString("%1.issocfg").arg(obj->displayName()));
//    //
//    // удалить таблицу фотоснимков
//    db->dropPhotosTable();
//    // удалить таблицу событий
//    db->dropEventsTable();
//    // удалить таблицу объектов
//    db->dropObjectsTable();
//    // создать таблицу объектов
//    db->createObjectsTable();
//    // создать таблицу событий
//    db->createEventsTable();
//    // создать таблицу фотоснимков
//    db->createPhotosTable();
//    //
//    // рассчитать сводные данные
//    obj->calcTotalSummary();

//    // добавить охраняемый объект в БД
//    db->insertObject(obj->objectCard().id(),
//                     obj->objectCard().city(),
//                     obj->objectCard().region(),
//                     obj->objectCard().street(),
//                     obj->objectCard().building(),
//                     obj->objectCard().contactName(),
//                     obj->objectCard().phone(),
//                     obj->totalModules(),
//                     obj->totalMs(),
//                     obj->totalChannels(),
//                     configData);
////    db->insertObject(obj->id(),
////                     obj->displayName(),
////                     obj->objectCard().address(),
////                     obj->objectCard().contactName(),
////                     obj->objectCard().phone(),
////                     obj->totalModules(),
////                     obj->totalMs(),
////                     obj->totalChannels(),
////                     configData);

//    // уничтожить объект
//    delete obj;
//    // вернуть успех
//    return dlg.objectId();
//}


bool IssoConfigUtils::insertConfig(IssoDataBase *db, const QByteArray &configData)
{
    if (!db)
        return false;
    // загрузить объект из массива
    IssoObject obj;
    obj.load(configData);

    // рассчитать сводные данные
    obj.calcTotalSummary();

    // добавить охраняемый объект в БД
    bool res = db->insertObject(obj.objectCard().id(),
                                obj.objectCard().city(),
                                obj.objectCard().region(),
                                obj.objectCard().street(),
                                obj.objectCard().building(),
                                obj.objectCard().contactName(),
                                obj.objectCard().phone(),
                                obj.totalModules(),
                                obj.totalMs(),
                                obj.totalChannels(),
                                configData);
//    bool res = db->insertObject(obj.id(),
//                                obj.displayName(),
////                                obj.objectCard().name(),
//                                obj.objectCard().address(),
//                                obj.objectCard().contactName(),
//                                obj.objectCard().phone(),
//                                obj.totalModules(),
//                                obj.totalMs(),
//                                obj.totalChannels(),
//                                configData);
    return res;
}


bool IssoConfigUtils::deleteConfig(IssoDataBase *db, int objectId)
{
    return (db ? db->deleteObject(objectId) : false);
}


IssoObject *IssoConfigUtils::loadObject(IssoDataBase *db, int objectId)
{
    if (!db)
        return nullptr;
    // создать охраняемый объект
//    IssoObject* obj = new IssoObject(objectId);
    IssoObject* obj = new IssoObject();
    // получить конфигурацию объекта из БД
    QByteArray configData = db->readObjectConfig(objectId);
    // загрузить конфигурацию объекта из БД
    obj->load(configData);
    // вернуть загруженный объект
    return obj;
}


QList<int> IssoConfigUtils::getObjectIds(IssoDataBase *db)
{
    return (db ? db->readObjectIds() : QList<int>());
}


int IssoConfigUtils::getFirstObjectId(IssoDataBase *db)
{
//    return db->readFirstObjectId();
    if (!db)
        return -1;
    // прочитать список ID объектов из БД
    auto ids = db->readObjectIds();
    // вернуть -1, если список пуст, иначе первый ID
    return (ids.isEmpty() ? -1 : ids.first());
}


QMap<int, QString> IssoConfigUtils::getObjectNames(IssoDataBase *db)
{
//    return db->readObjectNames();
    return (db ? db->readObjectNames() : QMap<int, QString>());
}


bool IssoConfigUtils::updateConfig(IssoDataBase *db, IssoObject *obj)
{
    if (!db)
        return false;
    // сохранить конфигурацию охраняемого объекта в файл
    QByteArray configData = obj->save(QString("%1.issocfg").arg(obj->displayName()));
    // обновить конфигурацию объекта
    return db->updateObjectConfig(obj->id(), configData);
}


bool IssoConfigUtils::objectExists(IssoDataBase *db, int objectId)
{
    if (!db)
        return false;
    // прочитать список ID объектов из БД
    auto ids = db->readObjectIds();
    // определить содержится ли заданный ID в БД
    return ids.contains(objectId);
}


void IssoConfigUtils::createAutorunFile()
{
    QFile file(AUTORUN_FILENAME);
    file.open(QFile::ReadWrite);
}


void IssoConfigUtils::deleteAutorunFile()
{
    QFile file(AUTORUN_FILENAME);
    if (file.exists())
        file.remove();
}


bool IssoConfigUtils::needAutorun()
{
    return QFile::exists(AUTORUN_FILENAME);
}
