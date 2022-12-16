#include "issobuilding.h"


int IssoBuilding::testId = 1;
int IssoBuilding::testAddr = 100;


IssoBuilding::IssoBuilding() : IssoBuilding(0, "")
{

}


IssoBuilding::IssoBuilding(int number, QString name, QObject *parent)
    : QObject(parent),
      m_number(number),
      m_name(name)
{
    init();
}


IssoBuilding::~IssoBuilding()
{
    qDebug() << "~IssoBuilding - " << this->displayName() << endl;
    //
    uninit();
}


void IssoBuilding::loadFromStream(QDataStream &stream)
{
    // номер здания
    stream >> m_number;
    // имя здания
    stream >> m_name;

    //
    // количество этажей здания
    int floorCount;
    stream >> floorCount;
    // этажи
    for (int i = 0; i < floorCount; ++i)
    {
        // создать
        IssoFloorPlan* floor = new IssoFloorPlan();
        // загрузить этаж
        floor->loadFromStream(stream);
        // добавить в список этажей
        addFloor(floor);
    }
}


void IssoBuilding::saveToStream(QDataStream &stream)
{
    // номер здания
    stream << m_number;
    // имя здания
    stream << m_name;
    //
    // количество этажей здания
    int floorCount = m_floors.size();
    stream << floorCount;
    // этажи
    foreach (IssoFloorPlan* floor, m_floors)
    {
        // сохранить этаж
        floor->saveToStream(stream);
    }
}


QList<IssoFloorPlan *> IssoBuilding::floors() const
{
    return m_floors;
}


int IssoBuilding::floorCount() const
{
    return m_floors.size();
}


QString IssoBuilding::name() const
{
    return m_name;
}

QString IssoBuilding::displayName() const
{
    return QString("%1-%2").arg(m_name)
                           .arg(m_number, 2, 10, QLatin1Char('0'));
}


QStringList IssoBuilding::floorNames()
{
    QStringList list;
    // получить имена всех планов этажей
    foreach (IssoFloorPlan* floor, m_floors)
        list << floor->displayName();
    return list;
}


QSet<int> IssoBuilding::floorNumbers()
{
    QSet<int> floorNumbers;
    foreach (IssoFloorPlan* floor, m_floors)
        floorNumbers.insert(floor->number());
    return floorNumbers;
}


IssoFloorPlan *IssoBuilding::findFloorByName(const QString &name)
{
    foreach (IssoFloorPlan* floor, m_floors)
    {
        if (floor->displayName() == name)
            return floor;
    }
    return nullptr;
}


IssoFloorPlan *IssoBuilding::findFloorByNumber(int number)
{
    foreach (IssoFloorPlan* floor, m_floors)
    {
        if (floor->number() == number)
            return floor;
    }
    return nullptr;
}


IssoModule *IssoBuilding::findModuleById(int moduleId)
{
    foreach (IssoFloorPlan* floor, m_floors)
    {
        // искать модуль по id на всех этажах здания
        IssoModule* module = floor->findById(moduleId);
        // если найден, вернуть модуль
        if (module)
            return module;
    }
    return nullptr;
}

IssoModule *IssoBuilding::findModuleByAddress(QHostAddress addr)
{
    foreach (IssoFloorPlan* floor, m_floors)
    {
        // искать модуль по адресу на всех этажах здания
        IssoModule* module = floor->findByAddress(addr);
        // если найден, вернуть модуль
        if (module)
            return module;
    }
    return nullptr;
}

IssoModule *IssoBuilding::findModuleByDisplayName(const QString &displayName)
{
    foreach (IssoFloorPlan* floor, m_floors)
    {
        // искать модуль по адресу на всех этажах здания
        IssoModule* module = floor->findByDisplayName(displayName);
        // если найден, вернуть модуль
        if (module)
            return module;
    }
    return nullptr;
}

QList<IssoModule*> IssoBuilding::modules()
{
    QList<IssoModule*> list;
    foreach (IssoFloorPlan* floor, m_floors)
    {
        list.append(floor->modules());
    }
    return list;
}


int IssoBuilding::number() const
{
    return m_number;
}


QSet<IssoModuleState> IssoBuilding::moduleStates() const
{
    QSet<IssoModuleState> states;
    foreach (IssoFloorPlan* floor, m_floors)
    {
        // добавить набор состояний модулей текущего этажа
        states.unite(floor->moduleStates());
    }
    return states;
}


bool IssoBuilding::activated() const
{
    foreach (IssoFloorPlan* floor, m_floors)
    {
        if (!floor->activated())
            return false;
    }
    return true;
}


void IssoBuilding::init()
{
}


void IssoBuilding::uninit()
{
    // уничтожить все планы этажей
    foreach (IssoFloorPlan* floor, m_floors)
        delete floor;
    // очистить список планов этажей
    m_floors.clear();
}


bool IssoBuilding::addFloor(int number, const QPixmap& background)
{
    // если этаж с заданным номером уже существует, вырнуть неуспех
    if (findFloorByNumber(number))
        return false;
    // создать план этажа
    IssoFloorPlan* floor = new IssoFloorPlan(number, m_number);
    // установить этажу фоновое изображение
    floor->setBackground(background.toImage());
    return addFloor(floor);
}


bool IssoBuilding::addFloor(IssoFloorPlan *floor)
{
    if (!floor)
        return false;
    // добавить в список этажей здания
    m_floors << floor;
    // сортировать список по номеру
    std::sort(m_floors.begin(), m_floors.end(),
              [](const IssoFloorPlan* a, const IssoFloorPlan* b) ->
                            bool { return a->number() < b->number(); });
    return true;
}


bool IssoBuilding::removeFloor(IssoFloorPlan* floor)
{
    if (!floor)
        return false;
    // удалить этаж из списка здания
    bool result = m_floors.removeOne(floor);
    // уничтожить этаж
    delete floor;
    return result;
}
