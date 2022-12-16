#include "issoobject.h"


IssoObject::IssoObject(QObject *parent) /*: IssoObject(0, parent)*/
    : QObject(parent),
      m_currentUniqueId(0),
      m_currentIp(IssoConst::DEFAULT_MODULE_IP),
      m_currentMac(IssoConst::DEFAULT_MAC)
{
}


//IssoObject::IssoObject(int id, QObject *parent)
//    : QObject(parent),
////      m_id(id),
//      m_currentUniqueId(0),
//      m_currentIp(IssoConst::DEFAULT_MODULE_IP),
//      m_currentMac(IssoConst::DEFAULT_MAC)
//{
//    m_objectCard.setId(id);
//}


IssoObject::~IssoObject()
{
    qDebug() << "~IssoObject" << endl;

    // обнулить указатели
    mp_floor = nullptr;
    mp_building = nullptr;
    // уничтожить все здания
    // (модули удаляются с этажей,
    // далее уничтожаются этажи, затем здания)
    qDebug() << "destroyBuildings();" << endl;
    destroyBuildings();
    // уничтожить все модули
    qDebug() << "destroyModules();" << endl;
    destroyModules();
    // уничтожить все камеры
    qDebug() << "destroyCameras();" << endl;
    destroyCameras();
    // удалить все конфиги модулей
    qDebug() << "removeAllModuleConfigs();" << endl;
    removeAllModuleConfigs();
    // удалить все конфиги шлейфов
    qDebug() << "removeAllChainConfigs();" << endl;
    removeAllChainConfigs();
    // уничтожить все сценарии
    qDebug() << "destroyAllScenarios();" << endl;
    destroyAllScenarios();
}


void IssoObject::load(QDataStream &stream)
{
//    // ID охраняемого объекта
//    stream >> m_id;
    // карточка охраняемого объекта
    m_objectCard.loadFromStream(stream);
    //
    // параметры, необходимые для добавления новых модулей
    stream >> m_currentUniqueId;
    stream >> m_currentIp;
    stream >> m_currentMac;
    stream >> m_serverIp;
    //
    // список нераспределенных модулей
    QList<IssoModule*> unlocatedList;
    // уничтожить здания
    destroyBuildings();
    // уничтожить все модули
    destroyModules();
    //
    // здания:
    //
    // количество зданий
    int buildCount;
    stream >> buildCount;
    // загрузить каждое здание
    for (int i = 0; i < buildCount; ++i)
    {
        // создать здание
        IssoBuilding* build = new IssoBuilding();
        // загрузить здание из потока
        build->loadFromStream(stream);
        // добавить в список зданий
        m_buildings << build;
    }
    //
    // модули:
    //
    // количество модулей
    int cnt;
    stream >> cnt;
    // загрузить каждый модуль
    for (int i = 0; i < cnt; ++i)
    {
        // создать модуль
        IssoModule* module = new IssoModule();
        // загрузить модуль
        module->loadFromStream(stream);
        // найти этаж
        IssoFloorPlan* floor = this->findFloor(module->buildNum(),
                                               module->floorNum());
        // добавить модуль на этаж
        if (floor)
            floor->addModule(module);
        else
            // иначе добавить в список неразмщенных
            unlocatedList << module;
    }
    // отсортировать нераспределенные модули
    std::sort(unlocatedList.begin(), unlocatedList.end(),
              [](const IssoModule* a, const IssoModule* b) ->
                            bool { return a->displayId() < b->displayId(); });
    // добавить в систему нераспределенные модули
    addModules(unlocatedList);
    // добавить в систему модули каждого этажа каждого здания
    foreach (IssoBuilding* build, m_buildings)
    {
        foreach (IssoFloorPlan* floor, build->floors())
        {
            // добавить в систему модули, распределенные по этажу
            addModules(floor->modules());
        }
    }
    //
    // камеры:
    //
    // количество камер
    int cameraCount;
    stream >> cameraCount;
    // загрузить каждую камеру
    for (int i = 0; i < cameraCount; ++i)
    {
        // создать камеру
        IssoCamera* camera = new IssoCamera();
        // загрузить камеру из потока
        camera->loadFromStream(stream);
        // добавить в список камер объекта
        addCamera(camera);
    }
    //
    // конфигурации датчиков / шлейфов
    //
    loadChainConfigs(stream);
    //
    // конфигурации модулей
    //
    loadModuleConfigs(stream);
    //
    // классы реле
    //
    stream >> m_relayClasses;
    //
    // сценарии
    //
    loadScenarios(stream);
    //
    // сценарии, назныченные кнопкам панели управления системой
    //
    stream >> m_buttonScenarioLinks;
}


void IssoObject::save(QDataStream &stream)
{
//    // ID охраняемого объекта
//    stream << m_id;
    // карточка охраняемого объекта
    m_objectCard.saveToStream(stream);
    //
    // параметры, необходимые для добавления новых модулей
    stream << m_currentUniqueId;
    stream << m_currentIp;
    stream << m_currentMac;
    stream << m_serverIp;
    //
    // здания:
    //
    // количество зданий
    int buildCount = m_buildings.size();
    stream << buildCount;
    // сохранить каждое здание
    foreach (IssoBuilding* build, m_buildings)
    {
        // сохранить здание в поток
        build->saveToStream(stream);
    }
    //
    // модули:
    //
    // количество модулей
    int moduleCount = m_modules.size();
    stream << moduleCount;
    // сохранить каждый модуль
    foreach (IssoModule* module, m_modules)
    {
        // сохранить модуль в поток
        module->saveToStream(stream);
    }
    //
    // камеры:
    //
    // количество камер
    int cameraCount = m_cameras.size();
    stream << cameraCount;
    // сохранить каждую камеру
    foreach (IssoCamera* camera, m_cameras)
        camera->saveToStream(stream);
    //
    // конфигурации датчиков / шлейфов
    //
    saveChainConfigs(stream);
    //
    // конфигурации модулей
    //
    saveModuleConfigs(stream);
    //
    // классы реле
    //
    stream << m_relayClasses;
    //
    // сценарии:
    //
    saveScenarios(stream);
    //
    // сценарии, назныченные кнопкам панели управления системой
    //
    stream << m_buttonScenarioLinks;
}


