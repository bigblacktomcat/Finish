#include "issomodule.h"
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsWidget>


IssoModule::IssoModule()
    : IssoModule(0, 0, 0, 0,
                 IssoConst::DEFAULT_MODULE_IP,
                 IssoConst::SEND_PORT,
                 IssoConst::DEFAULT_MAC,
                 IssoConst::DEFAULT_SERVER_IP)
{
}


IssoModule::IssoModule(int uniqueId, int displayId, qreal x, qreal y,
                       const QHostAddress &ip, quint16 port,
                       const QByteArray &mac,
                       const QHostAddress &serverip, quint16 serverPort,
                       qreal width, qreal height,
                       int floorNum, int buildNum,
                       QGraphicsItem *parent)
    : QGraphicsObject(parent),
      m_uniqueId(uniqueId),
      m_displayId(displayId),
      m_ip(ip),
      m_port(port),
      m_mac(mac),
      m_serverIp(serverip),
      m_serverPort(serverPort),
      m_phone(IssoConst::DEFAULT_PHONE),
      m_floorNum(floorNum),
      m_buildNum(buildNum),
      m_moduleConfigName(""),
      m_rect(QRectF(0, 0, width, height)),
      m_fillColor(IssoCommonData::backgroundByModuleState(INACTIVE)),
      m_textColor(IssoCommonData::foregroundByModuleState(INACTIVE))
{
    // сделать компонент выделяемым и перемещаемым
    setFlags(ItemIsSelectable | ItemIsMovable | ItemSendsGeometryChanges);
    // установить реакцию на вход / выход мыши из границ компонента
    setAcceptHoverEvents(true);
    // сформировать геометрию модуля
    createShape();
    // задать позицию
    setPos(x, y);
}


IssoModule::IssoModule(const IssoModule &other) : QGraphicsObject(other.parentObject())
{
    m_uniqueId = other.m_uniqueId;
    m_displayId = other.m_displayId;
    m_activated = other.m_activated;
    // дескриптор
    m_ip = other.m_ip;
    m_port = other.m_port;
    m_mac = other.m_mac;
    m_serverIp = other.m_serverIp;
    m_serverPort = other.m_serverPort;
    m_phone = other.m_phone;
    m_phoneEnabled = other.m_phoneEnabled;
    //
    m_floorNum = other.m_floorNum;
    m_buildNum = other.m_buildNum;
    // периферия:
    // камера
    m_cameraEnabled = other.m_cameraEnabled;
    m_cameraId = other.m_cameraId;
    // табло
    m_boardEnabled = other.m_boardEnabled;
    m_boardIp = other.m_boardIp;
    m_boardPort = other.m_boardPort;
    //
    // настройки графического отображения
    m_rect = other.m_rect;
    m_fillColor = other.m_fillColor;
    m_textColor = other.m_textColor;
    m_shape = other.m_shape;
    m_topTitleRect = other.m_topTitleRect;
    m_bottomTitleRect = other.m_bottomTitleRect;
    //
    m_moduleState = other.m_moduleState;
}


IssoModule::~IssoModule()
{
    removeAllSensorChains();
    //
    qDebug() << "~IssoModule - " << this->displayName() << endl;
}


void IssoModule::loadFromStream(QDataStream &stream)
{
    // уникальный идентификатор
    stream >> m_uniqueId;
    // отображаемый идентификатор
    stream >> m_displayId;
    // координаты:
    // X
    int posX;
    stream >> posX;
    this->setX(posX);
    // Y
    int posY;
    stream >> posY;
    this->setY(posY);
    // признак активации
    stream >> m_activated;
    // дескриптор
    QByteArray descData(IssoModuleDescriptor::LENGTH, '0');
    stream >> descData;
    IssoModuleDescriptor* desc = IssoModuleDescriptor::fromBytes(descData);
    setDescriptor(*desc);
    delete desc;
    // этаж
    stream >> m_floorNum;
    // здание
    stream >> m_buildNum;
    //
    // периферия:
    // табло
    stream >> m_boardEnabled;
    if (m_boardEnabled)
    {
        // ip
        quint32 boardIp;
        stream >> boardIp;
        m_boardIp = QHostAddress(boardIp);
        // порт
        stream >> m_boardPort;
    }
    //
    // имя конфигурации модуля
    stream >> m_moduleConfigName;
    //
    // коллекция графических объектов шлейфов:
    //
    // загрузить количество шлейфов
    int chainCount;
    stream >> chainCount;
    // загрузить шлейфы
    for (int i = 0; i < chainCount; ++i)
    {
        // создать шлейф
        IssoSensorChain* chain = new IssoSensorChain();
        // загрузить ID
        IssoParamId id;
        stream >> id;
        // загрузить шлейф
        chain->loadFromStream(stream);
        // добавить в коллекцию
        insertSensorChain(id, chain);
    }
    //
    // коллекция ID мультидатчиков, находящихся в сервисном режиме:
    // загрузить коллекцию ID мультидатчиков на обслуживании
    QSet<quint8> serviceIds;
    stream >> serviceIds;
    // задать сервисный режим для каждого мультидатчика из коллекции
    foreach (quint8 multiSensorId, serviceIds)
        setMultiSensorServiceMode(multiSensorId, true);
}


