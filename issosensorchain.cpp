#include "issosensorchain.h"

#include <QGraphicsScene>


IssoSensorChain::IssoSensorChain(QObject *parent)
    : IssoSensorChain(0, 0, 0, SMOKE_CHAIN, 0, parent)
{
}


IssoSensorChain::IssoSensorChain(int id, int moduleUniqueId, int moduleId,
                                 IssoChainType chainType, int sensorCount,
                                 QObject *parent)
    : QObject(parent),
      m_id(id),
      m_moduleUniqueId(moduleUniqueId),
      m_moduleDisplayId(moduleId),
      m_chainType(chainType)/*,
      m_visible(false)*/
{
    addSeveral(sensorCount);
//    for (int i = 1; i <= sensorCount; ++i)
//    {
//        // создать объект датчика
//        IssoSensor* sensor =
//                new IssoSensor(i, m_moduleUniqueId, m_moduleDisplayId, m_id, m_chainType);
//        // добавить датчик в шлейф
//        add(sensor);
//    }
}


IssoSensorChain::~IssoSensorChain()
{
//    qDebug() << "~IssoSensorChain" << endl;
    removeAll();
}


void IssoSensorChain::loadFromStream(QDataStream &stream)
{
    // идентификатор (номер) шлейфа
    stream >> m_id;
    // уникальный ID модуля
    stream >> m_moduleUniqueId;
    // отображаемый ID модуля
    stream >> m_moduleDisplayId;
    // тип шлейфа
    stream >> m_chainType;
    //
    // список датчиков шлейфа:
    //
    // количество датичиков
    int sensorCount;
    stream >> sensorCount;
    // датчики
    for (int i = 0; i < sensorCount; ++i)
    {
        // создать датчик
        IssoSensor* sensor = new IssoSensor();
        // загрузить из потока
        sensor->loadFromStream(stream);
        // добавить в список
        add(sensor);
    }
}


void IssoSensorChain::saveToStream(QDataStream &stream)
{
    // идентификатор (номер) шлейфа
    stream << m_id;
    // уникальный ID модуля
    stream << m_moduleUniqueId;
    // отображаемый ID модуля
    stream << m_moduleDisplayId;
    // тип шлейфа
    stream << m_chainType;
    //
    // список датчиков шлейфа:
    //
    // количество датичиков
    stream << m_sensors.size();
    // датчики
    foreach (IssoSensor* sensor, m_sensors)
    {
        sensor->saveToStream(stream);
    }
}


int IssoSensorChain::id() const
{
    return m_id;
}


void IssoSensorChain::setId(int id)
{
    m_id = id;
}


int IssoSensorChain::moduleUniqueId() const
{
    return m_moduleUniqueId;
}


void IssoSensorChain::setModuleUniqueId(int moduleUniqueId)
{
    m_moduleUniqueId = moduleUniqueId;
}


int IssoSensorChain::moduleDisplayId() const
{
    return m_moduleDisplayId;
}


void IssoSensorChain::setModuleDisplayId(int moduleDisplayId)
{
    m_moduleDisplayId = moduleDisplayId;
}


void IssoSensorChain::add(IssoSensor *sensor)
{
    m_sensors.append(sensor);
}


void IssoSensorChain::addSeveral(int sensorCount)
{
    // определить ID, с которого начинать добавление
    int startId = (m_sensors.isEmpty() ? 1 : m_sensors.last()->id() + 1);
    for (int i = 0; i < sensorCount; ++i)
    {
        // создать объект датчика
        IssoSensor* sensor = new IssoSensor(startId + i, m_moduleUniqueId,
                                            m_moduleDisplayId, m_id, m_chainType);
        // добавить датчик в шлейф
        add(sensor);
    }
}


void IssoSensorChain::remove(IssoSensor *sensor)
{
    // удалить датчик из списка
    m_sensors.removeOne(sensor);
    // уничтожить датчик
    delete sensor;
}


void IssoSensorChain::removeById(int sensorId)
{
    remove(sensor(sensorId));
}


void IssoSensorChain::removeSeveral(int sensorCount)
{
    for (int i = 0; i < sensorCount; ++i)
    {
        if (m_sensors.isEmpty())
            break;
        remove(m_sensors.last());
    }
}


void IssoSensorChain::removeAll()
{
    while(!m_sensors.empty())
        remove(m_sensors.last());
}


IssoChainType IssoSensorChain::type() const
{
    return m_chainType;
}


void IssoSensorChain::setType(const IssoChainType &type)
{
    m_chainType = type;
}