bool IssoObject::load(const QString &fileName)
{
    QFile file(fileName);
    // если файл не найден, выход
    if (!file.exists())
        return false;
    // открыть файл для чтения
    if (file.open(QIODevice::ReadOnly))
    {
        QByteArray ba = file.readAll();
        // создать поток для чтения
        QDataStream out(ba);
        // загрузить конфигурацию из потока
        load(out);
        return true;
    }
    else
        return false;
}


QByteArray IssoObject::save(const QString &fileName)
{
    QByteArray configData;
    // создать файл
    QFile file(fileName);
    // открыть файл для записи
    if (file.open(QIODevice::ReadWrite))
    {
        // создать поток для записи
        QDataStream out(&file);
        // сохранить здания в поток
        save(out);
        //
        // получить данные конфига
        out.device()->reset();
        configData = out.device()->readAll();
    }
    file.close();
    // вернуть массив данных конфигурации
    return configData;
}


void IssoObject::load(const QByteArray &configData)
{
    QByteArray ba = configData;
    // создать поток для чтения
    QDataStream out(ba);
    // загрузить конфигурацию из потока
    load(out);
}


IssoBuilding *IssoObject::findBuildingByName(const QString &name)
{
    foreach (IssoBuilding* build, m_buildings)
    {
        if (build->displayName() == name)
            return build;
    }
    return nullptr;
}


IssoBuilding *IssoObject::findBuildingByNumber(int num)
{
    foreach (IssoBuilding* build, m_buildings)
    {
        if (build->number() == num)
            return build;
    }
    return nullptr;
}


IssoModule *IssoObject::findModuleByDisplayName(const QString &displayName, bool locatedOnly)
{
    // модулей с пустым именем быть не может
    if (displayName.isEmpty())
        return nullptr;

    foreach (IssoModule* module, m_modules)
    {
        if (module->displayName() == displayName)
        {
//            return module;
            // если ищем среди всех модулей, не проверять признак распределения,
            // либо, если среди распределенных, проверить признак распределения
            if (!locatedOnly || module->isLocated())
                return module;
        }
    }
    return nullptr;
}


IssoModule *IssoObject::findModuleById(int uniqueId, bool locatedOnly)
{
    foreach (IssoModule* module, m_modules)
    {
        if (module->uniqueId() == uniqueId)
        {
//            return module;
            // если ищем среди всех модулей, не проверять признак распределения,
            // либо, если среди распределенных, проверить признак распределения
            if (!locatedOnly || module->isLocated())
                return module;
        }
    }
    return nullptr;
}


IssoModule *IssoObject::findModuleByAddress(const QHostAddress &ip, bool locatedOnly)
{
    foreach (IssoModule* module, m_modules)
    {
        // если IP совпадает
        if (ip.isEqual(module->ip(), QHostAddress::ConvertV4MappedToIPv4))
        {
//            // если искать среди распределенных модулей, то номера здания и этажа != 0
//            if (!locatedOnly || ((module->buildNum() != 0) && (module->floorNum() != 0)))
            // если ищем среди всех модулей, не проверять признак распределения,
            // либо, если среди распределенных, проверить признак распределения
            if (!locatedOnly || module->isLocated())
                return module;
        }
    }
    return nullptr;
}


IssoFloorPlan *IssoObject::findFloor(int buildNum, int floorNum)
{
    // найти здание по номеру
    IssoBuilding* build = findBuildingByNumber(buildNum);
    if (!build)
        return nullptr;
    // найти этаж в здании по номеру
    IssoFloorPlan* floor = build->findFloorByNumber(floorNum);
    return floor;
}


IssoCamera *IssoObject::findCameraById(int id)
{
    foreach (IssoCamera* camera, m_cameras)
    {
        if (camera->id() == id)
            return camera;
    }
    return nullptr;
}


IssoCamera *IssoObject::findCameraByIp(const QHostAddress &ip)
{
    foreach (IssoCamera* camera, m_cameras)
    {
        if (camera->ip() == ip)
            return camera;
    }
    return nullptr;
}


bool IssoObject::changeModuleLocation(IssoModule *module, int buildNum, int floorNum)
{
    // найти этаж, на котором расположен модуль
    IssoFloorPlan* oldFloor = this->findFloor(module->buildNum(), module->floorNum());
    // найти этаж, на который перемещается модуль
    IssoFloorPlan* newFloor = this->findFloor(buildNum, floorNum);
    // если найден старый этаж
    if (oldFloor)
    {
        // удалить модуль из списка старого этажа
        oldFloor->removeModule(module);
    }
    // если найден новый этаж
    if (newFloor)
    {
        // определить свободный порядковый номер на этаже
        int newDisplayId = newFloor->nextAvailableModuleDisplayId();
        // если свободных номеров нет, не добавлять модуль
        if (newDisplayId != -1)
        {
            // задать модулю новый порядковый номер
            module->setDisplayId(newDisplayId);
            // добавить модуль в список нового этажа
            newFloor->addModule(module);
        }
    }
    return true;
}


bool IssoObject::changeCameraLocation(IssoCamera *camera, int buildNum, int floorNum)
{
    // найти этаж, на котором расположена камера
    if (auto oldFloor = findFloor(camera->buildNum(), camera->floorNum()))
    {
        // удалить камеру из списка старого этажа
        oldFloor->removeCamera(camera);
    }
    // найти этаж, на который перемещается камера
    if (auto newFloor = findFloor(buildNum, floorNum))
    {
        // добавить камеру в список нового этажа
        newFloor->addCamera(camera);
    }
    // вернуть успех
    return true;
}