void IssoModule::saveToStream(QDataStream &stream)
{
    // уникальный идентификатор
    stream << m_uniqueId;
    // отображаемый идентификатор
    stream << m_displayId;
    // координаты
    int posX = this->x();
    stream << posX;
    int posY = this->y();
    stream << posY;
    // признак активации
    stream << m_activated;
    // дескриптор
    IssoModuleDescriptor desc = getDescriptor();
    stream << desc.bytes();
    // этаж
    stream << m_floorNum;
    // здание
    stream << m_buildNum;
    //
    // периферия:
    // табло
    stream << m_boardEnabled;
    if (m_boardEnabled)
    {
        stream << m_boardIp.toIPv4Address();
        stream << m_boardPort;
    }
    //
    // имя конфигурации модуля
    stream << m_moduleConfigName;
    //
    // коллекция графических объектов шлейфов:
    //
    // сохранить количество шлейфов
    stream << m_chains.size();
    // сохранить каждый шлейф
    foreach (IssoParamId id, m_chains.keys())
    {
        // получить шлейф
        IssoSensorChain* chain = sensorChain(id);
        if (!chain)
            continue;
        // сохранить ID
        stream << id;
        // сохранить шлейф
        chain->saveToStream(stream);
    }
    // коллекция ID мультидатчиков, находящихся в сервисном режиме
    stream << m_serviceMultiSensorIds;
}


QRectF IssoModule::boundingRect() const
{
    return m_shape.boundingRect();
}


QPainterPath IssoModule::shape() const
{
    return m_shape;
}


void IssoModule::paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *option,
                       QWidget *widget)
{
    // подавить предупреждения
    Q_UNUSED(widget);
    painter->setRenderHints(QPainter::Antialiasing |
                            QPainter::SmoothPixmapTransform);
    // получить коэффициент масштабирования
    const qreal lod =
            option->levelOfDetailFromTransform(painter->worldTransform());
    // если мышь находится над компонентом, сделать фон светлее
    QColor fillColor = (option->state & QStyle::State_MouseOver)
                        ? m_fillColor.lighter() : m_fillColor;
    QColor penColor = QColor(Qt::black);
    // сохранить настройки
    painter->save();
    // настроить обводку
    QPen pen = painter->pen();
    pen.setWidthF(pen.widthF() / lod);  // сохранить толщину линии при масштабировании
    pen.setColor(penColor);             // установить цвет обводки
    painter->setPen(pen);               // установить настроенную обводку
    //
    // залить фон модуля
    painter->setBrush(fillColor);       // заливка
    painter->drawPath(this->shape());   // отрисовка
    //
    // если модуль выбран, отобразить выделение
    if (this->isSelected())
    {
        // цвет заливки
        painter->setBrush(SELECT_COLOR);
        // сформировать выделяемый (нижний) прямоугольник
        QPainterPath bottomPath;
        bottomPath.addRect(m_bottomTitleRect);
        // получить выделяемую область модуля по форме модуля
        QPainterPath selectPath = bottomPath.intersected(shape());
        // завершить фигуру
        selectPath.closeSubpath();
        // отобразить
        painter->drawPath(selectPath);
    }
    //
    // верхний заголовок(здание-этаж)
    //
    QString topTitle = this->topTitle();
    // цвет заголовка
    painter->setPen(m_textColor);
    // установить шрифт
    QFont font = QFont(/*"Sans Serif"*/"Arial", TOP_TITLE_FONT_SIZE, QFont::Medium);
    painter->setFont(font);
    // измерить верхний заголовок
    QFontMetrics fm = QFontMetrics(font);
    int strWidth = fm.width(topTitle);
    int strHeight = fm.capHeight();
    // рассчитать координаты верхнего заголовка
    qreal strX = m_topTitleRect.center().x() - strWidth / 2;
    qreal strY = m_topTitleRect.center().y() + strHeight / 2;
    // вывести верхний заголовок
    painter->drawText(strX, strY, topTitle);
    //
    // нижний заголовок (номер модуля)
    //
    QString bottomTitle = this->bottomTitle();
    // если модуль выбран, установить цвет заголовка
    if (this->isSelected())
        painter->setPen(SELECT_TEXT_COLOR);
    // установить шрифт
    font = QFont(/*"Sans Serif"*/"Arial", BOTTOM_TITLE_FONT_SIZE, QFont::Medium);
    painter->setFont(font);
    // измерить нижний заголовок
    fm = QFontMetrics(font);
    strWidth = fm.width(bottomTitle);
    strHeight = fm.capHeight();
    // рассчитать координаты нижнего заголовка
    strX = m_bottomTitleRect.center().x() - strWidth / 2;
    strY = m_bottomTitleRect.center().y() + strHeight / 2;
    // вывести верхний заголовок
    painter->drawText(strX, strY, bottomTitle);
    //
    // восстановить настройки
    painter->restore();
}