QList<IssoSensor *> IssoSensorChain::sensors() const
{
    return m_sensors;
}


void IssoSensorChain::setSensors(QList<IssoSensor*> sensors)
{
    removeAll();
    m_sensors.append(sensors);
}


IssoSensor *IssoSensorChain::sensor(quint8 id)
{
    foreach (IssoSensor* sensor, m_sensors)
    {
        if (sensor->id() == id)
            return sensor;
    }
    return nullptr;
}


QString IssoSensorChain::sensorLocation(quint8 id)
{
    auto curSensor = this->sensor(id);
    return (curSensor ? curSensor->locationInfo() : "");
}


int IssoSensorChain::sensorCount()
{
    return m_sensors.size();
}


void IssoSensorChain::setSensorCount(int count)
{
    int diffCount = count - m_sensors.size();
    // если число датчиков не изменяется, выход
    if (diffCount == 0)
        return;
    // если число датчиков больше заданого значения, удалить лишние
    if (diffCount < 0)
        removeSeveral(-diffCount);
    else
        // иначе добавить недостающие
        addSeveral(diffCount);
}


QList<int> IssoSensorChain::sensorIds()
{
    QList<int> list;
    foreach (auto sensor, m_sensors)
        list << sensor->id();
    return list;
}


int IssoSensorChain::alarmCount()
{
    int count = 0;
    foreach (auto sensor, m_sensors)
    {
        if ((sensor->state() == STATE_WARNING) ||
                (sensor->state() == STATE_ALARM))
            count++;
    }
    return count;
}


//bool IssoSensorChain::visible() const
//{
//    return m_visible;
//}


void IssoSensorChain::displayAllSensors(QGraphicsScene *scene)
{
    if (!scene)
        return;
//    m_visible = true;
    foreach (IssoSensor* sensor, m_sensors)
    {
        if (sensor->scene() != scene)   // !sensor->isVisible())
            scene->addItem(sensor);
    }
}


void IssoSensorChain::displayAlarmSensors(QGraphicsScene *scene)
{
    if (!scene)
        return;
//    m_visible = true;
    foreach (IssoSensor* sensor, m_sensors)
    {
        // если датчик не в Норме, отобразить
        if (sensor->state() != STATE_NORMAL)
        {
            // если не на сцене, добавить
            if (sensor->scene() != scene)
                scene->addItem(sensor);
        }
        // иначе скрыть датчик, если он отображен
        else if (auto scene = sensor->scene())
        {
            scene->removeItem(sensor);
        }
    }
}


void IssoSensorChain::hideAllSensors()
{
//    m_visible = false;
    foreach (IssoSensor* sensor, m_sensors)
    {
//        QGraphicsScene *scene = sensor->scene();
        if (auto scene = sensor->scene())
            scene->removeItem(sensor);
    }
}


void IssoSensorChain::setInitPosition(QPointF pos)
{
    foreach (IssoSensor* sensor, m_sensors)
        sensor->setPos(pos);
}


void IssoSensorChain::setStateToAllSensors(IssoState state, const QString &hint)
{
    // задать всем аналоговым датчикам одно состояние
    foreach (IssoSensor* sensor, m_sensors)
    {
        // задать состояние
        sensor->setState(state);
        // задать подсказку
        sensor->setToolTip(hint);
    }
}


void IssoSensorChain::setStateToSingleSensor(int multiSensorId, IssoState state, const QString &hint)
{
    foreach (IssoSensor* sensor, m_sensors)
    {
        // если цифровой датчик найден, задать состояние
        if (sensor->id() == multiSensorId)
        {
            // задать состояние
            sensor->setState(state);
            // задать подсказку
            sensor->setToolTip(hint);
            return;
        }
    }
}


void IssoSensorChain::clearState()
{
    foreach (IssoSensor* sensor, m_sensors)
    {
        // сбросить состояние
        sensor->setState(STATE_UNDEFINED);
        // очистить подсказку
        sensor->setToolTip("");
    }
}


bool IssoSensorChain::inNormalState()
{
    foreach (IssoSensor* sensor, m_sensors)
    {
        if ((sensor->state() != STATE_NORMAL) &&
            (sensor->state() != VOLTAGE_MEDIUM) &&
            (sensor->state() != VOLTAGE_HIGH))
            return false;
    }
    return true;
}


void IssoSensorChain::setMovable(bool movable)
{
    // задать возможность перемещения всех датчиков шлейфа
    foreach (IssoSensor* sensor, m_sensors)
    {
        sensor->setMovable(movable);
    }
}