QList<IssoModule *> IssoObject::modules() const
{
    return m_modules;
}


void IssoObject::addModule(IssoModule *module)
{
    // добавить модуль в список
    m_modules.append(module);

//    // сортировать модули по ip
//    std::sort(m_modules.begin(), m_modules.end(),
//          [](const IssoModule* a, const IssoModule* b) ->
//                        bool { return a->ip().toIPv4Address() < b->ip().toIPv4Address(); });
}


QString IssoObject::addModule(int displayId, const QHostAddress &ip,
                              quint16 port, int uniqueId)
{
    // если модуль с данным id уже существует
    if (this->findModuleById(uniqueId))
    {
        // вернуть пустую строку
        return "";
    }
    // создать модуль
    IssoModule* module = new IssoModule(uniqueId, displayId, 0, 0, ip, port, nextMac(), m_serverIp);
    // добавить модуль
    this->addModule(module);
    // вернуть успех
    return module->displayName();
}


void IssoObject::addModules(const QList<IssoModule *> &modules)
{
    foreach (IssoModule* module, modules)
    {
        addModule(module);
    }
}


void IssoObject::removeModule(IssoModule *module)
{
    if (!module)
        return;
    // удалить из списка модулей системы
    m_modules.removeOne(module);
    // найти здание, в котором расположен модуль
    IssoBuilding* build = findBuildingByNumber(module->buildNum());
    // если здание найдено
    if (build)
    {
        // найти этаж, на котором расположен модуль
        IssoFloorPlan* floor = build->findFloorByNumber(module->floorNum());
        // если этаж найден, удалить модуль из списка
        if (floor)
            floor->removeModule(module);
    }
    // уничтожить модуль
    delete module;
}


bool IssoObject::removeModule(const QString &moduleName)
{
    // найти модуль по отображаемому имени
    IssoModule* module = this->findModuleByDisplayName(moduleName);
    // если найден
    if (module)
    {
        // удалить модуль из системы
        this->removeModule(module);
        // вернуть успех
        return true;
    }
    else
        // вернуть неуспех
        return false;
}


bool IssoObject::addFloorToBuilding(const QString &buildName, int floorNum,
                                    const QPixmap &background)
{
    IssoBuilding* build = findBuildingByName(buildName);
    if (!build)
        return false;
    // добавить новый этаж в здание
    return build->addFloor(floorNum, background);
}


bool IssoObject::removeFloorFromBuilding(const QString &buildName, const QString &floorName)
{
    IssoBuilding* build = findBuildingByName(buildName);
    if (!build)
        return false;
    // найти этаж по имени
    IssoFloorPlan* floor = build->findFloorByName(floorName);
    if (!floor)
        return false;
    // убрать все модули с этажа
    // (переместить в список не размещенных)
    floor->removeAllModules();
    // удалить этаж из здания
    return build->removeFloor(floor);
}


QString IssoObject::addBuilding(const QString &name, int number)
{
    // создать здание
    IssoBuilding* build = new IssoBuilding(number, name);
    // если существует здание с таким же отображаемым именем или номером
    if (this->findBuildingByName(build->displayName()) ||
            this->findBuildingByNumber(build->number()))
    {
        // уничтожить новое здание
        delete build;
        // вернуть пустую строку
        return "";
    }
    // добавить здание в список зданий
    m_buildings << build;
    return build->displayName();
}


void IssoObject::addBuilding(int buildNum, int lowerFloorNum, int floorCount,
                             const QMap<QString, QString> &imageMap,
                             const QMap<int, int> &moduleCountMap,
                             const QSet<IssoParamId> &checkableParams)
{
    // создать здание
    IssoBuilding* build = new IssoBuilding(buildNum, tr("Здание"));
    // добавить в список зданий объекта
    m_buildings << build;
    // число этажей, которые необходимо добавить в здание
    int floorsLeft = floorCount;
    // добавить в здание этажи, нумеруя с минимального номера
    for (int iFloor = lowerFloorNum; floorsLeft > 0; ++iFloor)
    {
        // нулевого этажа быть не может
        if (iFloor == 0)
            continue;
        // создать этаж
        IssoFloorPlan* floor = new IssoFloorPlan(iFloor, build->number());
        //
        // получить графплан этажа по имени
        QString base64Img = imageMap.value(floor->displayName());
        QImage img = IssoCommonData::convertBase64ToImage(base64Img);
        // задать план этажа
        floor->setBackground(img);
        //
        // добавить в список этажей здания
        build->addFloor(floor);
        // получить количество модулей для данного этажа по его номеру
        int moduleCount = moduleCountMap.value(floor->number());
        if (moduleCount == -1)
            continue;
        // добавить на этаж модули
        for (int iModule = 1; iModule <= moduleCount; ++iModule)
        {
            // получить следующую позицию для добавления модуля на этаж
            QPointF modulePos = floor->nextModulePosition();
            // создать модуль
            IssoModule* module = new IssoModule(nextUniqueId(),
                                                iModule,
                                                modulePos.x(),
                                                modulePos.y(),
                                                nextIp(),
                                                IssoConst::SEND_PORT,
                                                nextMac(),
                                                m_serverIp);
            // добавить модуль на этаж
            floor->addModule(module);
            // добавить модуль в систему
            m_modules << module;
        }
        // уменьшить число недобавленных этажей
        --floorsLeft;
    }
}


void IssoObject::removeBuilding(const QString &name)
{
    // найти здание по отображаемому имени
    IssoBuilding* build = findBuildingByName(name);
    if (!build)
        return;
    // удалить здание из списка
    m_buildings.removeOne(build);
    // уничтожить здание
    delete build;
}