qreal IssoModule::width() const
{
    return boundingRect().width();
}


qreal IssoModule::height() const
{
    return boundingRect().height();
}


int IssoModule::displayId() const
{
    return m_displayId;
}


QString IssoModule::displayName() const
{
    QString resultStr;
    // номер здания или этажа = 0
    if ((m_buildNum == 0) || (m_floorNum == 0))
    {
        // вернуть уникальный id модуля
        resultStr = tr("Модуль-%1").arg(m_uniqueId);
    }
    else
    {
        // если подвал, то вместо "-" отображать "П"
        QString floorStr = (m_floorNum < 0 ?
                             QString("П%1").arg(abs(m_floorNum)) :
                             QString("%1").arg(m_floorNum, 2, 10, QLatin1Char('0')));
        // сформировать строку формата XX-YY-ZZZ, где
        //      XX - номер здания
        //      YY - номер этажа
        //      ZZZ - порядковый номер модуля
        resultStr = QString("%1-%2-%3").arg(m_buildNum, 2, 10, QLatin1Char('0'))
                                       .arg(floorStr)
                                       .arg(m_displayId, 3, 10, QLatin1Char('0'));
    }
    return resultStr;
}


int IssoModule::floorNum() const
{
    return m_floorNum;
}


void IssoModule::setFloorNum(int floorNum)
{
    m_floorNum = floorNum;
}


int IssoModule::buildNum() const
{
    return m_buildNum;
}


void IssoModule::setBuildNum(int buildNum)
{
    m_buildNum = buildNum;
}


QHostAddress IssoModule::ip() const
{
    return m_ip;
}


quint16 IssoModule::port() const
{
    return m_port;
}


IssoModuleState IssoModule::moduleState() const
{
    return m_moduleState;
}


void IssoModule::setModuleState(const IssoModuleState &state)
{
    // сохранить состояние
    m_moduleState = state;
    // установить соответствующие цвета
    m_fillColor = IssoCommonData::backgroundByModuleState(state);
    m_textColor = IssoCommonData::foregroundByModuleState(state);
    // обновить
    update();
}


IssoSensorDisplayMode IssoModule::setSensorDisplayMode(IssoSensorDisplayMode mode)
{
    // сохранить режим отображения
    m_sensorDisplayMode = mode;
    // обновить режим отображения датчиков для каждого шлейфа
    foreach (IssoParamId id, m_chains.keys())
    {
        updateSensorsVisibility(sensorChain(id));
    }
}


void IssoModule::updateSensorsVisibility(IssoSensorChain *chain)
{
    if (!scene() || !chain)
        return;
    switch(m_sensorDisplayMode)
    {
        case SENSOR_DISPLAY_ALL:
        {
            chain->displayAllSensors(scene());
            break;
        }
        case SENSOR_DISPLAY_ALARM:
        {
            chain->displayAlarmSensors(scene());
            break;
        }
        case SENSOR_HIDE_ALL:
        {
            chain->hideAllSensors();
            break;
        }
    }
}


int IssoModule::paramIdToChainId(IssoParamId paramId)
{
    int chainId = 0;
    switch(paramId)
    {
        case ANALOG_CHAIN_1:
            chainId = 1;
            break;
        case ANALOG_CHAIN_2:
            chainId = 2;
            break;
        case ANALOG_CHAIN_3:
            chainId = 3;
            break;
        case ANALOG_CHAIN_4:
            chainId = 4;
            break;
        case ANALOG_CHAIN_5:
            chainId = 5;
            break;
        case ANALOG_CHAIN_6:
            chainId = 6;
            break;
        case MULTISENSOR:
            chainId = 1;
            break;
        default:
            break;
    }
    return chainId;
}

IssoParamId IssoModule::chainIdToParamId(int chainId, IssoChainType chainType)
{
    IssoParamId paramId = PARAM_UNDEFINED;
    switch(chainType)
    {

        // цифровые шлейфы
        case MULTICRITERIAL:
        {
            paramId = MULTISENSOR;
            break;
        }
        // аналоговые шлейфы
        default:
        {
            switch(chainId)
            {
                case 1:
                    paramId = ANALOG_CHAIN_1;
                    break;
                case 2:
                    paramId = ANALOG_CHAIN_2;
                    break;
                case 3:
                    paramId = ANALOG_CHAIN_3;
                    break;
                case 4:
                    paramId = ANALOG_CHAIN_4;
                    break;
                case 5:
                    paramId = ANALOG_CHAIN_5;
                    break;
                case 6:
                    paramId = ANALOG_CHAIN_6;
                    break;
                default:
                    break;
            }
            break;
        }
    }
    return paramId;
}