void IssoObject::addCamera(IssoCamera *camera)
{
    if (!camera)
        return;
    // найти этаж
    IssoFloorPlan* floor = findFloor(camera->buildNum(),
                                     camera->floorNum());
    // добавить в список камер объекта
    m_cameras << camera;
    // отсортировать список
    std::sort(m_cameras.begin(), m_cameras.end(),
              [](const IssoCamera* a, const IssoCamera* b) ->
              bool { return a->id() < b->id(); });
    // добавить камеру на этаж
    if (floor)
        floor->addCamera(camera);
}


void IssoObject::removeCamera(IssoCamera *camera)
{
    if (!camera)
        return;
    //
    // найти этаж
    IssoFloorPlan* floor = findFloor(camera->buildNum(),
                                     camera->floorNum());
    // удалить камеру с этажа
    if (floor)
        floor->removeCamera(camera);
    // удалить из списка камер объекта
    m_cameras.removeAll(camera);
    // уничтожить камеру
    delete camera;
}


IssoCamera *IssoObject::addCamera(int id, const QHostAddress &ip, quint16 port,
                                  const QString& password,
                                  int buildNum, int floorNum)
{
    // если камера с данным id уже существует
    if (findCameraById(id))
    {
        // вернуть пустую строку
        return nullptr;
    }
    // создать камеру
    IssoCamera* camera = new IssoCamera(id, ip, port, password, buildNum, floorNum);
    // добавить в объект
    addCamera(camera);
    // вернуть указатель на новую камеру
    return camera;
}


bool IssoObject::removeCamera(int id)
{
    // найти камеру по ID
    IssoCamera* camera = findCameraById(id);
    if (!camera)
    {
        return false;
    }
    // удалить камеру из объекта
    removeCamera(camera);
    return true;
}


IssoModuleState IssoObject::state() const
{
    IssoModuleState result = ACTIVE;
    // проход по размещенным модулям
    foreach (IssoModule* module, locatedModules())
    {
        result = (module->moduleState() < result ? module->moduleState() : result);
    }
    return result;
}


void IssoObject::setGraphicItemsMovable(bool movable)
{
    // разрешить / запретить перемещение всех модулей,
    // распределенных по этажам
    foreach (IssoModule* module, locatedModules())
        module->setMovable(movable);
    // разрешить / запретить перемещение камеры
    foreach (IssoCamera* camera, m_cameras)
        camera->setMovable(movable);
}


QList<IssoModule *> IssoObject::locatedModules() const
{
    // сформировать список модулей
    QList<IssoModule*> modules;
    // добавить в список все модули всех зданий
    foreach (IssoBuilding* build, m_buildings)
        modules.append(build->modules());
    return modules;
}


QStringList IssoObject::unlocatedModuleNames() const
{
    QStringList list;
    foreach (IssoModule* module, m_modules)
    {
        // если номер здания равен 0, добавить имя в список
        if (module->buildNum() == 0)
            list << module->displayName();
    }
    return list;
}


QStringList IssoObject::moduleNames() const
{
    QStringList list;
    foreach (IssoModule* module, m_modules)
        list << module->displayName();
    return list;
}


QStringList IssoObject::buildingNames() const
{
    QStringList list;
    foreach (IssoBuilding* build, m_buildings)
        list << build->displayName();
    return list;
}


IssoModule *IssoObject::selectedModule()
{
    // если этаж не выбран, вернуть пустой указатель
    if (!mp_floor)
        return nullptr;
    // вернуть модуль
    return mp_floor->selectedModule();
}


void IssoObject::setCurrentIp(const QHostAddress &ip)
{
    m_currentIp = ip;
}


void IssoObject::setCurrentMac(const QByteArray &mac)
{
    m_currentMac = mac;
}


IssoObjectCard IssoObject::objectCard() const
{
    return m_objectCard;
}


void IssoObject::setObjectCard(const IssoObjectCard &objectCard)
{
    m_objectCard = objectCard;
}


void IssoObject::sortModules()
{
    // сортировать список
    std::sort(m_modules.begin(), m_modules.end(),
              [](const IssoModule* a, const IssoModule* b) ->
              bool { return a->displayName() < b->displayName(); });
}


QList<IssoModule *> IssoObject::filteredModules(int buildNum, int floorNum)
{
    QList<IssoModule*> list;
    // если здание не выбрано, вернуть все модули
    if (buildNum == 0)
    {
        list = m_modules;
    }
    // иначе найти здание
    else
    {
        IssoBuilding* build = findBuildingByNumber(buildNum);
        if (build)
        {
            // если этаж не выбран, вернуть все модули здания
            if (floorNum == 0)
            {
                list = build->modules();
            }
            // иначе вернуть модули этажа
            else
            {
                IssoFloorPlan* floor = build->findFloorByNumber(floorNum);
                if (floor)
                {
                    list = floor->modules();
                }
            }
        }
    }
    return list;
}


QList<IssoModule *> IssoObject::filteredModules(const QString &buildName,
                                                const QString &floorName)
{
    int buildNum, floorNum;
    // определить номер здания
    IssoBuilding* build = findBuildingByName(buildName);
    buildNum = (build ? build->number() : 0);
    // определить номер этажа
    if (build)
    {
        IssoFloorPlan* floor = build->findFloorByName(floorName);
        floorNum = (floor ? floor->number() : 0);
    }
    else
    {
        floorNum = 0;
    }
    // вернуть отфильтрованые модули
    return filteredModules(buildNum, floorNum);
}


QList<int> IssoObject::availableBuildNumbers()
{
    QSet<int> availableNumbers, busyNumbers;
    // сформировать набор возможных номеров зданий
    for (int i = 1; i <= 99; ++i)
        availableNumbers.insert(i);
    // сформировать набор занятых номеров
    foreach (IssoBuilding* build, m_buildings)
        busyNumbers.insert(build->number());
    // вычесть занятые номера
    availableNumbers.subtract(busyNumbers);
    // преобразовать в список
    QList<int> list = availableNumbers.toList();
    // отсортировать список
    std::sort(list.begin(), list.end());
    return list;
}


QList<int> IssoObject::availableFloorNumbers()
{
    QSet<int> availableNumbers;
    // сформировать набор возможных номеров этажей
    for (int i = -9; i <= 99; ++i)
    {
        if (i != 0)
            availableNumbers.insert(i);
    }
    // вычесть занятые номера
    if (currentBuilding())
        availableNumbers.subtract(currentBuilding()->floorNumbers());
    // преобразовать в список
    QList<int> list = availableNumbers.toList();
    // отсортировать список
    std::sort(list.begin(), list.end());
    return list;
}


QList<int> IssoObject::availableCameraIds()
{
    QSet<int> availableIds, busyIds;
    // сформировать набор возможных ID камер
    for (int i = 1; i <= 999; ++i)
        availableIds.insert(i);
    // сформировать набор занятых ID
    foreach (IssoCamera* camera, cameras())
        busyIds.insert(camera->id());
    // вычесть занятые ID
    availableIds.subtract(busyIds);
    // преобразовать в список
    QList<int> list = availableIds.toList();
    // отсортировать список
    std::sort(list.begin(), list.end());
    return list;
}


QList<IssoBuilding *> &IssoObject::buildings()
{
    return m_buildings;
}


IssoBuilding *IssoObject::currentBuilding()
{
    return mp_building;
}


void IssoObject::setCurrentBuilding(IssoBuilding *building)
{
    mp_building = building;
}


IssoFloorPlan *IssoObject::currentFloor()
{
    return mp_floor;
}


void IssoObject::setCurrentFloor(IssoFloorPlan *floor)
{
    mp_floor = floor;
}


int IssoObject::nextUniqueId()
{
    return ++m_currentUniqueId;
}


QHostAddress IssoObject::nextIp()
{
//    // запомнить текущий ip
//    QHostAddress ipToReturn = m_currentIp;
//    // преобразовать в int
//    quint32 curIpValue = m_currentIp.toIPv4Address();
//    // инкрементировать
//    curIpValue++;
//    // получить младший разряд
//    quint8 lsb = curIpValue & 0xFF;
//    // младший разряд не может быть равен 0, 1, 255
//    switch(lsb)
//    {
//        case 0:
//            curIpValue += 2;
//            break;

//        case 1:
//        case 255:
//            curIpValue++;
//            break;

//        default:
//            break;
//    }
//    // преобразовать из BigEndian
//    quint32 nextIpValue = curIpValue;
//    // преобразовать в адрес
//    m_currentIp = QHostAddress(nextIpValue);
//    // вернуть запомненный ip
//    return ipToReturn;


    // запомнить текущий ip
    QHostAddress ipToReturn = m_currentIp;
    // сохранить следующий адрес, как текущий
    m_currentIp = IssoCommonData::incrementIp(m_currentIp);
    // вернуть запомненный ip
    return ipToReturn;
}


QByteArray IssoObject::nextMac()
{
    // запомнить текущий MAC
    QByteArray macToReturn = m_currentMac;
    // инекрементировать текущий MAC
    QByteArray ba = m_currentMac;
    ba.prepend(2, 0x00);
    QDataStream stream(&ba, QIODevice::ReadWrite);
    quint64 macValue;
    stream >> macValue;
    macValue++;
    stream.device()->reset();
    stream << macValue;
    m_currentMac = ba.right(6);
    return macToReturn;
}


int IssoObject::id() const
{
//    return m_id;
    return objectCard().id();
}

void IssoObject::setId(int id)
{
//    m_id = id;
    m_objectCard.setId(id);
}


QList<IssoCamera *> IssoObject::cameras() const
{
    return m_cameras;
}


QMap<QString, IssoModuleConfig *> IssoObject::moduleConfigs() const
{
    return m_moduleConfigs;
}


void IssoObject::setModuleConfigs(const QMap<QString, IssoModuleConfig *> &moduleConfigs)
{
    m_moduleConfigs = moduleConfigs;
}


void IssoObject::insertModuleConfig(IssoModuleConfig *moduleCfg)
{
    m_moduleConfigs.insert(moduleCfg->name(), moduleCfg);
}


void IssoObject::removeModuleConfig(const QString &moduleCfgName)
{
    // получить конфигурацию модуля
    IssoModuleConfig* cfg = moduleConfig(moduleCfgName);
    // удалить из коллекции
    m_moduleConfigs.remove(moduleCfgName);
    // если конфигурация не пуста, уничтожить
    if (cfg)
        delete cfg;
}


void IssoObject::removeAllModuleConfigs()
{
    foreach (QString moduleCfgName, m_moduleConfigs.keys())
    {
        removeModuleConfig(moduleCfgName);
    }
}


IssoModuleConfig *IssoObject::moduleConfig(const QString &moduleCfgName)
{
    return m_moduleConfigs.value(moduleCfgName, nullptr);
}


QStringList IssoObject::moduleConfigNames() const
{
    return m_moduleConfigs.keys();
}


void IssoObject::saveModuleConfigs(QDataStream &stream)
{
    // записать количество конфигураций модулей
    stream << m_moduleConfigs.size();
    // записать в поток каждую конфигурацию модуля
    foreach (IssoModuleConfig* cfg, m_moduleConfigs.values())
    {
        stream << cfg;
    }
}


bool IssoObject::loadModuleConfigs(QDataStream &stream)
{
    // считать количество конфигураций модулей
    int cfgCount;
    stream >> cfgCount;
    // считать из потока каждую конфигурацию модуля
    while (cfgCount > 0)
    {
        // создать пустую конфигурацию
        IssoModuleConfig* cfg = new IssoModuleConfig();
        // загрузить из потока
        stream >> cfg;
        // добавить в карту
        m_moduleConfigs.insert(cfg->name(), cfg);
        // уменьшить счетчик
        --cfgCount;
    }
    return true;
}


QMap<QString, IssoChainConfig *> IssoObject::chainConfigs() const
{
    return m_chainConfigs;
}