void IssoModule::analyzeSensorStates()
{
    IssoModuleState resultState = m_stateParams.getResultState();
    // установить результирующее состояние модуля
    setModuleState(resultState);
}


void IssoModule::setMovable(bool movable)
{
    // задать возможность перемещения модуля
    setFlag(ItemIsMovable, movable);
    // задать возможность перемещения шлейфов
    foreach (IssoParamId id, m_chains.keys())
    {
        IssoSensorChain* chain = sensorChain(id);
        if (chain)
            chain->setMovable(movable);
    }
}


void IssoModule::setDisplayId(int id)
{
    m_displayId = id;
}


int IssoModule::uniqueId() const
{
    return m_uniqueId;
}


void IssoModule::setUniqueId(int uniqueId)
{
    m_uniqueId = uniqueId;
}


void IssoModule::setIp(const QHostAddress &address)
{
    m_ip = address;
}


void IssoModule::setPort(const quint16 &port)
{
    m_port = port;
}


bool IssoModule::isLocated() const
{
    return (m_buildNum != 0) && (m_floorNum != 0);
}


bool IssoModule::isActivated() const
{
    return m_activated;
}


void IssoModule::setActivated(bool activated)
{
    m_activated = activated;
}


IssoModuleDescriptor IssoModule::getDescriptor() const
{
    return IssoModuleDescriptor(m_ip.toIPv4Address(), m_mac, m_port,
                                m_serverIp.toIPv4Address(), m_serverPort,
                                m_phone, m_phoneEnabled, (quint8)0);
}


void IssoModule::setDescriptor(const IssoModuleDescriptor &descr)
{
    m_ip = QHostAddress(descr.ip());
    m_mac = descr.mac();
    m_port = descr.port();
    m_serverIp = QHostAddress(descr.serverIp());
    m_serverPort = descr.serverPort();
    m_phone = descr.phone();
    m_phoneEnabled = descr.phoneEnabled();
}


bool IssoModule::getCameraEnabled() const
{
    return m_cameraEnabled;
}


int IssoModule::getCameraId() const
{
    return m_cameraId;
}


bool IssoModule::getBoardEnabled() const
{
    return m_boardEnabled;
}


QHostAddress IssoModule::getBoardIp() const
{
    return m_boardIp;
}


quint16 IssoModule::getBoardPort() const
{
    return m_boardPort;
}


void IssoModule::setCameraEnabled(bool enabled, int cameraId)
{
    m_cameraEnabled = enabled;
    m_cameraId = (enabled ? cameraId : 0);
}


void IssoModule::setBoardEnabled(bool enabled, const QHostAddress &boardIp, quint16 boardPort)
{
    m_boardEnabled = enabled;
    m_boardIp = (enabled ? boardIp : QHostAddress());
    m_boardPort = (enabled ? boardPort : 0);
}


void IssoModule::clearStateParams()
{
    m_stateParams.clearParams();
    //
    foreach (IssoParamId paramId, m_chains.keys())
    {
        IssoSensorChain* chain = sensorChain(paramId);
        chain->clearState();
    }
    setModuleState(INACTIVE);
}


bool IssoModule::updateStateParam(const QSharedPointer<IssoParam>& paramPtr)
{
    if (!paramPtr)
        return false;
    IssoSensorStateParam* param =
            dynamic_cast<IssoSensorStateParam*>(paramPtr.data());
    if (!param)
        return false;
    // получить текущий параметр по id
    IssoSensorStateParam* curParam = stateParamById(param->id());
    // определить, изменилось ли состояние
    bool stateChanged = !curParam || (curParam && (curParam->state() != param->state()));
    //
    // обновить состояние аналогового шлейфа
    updateAnalogChainState(param);
    // обновить параметр
    m_stateParams.insertParamPtr(paramPtr);
    // если параметр изменился, добавить в список
    if (stateChanged)
        m_changedParams.append(paramPtr);
    //
    // вернуть результат
    return stateChanged;
}