void IssoObject::setChainConfigs(const QMap<QString, IssoChainConfig *> &chainConfigs)
{
    m_chainConfigs = chainConfigs;
}


void IssoObject::insertChainConfig(IssoChainConfig *chainCfg)
{
    m_chainConfigs.insert(chainCfg->name(), chainCfg);
}


void IssoObject::removeChainConfig(const QString &chainCfgName)
{
    // получить конфигурацию шлейфа
    IssoChainConfig* cfg = chainConfig(chainCfgName);
    // удалить из коллекции
    m_chainConfigs.remove(chainCfgName);
    // если конфигурация не пуста, уничтожить
    if (cfg)
        delete cfg;
}

void IssoObject::removeAllChainConfigs()
{
    foreach (QString chainCfgName, m_chainConfigs.keys())
    {
        removeChainConfig(chainCfgName);
    }
}


IssoChainConfig *IssoObject::chainConfig(const QString &chainCfgName)
{
    return m_chainConfigs.value(chainCfgName, nullptr);
}


void IssoObject::saveChainConfigs(QDataStream &stream)
{
    // записать количество конфигураций шлейфов
    stream << m_chainConfigs.size();
    // записать в поток каждую конфигурацию шлейфа
    foreach (IssoChainConfig* cfg, m_chainConfigs.values())
    {
        cfg->saveToStream(stream);
    }
}


bool IssoObject::loadChainConfigs(QDataStream &stream)
{
    // считать количество конфигураций шлейфов
    int cfgCount;
    stream >> cfgCount;
    // считать из потока каждую конфигурацию шлейфа
    while (cfgCount > 0)
    {
        // считать тип шлейфа
        IssoChainType chainType;
        stream >> chainType;
        // сместить позицию потока назад на размер типа шлейфа
        stream.device()->seek(stream.device()->pos() - sizeof(chainType));

        // указатель на конфигурацию
        IssoChainConfig* cfg = IssoChainConfigFactory::make(chainType);
        if (!cfg)
            return false;
        // загрузить из потока
        cfg->loadFromStream(stream);
        // добавить в карту
        m_chainConfigs.insert(cfg->name(), cfg);
        // уменьшить счетчик
        --cfgCount;
    }
    return true;
}


void IssoObject::applyModuleConfigToModule(const QString& moduleName,
                                           const QString& moduleCfgName)
{
    // найти модуль по имени
    IssoModule* module = findModuleByDisplayName(moduleName);
    if (!module)
        return;
    //
    // сбросить состояния параметров модуля модуля
    module->clearStateParams();
    //
    // найти конфиг модуля по имени
    IssoModuleConfig* moduleCfg = moduleConfig(moduleCfgName);
    // если конфиг не найден
    if (!moduleCfg)
    {
        // сбросить имя конфига модуля
        module->setModuleConfigName("");
        // удалить графические объекты-шлейфы
        module->removeAllSensorChains();
        return;
    }
    // задать модулю новое имя конфига
    module->setModuleConfigName(moduleCfgName);
    //
    // сформировать коллекцию конфигов шлейфов модуля
    QMap<IssoParamId, IssoChainConfig*> chainCfgs;
    // проход по ссылкам на конфиги шлейфов
    foreach (IssoChainConfigLink link, moduleCfg->chainConfigLinks().values())
    {
        // если ссылка неактивна, далее
        if (!link.enabled())
            continue;
        // найти конфиг шлейфа по имени
        IssoChainConfig* cfg = chainConfig(link.chainCfgName());
        if (!cfg)
            continue;
        // добавить конфиг шлейфа в результирующую коллекцию
        chainCfgs.insert(link.paramId(), cfg);
    }
    // обновить отображаемые шлейфы модуля
    module->updateChains(chainCfgs);
}


bool IssoObject::moduleParamEnabled(const QString &moduleCfgName,
                                          IssoParamId paramId)
{
    // найти конфиг модуля по имени
    IssoModuleConfig* moduleCfg = this->moduleConfig(moduleCfgName);
    // параметр активизирован, если конфиг модуля найден и подключен
    return ((moduleCfg) && (moduleCfg->paramEnabled(paramId)));
}


IssoChainConfig *IssoObject::moduleChainConfig(const QString &moduleCfgName,
                                               IssoParamId paramId)
{
    // найти конфиг модуля по имени
    IssoModuleConfig* moduleCfg = this->moduleConfig(moduleCfgName);
    if (!moduleCfg)
        return nullptr;
    // получить имя конфига шлейфа по ID шлейфа
    QString chainCfgName = moduleCfg->chainConfigName(paramId);
    // получить конфиг шлейфа по имени
    return this->chainConfig(chainCfgName);
}


IssoAddressChainConfig *IssoObject::moduleAddressChainConfig(const QString &moduleCfgName)
{
    // получить конфиг адресного шлейфа модуля
    IssoChainConfig* chainCfg = moduleChainConfig(moduleCfgName, MULTISENSOR);
    // привести к конфигу адресного шлейфа и вернуть
    return dynamic_cast<IssoAddressChainConfig*>(chainCfg);
}


IssoMultiSensorConfig *IssoObject::findMultiSensorCfgByModuleCfgName(const QString &moduleCfgName,
                                                           int multiSensorId)
{
    // получить конфиг адресного шлейфа модуля
    auto addrChainCfg = moduleAddressChainConfig(moduleCfgName);
    if (!addrChainCfg)
        return nullptr;
    // вернуть конфиг мультидатчика
    return addrChainCfg->multiSensorConfig(multiSensorId);
}


IssoMultiSensorConfig *IssoObject::findMultiSensorCfgByModuleId(int moduleId, int multiSensorId)
{
    auto module = findModuleById(moduleId);
    if (!module)
        return nullptr;
    // вернуть конфиг мультидатчика
    return findMultiSensorCfgByModuleCfgName(module->moduleConfigName(), multiSensorId);
}


IssoMultiSensorConfig *IssoObject::findMultiSensorCfgByModuleName(const QString &moduleName,
                                                                  int multiSensorId)
{
    auto module = findModuleByDisplayName(moduleName);
    if (!module)
        return nullptr;
    // вернуть конфиг мультидатчика
    return findMultiSensorCfgByModuleCfgName(module->moduleConfigName(), multiSensorId);
}


void IssoObject::applyModuleConfigChanges(const QString &moduleCfgName)
{
    foreach (IssoModule* module, locatedModules())
    {
        // если модуль использует данный конфиг
        if (module->moduleConfigName() == moduleCfgName)
        {
            // обновить все графические шлейфы модуля
            applyModuleConfigToModule(module->displayName(), moduleCfgName);
        }
    }
}


void IssoObject::applyChainConfigChanges(const QString &chainCfgName)
{
    IssoChainConfig* chainCfg = chainConfig(chainCfgName);
    foreach (IssoModuleConfig* moduleCfg, m_moduleConfigs.values())
    {
        // если конфиг модуля содержит ссылку на измененный конфиг шлейфа
        if (moduleCfg->containsChainConfigLink(chainCfgName))
        {
            // если конфиг шлейфа уже не существует, удалить ссылку на него
            if (!chainCfg)
                moduleCfg->removeChainConfigLinksByName(chainCfgName);
            // применить изменения конфига ко всем модулям системы,
            // использующим данный конфиг
            applyModuleConfigChanges(moduleCfg->name());
        }
    }
}


void IssoObject::setOnline()
{
    foreach (IssoModule* module, locatedModules())
        module->setOnline();
}

void IssoObject::setOffline()
{
    foreach (IssoModule* module, locatedModules())
        module->setOffline();
}


void IssoObject::setModuleOnline(const QString &moduleName)
{
    IssoModule* module = findModuleByDisplayName(moduleName);
    if (module)
        module->setOnline();
}


void IssoObject::setModuleOffline(const QString &moduleName)
{
    IssoModule* module = findModuleByDisplayName(moduleName);
    if (module)
        module->setOffline();
}


QStringList IssoObject::chainConfigNames()
{
    return m_chainConfigs.keys();
}


QStringList IssoObject::chainConfigNames(IssoChainType chainType)
{
    QStringList list;
    foreach (IssoChainConfig* cfg, m_chainConfigs.values())
    {
        if (cfg->type() == chainType)
            list << cfg->name();
    }
    return list;
}


QMap<QString, IssoRelayClass> IssoObject::relayClasses() const
{
    return m_relayClasses;
}


void IssoObject::setRelayClasses(const QMap<QString, IssoRelayClass> &relayClasses)
{
    m_relayClasses = relayClasses;
}


void IssoObject::insertRelayClass(const IssoRelayClass &relayClass)
{
    m_relayClasses.insert(relayClass.name, relayClass);
}


void IssoObject::removeRelayClass(const QString &relayClassName)
{
    // удалить класс реле из коллекции объекта
    m_relayClasses.remove(relayClassName);
    // удалить имя класса из всех сценариев
    foreach (IssoScenario* scenario, m_scenarios.values())
    {
        if (scenario->containsRelayClassName(relayClassName))
            scenario->removeRelayClassName(relayClassName);
    }
    // удалить привязки реле к имени класса во всех конфигах модулей
    foreach (IssoModuleConfig* moduleCfg, m_moduleConfigs)
    {
        moduleCfg->removeRelayClassName(relayClassName);
    }
}


void IssoObject::replaceRelayClass(const QString &oldClassName,
                                   const IssoRelayClass &newClass)
{
    // если класс присутствует в коллекции, заменить его на новый
    if (m_relayClasses.contains(oldClassName))
    {
        // удалить старый класс из коллекции объекта
        m_relayClasses.remove(oldClassName);
        // добавить новый класс в коллекцию объекта
        m_relayClasses.insert(newClass.name, newClass);
        // заменить во всех сценариях старое имя класса на новое
        foreach (IssoScenario* scenario, m_scenarios)
        {
            scenario->replaceRelayClassName(oldClassName, newClass.name);
        }
        // обновить имя класса во всех конфигах модулей
        foreach (IssoModuleConfig* moduleCfg, m_moduleConfigs)
        {
            moduleCfg->updateRelayClassName(oldClassName, newClass.name);
        }
    }
}


IssoRelayClass IssoObject::relayClass(const QString &relayClassName)
{
    return m_relayClasses.value(relayClassName);
}


bool IssoObject::relayClassExists(const QString &relayClassName)
{
    return m_relayClasses.contains(relayClassName);
}


QStringList IssoObject::relayClassNames() const
{
    return m_relayClasses.keys();
}


QMap<QString, IssoScenario *> IssoObject::scenarios() const
{
    return m_scenarios;
}


void IssoObject::setScenarios(const QMap<QString, IssoScenario *> &scenarios)
{
    m_scenarios = scenarios;
}


void IssoObject::insertScenario(IssoScenario *scenario)
{
    m_scenarios.insert(scenario->name(), scenario);
}


void IssoObject::removeScenario(const QString &scenarioName)
{
    // уничтожить объект сценария
    IssoScenario* scenario = m_scenarios.value(scenarioName, nullptr);
    if (scenario)
        delete scenario;
    // удалить сценарий из коллекции объекта
    m_scenarios.remove(scenarioName);
    // удалить имя сценария из всех конфигураций модуля
    foreach (IssoModuleConfig* cfg, m_moduleConfigs)
    {
        cfg->removeScenarioName(scenarioName);
    }
}