bool IssoModule::updateMultiSensorStateParam(const QSharedPointer<IssoParam>& paramPtr)
{
    if (!paramPtr)
        return false;
    //
    IssoMultiSensorStateParam* newParam =
            dynamic_cast<IssoMultiSensorStateParam*>(paramPtr.data());
    if (!newParam)
        return false;
    //
    // сформировать список измененных параметров:
    //
    // данные мультидатчиков, изменивших состояние
    QMap<quint8, IssoMultiSensorData> changedDataMap;
    // получить текущий параметр мультидатчика по id
    auto curParam = dynamic_cast<IssoMultiSensorStateParam*>(stateParamById(newParam->id()));
    // если данные получены впервые,
    // добавить все мультидатчики в коллекцию измененных
    if (!curParam)
        changedDataMap = newParam->dataMap();
    // иначе получить коллекцию измененных мультидатчиков
    else
        changedDataMap = curParam->diffDataMap(newParam->dataMap());
    //
    // определить, изменились ли состояния мультидатчиков
    bool statesChanged = !changedDataMap.isEmpty();
    // если состояния изменились
    if (statesChanged)
    {
        // создать параметр, содержащий данные измененных мультидатчиков
        auto changedParam = new IssoMultiSensorStateParam(newParam->id(), changedDataMap);
        // добавить в коллекцию измененных параметров
        m_changedParams.append(QSharedPointer<IssoParam>(changedParam));
    }
    // обновить состояние цифрового шлейфа
    updateAddressChainState(newParam);
    // обновить параметр
    m_stateParams.insertParamPtr(paramPtr);
    //
    // вернуть результат
    return statesChanged;
}


void IssoModule::setOnline()
{
    if (m_moduleState != INACTIVE)
        return;
    //
    for (int i = 0; i < m_stateParams.paramCount(); ++i)
    {
        // получить параметр состояния датчика
        QSharedPointer<IssoParam> stateParamPtr = m_stateParams.paramPtrByIndex(i);
        if (stateParamPtr.isNull())
            continue;
        //
        // привести к параметру состояния датчика
        IssoSensorStateParam* stateParam =
                dynamic_cast<IssoSensorStateParam*>(stateParamPtr.data());
        if (!stateParam)
            continue;
        //
        // привести к параметру состояния мальтидатчика
        IssoMultiSensorStateParam* multiParam =
                dynamic_cast<IssoMultiSensorStateParam*>(stateParam);
        if (multiParam)
        {
            updateMultiSensorStateParam(stateParamPtr);
        }
        else
        {
            updateStateParam(stateParamPtr);
        }
    }
    // проанализировать состояния датчиков и реле
    // и установить результирующее состояние модуля
    analyzeSensorStates();
}


void IssoModule::setOffline()
{
    if (m_moduleState == INACTIVE)
        return;

    foreach (IssoParamId paramId, m_chains.keys())
    {
        IssoSensorChain* chain = sensorChain(paramId);
        chain->clearState();
    }
    //
    setModuleState(INACTIVE);
}


IssoParamBuffer IssoModule::stateParams() const
{
    return m_stateParams;
}


IssoSensorStateParam *IssoModule::stateParamById(const IssoParamId &paramId)
{
    return dynamic_cast<IssoSensorStateParam*>(m_stateParams.paramById(paramId));
}


void IssoModule::setParamBuffer(const IssoParamBuffer &params)
{
    m_stateParams = params;
}


QString IssoModule::moduleConfigName() const
{
    return m_moduleConfigName;
}


void IssoModule::setModuleConfigName(const QString &moduleConfigName)
{
    m_moduleConfigName = moduleConfigName;
}


void IssoModule::addNewChain(IssoParamId paramId, IssoChainConfig *chainConfig)
{
    if (!chainConfig)
        return;
    // получить ID шлейфа
    int chainId = paramIdToChainId(paramId);
    if (chainId == 0)
        return;
    // создать шлейф
    IssoSensorChain* chain = nullptr;
    // получить тип конфига
    IssoChainType type = chainConfig->type();
    switch(type)
    {
        case SMOKE_CHAIN:
        case TEMP_CHAIN:
        case ALARM_BUTTON_CHAIN:
        case MOVE_DETECTOR_CHAIN:
        {
            // привести к конфигу неадресного шлейфа
            auto analogCfg = dynamic_cast<IssoAnalogChainConfig*>(chainConfig);
            // получить количество датчиков
            if (analogCfg)
            {
                // создать визуальный шлейф
                chain = new IssoSensorChain(chainId, m_uniqueId, m_displayId,
                                            type, analogCfg->sensorCount());
            }
            break;
        }
        case MULTICRITERIAL:
        {
            // привести к конфигу адресного шлейфа
            auto addressCfg = dynamic_cast<IssoAddressChainConfig*>(chainConfig);
            // получить количество мультидатчиков
            if (addressCfg)
            {
                // создать визуальный шлейф
                chain = new IssoSensorChain(chainId, m_uniqueId, m_displayId, type, 0);
                // сформировать список мультидатчиков шлейф
                foreach (auto msCfg, addressCfg->multiSensorConfigs())
                {
                    // создать визуальный мультидатчик
                    auto multiSensor = new IssoSensor(msCfg->id(), m_uniqueId,
                                                      m_displayId, 1, type);
                    // задать инфо о расположении
                    multiSensor->setLocationInfo(msCfg->locationDesc());
                    // добавить в список шлейфа
                    chain->add(multiSensor);
                }
            }
            break;
        }
        default:
            break;
    }
    if (!chain)
        return;
    // получить исходную позицию для данного шлейфа
    QPointF pos = calcInitChainPosition(paramId);
    // задать исходную позицию датчиков
    chain->setInitPosition(pos);
    // добавить к модулю (в коллекцию и на сцену)
    insertSensorChain(paramId, chain);
}


void IssoModule::updateChain(IssoParamId paramId, IssoChainConfig *chainConfig)
{
    if (!chainConfig)
        return;
    // получить ID шлейфа
    int chainId = paramIdToChainId(paramId);
    if (chainId == 0)
        return;
    // получить шлейф
    IssoSensorChain* chain = sensorChain(paramId);
    if (!chain)
        return;
    //
    // получить тип конфига
    IssoChainType type = chain->type();
    //
    // задать настройки
    chain->setId(chainId);
    chain->setModuleUniqueId(m_uniqueId);
    chain->setModuleDisplayId(m_displayId);
    chain->setType(type);
    // выполнить действия, специфичные для типа
    switch(type)
    {
        case SMOKE_CHAIN:
        case TEMP_CHAIN:
        case ALARM_BUTTON_CHAIN:
        case MOVE_DETECTOR_CHAIN:
        {
            // привести к конфигу неадресного шлейфа
            auto analogCfg = dynamic_cast<IssoAnalogChainConfig*>(chainConfig);
            // получить количество датчиков
            if (analogCfg)
            {
//                // создать визуальный шлейф
//                chain = new IssoSensorChain(chainId, m_uniqueId, m_displayId,
//                                            type, analogCfg->sensorCount());

                // задать количество датчиков на шлейфе
                chain->setSensorCount(analogCfg->sensorCount());
            }
            break;
        }
        case MULTICRITERIAL:
        {
            // привести к конфигу адресного шлейфа
            auto addressCfg = dynamic_cast<IssoAddressChainConfig*>(chainConfig);
            // получить количество мультидатчиков
            if (addressCfg)
            {
                // получить ID существующих датчиков
                auto curSensorIds = chain->sensorIds();
                // сформировать список мультидатчиков шлейфа
                foreach (auto msCfg, addressCfg->multiSensorConfigs())
                {
                    // получить ID мультидатчика из конфига
                    int sensorId = msCfg->id();
                    // найти датчик
                    auto multiSensor = chain->sensor(sensorId);
                    // если датчик не найден, создать его
                    if (!multiSensor)
                    {
                        // создать визуальный мультидатчик
                        multiSensor = new IssoSensor(sensorId, m_uniqueId,
                                                     m_displayId, 1, type);
                        // добавить в список шлейфа
                        chain->add(multiSensor);
                    }
                    // задать инфо о расположении
                    multiSensor->setLocationInfo(msCfg->locationDesc());
                    //
                    // удалить обработанный ID из списка
                    curSensorIds.removeAll(sensorId);
                }
                // удалить из шлейфа датчики, не входящие в новую конфигурацию
                foreach (auto id, curSensorIds)
                    chain->removeById(id);
            }
            break;
        }
        default:
            break;
    }
}


void IssoModule::updateChains(QMap<IssoParamId, IssoChainConfig *> chainConfigs)
{
    // получить ID существующих шлейфов
    auto curChainIds = m_chains.keys();
    // получить ID шлейфов из новых конфигураций
    auto newChainIds = chainConfigs.keys();
    // обойти все новые конфигурации
    foreach (IssoParamId id, newChainIds)
    {
        // получить конфиг шлейфа
        IssoChainConfig* cfg = chainConfigs.value(id);
        if (!cfg)
            continue;
        // если шлейф с текущим ID отсутствует, добавить
        if (!curChainIds.contains(id))
            addNewChain(id, cfg);
        else
            // иначе обновить
            updateChain(id, cfg);
        // удалить из списка обработанный ID
        curChainIds.removeAll(id);
    }
    // удалить шлейфы, оставшиеся в списке
    foreach (IssoParamId id, curChainIds)
        removeSensorChain(id);
}



void IssoModule::updateAnalogChainState(IssoSensorStateParam *param)
{
    IssoSensorChain* chain = sensorChain(param->id());
    if (!chain)
        return;
    // сформировать подсказку
    QString hint = HtmlUtils::makeAnalogChainHint(chain->moduleDisplayId(),
                                                  chain->id(),
                                                  param->state());
    // задать состояние аналогового шлейфа
    chain->setStateToAllSensors(param->state(), hint);
    updateSensorsVisibility(chain);
}