void IssoObject::updateScenario(const QString &oldScenarioName,
                                const QString &newScenarioName,
                                IssoScenarioScope newScope,
                                const QStringList &newClassNames)
{
    // получить сценарий по имени
    IssoScenario* scenario = this->scenario(oldScenarioName);
    // сохранить настройки сценария
    scenario->setName(newScenarioName);
    scenario->setScope(newScope);
    scenario->setRelayClassNames(newClassNames);
    // если имя сценария изменилось
    if (oldScenarioName != newScenarioName)
    {
        // изменить имя сценария во всех конфигах модуля
        foreach (IssoModuleConfig* cfg, m_moduleConfigs)
        {
            cfg->updateScenarioName(oldScenarioName, newScenarioName);
        }
    }
}


void IssoObject::destroyAllScenarios()
{
    // уничтожить все сценарии из коллекции
    foreach (IssoScenario* scenario, m_scenarios)
    {
        if (scenario)
            delete scenario;
    }
    // очистить коллекцию сценариев
    m_scenarios.clear();
}


IssoScenario *IssoObject::scenario(const QString &scenarioName)
{
    return m_scenarios.value(scenarioName, nullptr);
}


bool IssoObject::scenarioExists(const QString &scenarioName)
{
    return m_scenarios.contains(scenarioName);
}


QStringList IssoObject::scenarioNames() const
{
    return m_scenarios.keys();
}


void IssoObject::saveScenarios(QDataStream &stream)
{
    // записать количество сценариев
    stream << m_scenarios.size();
    // записать в поток каждый сценарий
    foreach (IssoScenario* scenario, m_scenarios)
    {
        scenario->saveToStream(stream);
    }
}


bool IssoObject::loadScenarios(QDataStream &stream)
{
    // считать количество сценариев
    int scenarioCount;
    stream >> scenarioCount;
    // считать из потока каждый сценарий
    while (scenarioCount > 0)
    {
        // создать пустой сценарий
        IssoScenario* scenario = new IssoScenario();
        // загрузить из потока
        scenario->loadFromStream(stream);
        // добавить в коллекцию
        insertScenario(scenario);
        // уменьшить счетчик
        --scenarioCount;
    }
    return true;
}


QMap<int, QString> IssoObject::buttonScenarioLinks() const
{
    return m_buttonScenarioLinks;
}


void IssoObject::setButtonScenarioLinks(const QMap<int, QString> &buttonScenarioLinks)
{
    m_buttonScenarioLinks = buttonScenarioLinks;
}


void IssoObject::insertButtonScenarioLink(int buttonNum, const QString &buttonScenarioName)
{
    m_buttonScenarioLinks.insert(buttonNum, buttonScenarioName);
}


void IssoObject::removeButtonScenarioLink(int buttonNum)
{
    m_buttonScenarioLinks.remove(buttonNum);
}


QString IssoObject::buttonScenarioLink(int buttonNum)
{
    return m_buttonScenarioLinks.value(buttonNum, "");
}


QString IssoObject::displayName() const
{
//    return QString("%1-%2").arg(/*m_id*/objectCard().id(), 4, 10, QLatin1Char('0'))
//                           .arg(m_objectCard.city());
    return m_objectCard.displayName();
}


QHostAddress IssoObject::serverIp() const
{
    return m_serverIp;
}


void IssoObject::setServerIp(const QHostAddress &serverIp)
{
    m_serverIp = serverIp;
}


void IssoObject::calcTotalSummary()
{
    // обнулить поля
    m_totalModules = m_totalMs = m_totalChannels = 0;
    // обойти все модули
    foreach (auto module, m_modules)
    {
        // если модуль не размещен, не обрабатывать
        if (!module->isLocated())
            continue;
        // инекрементировать общее число модулей
        m_totalModules++;
        // общее число МД
        auto addrChainCfg = moduleAddressChainConfig(module->moduleConfigName());
        if (!addrChainCfg)
            continue;
        // общее число каналов МД
        m_totalMs += addrChainCfg->multiSensorConfigCount();
        // обойти все конфиги МД
        foreach (auto msCfg, addrChainCfg->multiSensorConfigs())
        {
            m_totalChannels += msCfg->channelCount();
        }
    }
}


void IssoObject::calcAlarmSummary(int& alarmModules,
                                  int& alarmMs,
                                  int& alarmChannels) const
{
    // обнулить
    alarmModules = alarmMs = alarmChannels = 0;
    // обработать все модули
    foreach (auto module, m_modules)
    {
        // если модуль не размещен, не обрабатывать
        if (!module->isLocated())
            continue;
        // число тревожных модулей
        if (module->alarmed())
            alarmModules++;
        //
        // параметр состояний МД
        auto msParam = dynamic_cast<IssoMultiSensorStateParam*>(
                                module->stateParamById(MULTISENSOR));
        if (!msParam)
            continue;
        foreach (auto msData, msParam->dataMap())
        {
            // число тревожных МД
            alarmMs += (msData.alarmed() ? 1 : 0);
            // число тревожных каналов
            alarmChannels += msData.alarmChannels();
        }
    }
}


int IssoObject::currentUniqueId() const
{
    return m_currentUniqueId;
}

QHostAddress IssoObject::currentIp() const
{
    return m_currentIp;
}

QByteArray IssoObject::currentMac() const
{
    return m_currentMac;
}

int IssoObject::totalModules() const
{
    return m_totalModules;
}

int IssoObject::totalMs() const
{
    return m_totalMs;
}

int IssoObject::totalChannels() const
{
    return m_totalChannels;
}

void IssoObject::destroyBuildings()
{
    // уничтожить все здания
    foreach (IssoBuilding* build, m_buildings)
        delete build;
    // обнулить ссылки на здание и этаж
    mp_building = nullptr;
    mp_floor = nullptr;
    // очистить список зданий
    m_buildings.clear();
}


void IssoObject::destroyModules()
{
    // уничтожить все модули
    foreach (IssoModule* module, m_modules)
        delete module;
    // очистить список модулей
    m_modules.clear();
}


void IssoObject::destroyCameras()
{
    // уничтожить все камеры
    foreach (IssoCamera* camera, m_cameras)
        delete camera;
    // очистить список камер
    m_cameras.clear();

}