void IssoModule::updateAddressChainState(IssoMultiSensorStateParam *param)
{
    IssoSensorChain* chain = sensorChain(param->id());
    if (!chain)
        return;
    foreach (IssoMultiSensorData data, param->dataMap())
    {
        // если мультидатчик на сервисном обслуживании, не обрабатывать
        if (hasServiceMultiSensorId(data.multiSensorId()))
            continue;
        //
        // получить расположение мультидатчика
        QString location = chain->sensorLocation(data.multiSensorId());
        //
        // сформировать подсказку
        QString hint = HtmlUtils::makeMultiSensorHint(displayName(),
                                                      data.multiSensorId(),
                                                      data.state(),
                                                      location,
                                                      data.getDigitalSensorStates());
        // задать состояние мультидатчика на цифровом шлейфе
        chain->setStateToSingleSensor(data.multiSensorId(), data.state(), hint);
        updateSensorsVisibility(chain);
    }
}


void IssoModule::displayAllSensors()
{
    setSensorDisplayMode(SENSOR_DISPLAY_ALL);
}


void IssoModule::displayAlarmSensors()
{
    setSensorDisplayMode(SENSOR_DISPLAY_ALARM);
}


void IssoModule::hideAllSensors()
{
    setSensorDisplayMode(SENSOR_HIDE_ALL);
}


void IssoModule::insertSensorChain(IssoParamId id, IssoSensorChain *chain)
{
    if (!chain)
        return;
    // удалить шлейф с данным ID
    removeSensorChain(id);
    // добавить шлейф в коллекцию
    m_chains.insert(id, QSharedPointer<IssoSensorChain>(chain));
}


void IssoModule::removeSensorChain(IssoParamId id)
{
    // если в коллекции нет соответствующего шлейфа, выход
    if (!m_chains.contains(id))
        return;
    // получить шлейф по ID
    IssoSensorChain* chain = sensorChain(id);
    // убрать шлейф с графплана
    if (chain)
        chain->hideAllSensors();
    // удалить шлейф из коллекции
    m_chains.remove(id);
}


void IssoModule::removeAllSensorChains()
{
    foreach (IssoParamId id, m_chains.keys())
    {
        removeSensorChain(id);
    }
}


IssoSensorChain *IssoModule::sensorChain(IssoParamId id)
{
    // получить шлейф по ID
    QSharedPointer<IssoSensorChain> p = m_chains.value(id);
    // вернить указатель на шлейф или nullptr
    return (p.isNull() ? nullptr : p.data());
}


QPointF IssoModule::calcInitChainPosition(IssoParamId id)
{
    QPointF pos;
    switch(id)
    {
        case ANALOG_CHAIN_1:
        {
            pos = QPointF(x(),
                          y() + height());
            break;
        }
        case ANALOG_CHAIN_2:
        {
            pos = QPointF(x() + IssoSensor::DEFAULT_SIZE,
                          y() + height());
            break;
        }
        case ANALOG_CHAIN_3:
        {
            pos = QPointF(x() + 2 * IssoSensor::DEFAULT_SIZE,
                          y() + height());
            break;
        }
        case ANALOG_CHAIN_4:
        {
            pos = QPointF(x(),
                          y() + height() + IssoSensor::DEFAULT_SIZE);
            break;
        }
        case ANALOG_CHAIN_5:
        {
            pos = QPointF(x() + IssoSensor::DEFAULT_SIZE,
                          y() + height() + IssoSensor::DEFAULT_SIZE);
            break;
        }
        case ANALOG_CHAIN_6:
        {
            pos = QPointF(x() + 2 * IssoSensor::DEFAULT_SIZE,
                          y() + height() + IssoSensor::DEFAULT_SIZE);
            break;
        }
        case MULTISENSOR:
        {
            pos = QPointF(x() + 3 * IssoSensor::DEFAULT_SIZE,
                          y() + height());
            break;
        }
        default:
            break;
    }
    return pos;
}


QList<QSharedPointer<IssoParam> > IssoModule::changedParams() const
{
    return m_changedParams;
}


void IssoModule::clearChangedParams()
{
    m_changedParams.clear();
}


bool IssoModule::hasServiceMultiSensorId(quint8 multiSensorId)
{
    return m_serviceMultiSensorIds.contains(multiSensorId);
}


void IssoModule::setMultiSensorServiceMode(quint8 multiSensorId, bool serviceMode)
{
    IssoSensorChain* chain = sensorChain(MULTISENSOR);
    if (!chain)
        return;
    //
    // перевести мультидатчик в сервисный режим
    if (serviceMode)
    {
        // добавить ID мультидатчика в коллекцию
        m_serviceMultiSensorIds.insert(multiSensorId);
        // задать сервисное состояние и сбросить хинт мультидатчика
        chain->setStateToSingleSensor(multiSensorId, STATE_SERVICE);
    }
    // вывести мультидатчик из сервисного режима
    else
    {
        // удалить ID мультидатчика из коллекции
        m_serviceMultiSensorIds.remove(multiSensorId);
        // задать неопределенное состояние и сбросить хинт мультидатчика
        chain->setStateToSingleSensor(multiSensorId, STATE_UNDEFINED);
    }
}


void IssoModule::setMultiSensorActivated(quint8 multiSensorId, bool activated)
{
    // получить адресный шлейф (мультидатчиков)
    IssoSensorChain* chain = sensorChain(MULTISENSOR);
    if (!chain)
        return;
    // найти мультидатчик
    if (IssoSensor* multiSensor = chain->sensor(multiSensorId))
    {
        multiSensor->setActivated(activated);
    }
}


int IssoModule::msTotalCount()
{
    IssoSensorChain* chain = sensorChain(MULTISENSOR);
    return (chain ? chain->sensorCount() : 0);
}


int IssoModule::msAlarmCount()
{
    IssoSensorChain* chain = sensorChain(MULTISENSOR);
    return (chain ? chain->alarmCount() : 0);
}


QMap<IssoParamId, QSharedPointer<IssoSensorChain> > IssoModule::sensorChains() const
{
    return m_chains;
}


void IssoModule::createShape()
{
    // форма модуля
    m_shape.addRoundedRect(m_rect.x(), m_rect.y(),
                           m_rect.width(), m_rect.height(),
                           8, 8);
    // измерить шрифт
    QFont font(/*"Sans Serif"*/"Arial", TOP_TITLE_FONT_SIZE, QFont::Medium);
    QFontMetrics fm(font);
    // прямоугольник верхней части
    m_topTitleRect = this->shape().boundingRect();
    m_topTitleRect.setHeight(fm.height());
    // прямоугольник нижней части
    m_bottomTitleRect = this->shape().boundingRect();
    m_bottomTitleRect.adjust(0, m_topTitleRect.height(), 0, 0);
    //
    // фонари
    qreal ledSize = 16;
    QPainterPath ledPath;
    // нарисовать круг в центре координат
    ledPath.addEllipse(-ledSize * 0.5, -ledSize * 0.5, ledSize, ledSize);
    // нарисовать перекрестие
    ledPath.moveTo(-ledSize * 0.5, 0);
    ledPath.lineTo(ledSize * 0.5, 0);
    ledPath.moveTo(0, -ledSize * 0.5);
    ledPath.lineTo(0, ledSize * 0.5);
    // создать матрицу
    QMatrix matrix;
    // повернуть путь на заданный угол
    matrix.rotate(45);
    ledPath = matrix.map(ledPath);
    // #1
    // обнулить матрицу
    matrix.reset();
    // сместить фонарь в 1ую позицию
    matrix.translate(ledSize / 2, -ledSize / 2);
    ledPath = matrix.map(ledPath);
    // добавить фонарь к фигуре
    m_shape.addPath(ledPath);
    // #2
    // обнулить матрицу
    matrix.reset();
    // сместить фонарь во 2ую позицию
    matrix.translate(m_rect.width() - ledSize, 0);
    ledPath = matrix.map(ledPath);
    // добавить фонарь к фигуре
    m_shape.addPath(ledPath);
}


QString IssoModule::topTitle() const
{
    QString floorStr = (m_floorNum < 0 ?
                             QString("П%1").arg(abs(m_floorNum)) :
                             QString("%1").arg(m_floorNum, 2, 10, QLatin1Char('0')));
    return QString("%1-%2").arg(m_buildNum, 2, 10, QLatin1Char('0'))
            .arg(floorStr);
}


QString IssoModule::bottomTitle() const
{
    return QString("%1").arg(m_displayId, 3, 10, QLatin1Char('0'));
}


void IssoModule::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mousePressEvent(event);
    update();
}


void IssoModule::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
//    // вызвать метод предка
    QGraphicsItem::mouseMoveEvent(event);
    // обновить сцену
    update();
}


void IssoModule::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseReleaseEvent(event);
    update();
}


QVariant IssoModule::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange && scene())
    {
        // получить новую позицию модуля
        QPointF newPos = value.toPointF();
        // получить размеры сцены
        QRectF rect = scene()->sceneRect();
        // если левый верхний угол модуля вышел за пределы сцены
        if (!rect.contains(newPos))
        {
          // рассчитать x-координату позиции модуля
          newPos.setX(qMin(rect.right() - boundingRect().width(), qMax(newPos.x(), rect.left())));
          newPos.setY(qMin(rect.bottom() - boundingRect().height(), qMax(newPos.y(), rect.top())));
          // рассчитать y-координату позиции модуля
          return newPos;
        }
    }
    return QGraphicsObject::itemChange(change, value);
}


bool IssoModule::alarmed() const
{
    return (m_moduleState == ALARM);
}

