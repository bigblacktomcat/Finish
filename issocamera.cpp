#include "issocamera.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include "issocommondata.h"


IssoCamera::IssoCamera()
    : IssoCamera(0, IssoConst::DEFAULT_CAMERA_IP, IssoConst::DEFAULT_CAMERA_PORT, "")
{
}


IssoCamera::IssoCamera(int id, const QHostAddress &ip, quint16 port,
                       const QString &password, int buildNum, int floorNum,
                       QGraphicsItem *parent)
    : QGraphicsObject(parent),
      m_id(id),
      m_ip(ip),
      m_port(port),
      m_password(password),
      m_buildNum(buildNum),
      m_floorNum(floorNum),
      m_rect(QRectF(0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT)),
      m_fillColor(OFFLINE_COLOR),
      m_online(false)
{
    // сделать компонент выделяемым и перемещаемым
    setFlags(ItemIsSelectable | ItemIsMovable | ItemSendsGeometryChanges);
    // установить реакцию на вход / выход мыши из границ компонента
    setAcceptHoverEvents(true);
    // сформировать геометрию модуля
    createShape();
}


void IssoCamera::loadFromStream(QDataStream &stream)
{
    // ID
    stream >> m_id;
    // IP-адрес
    quint32 ipValue;
    stream >> ipValue;
    m_ip = QHostAddress(ipValue);
    // порт
    stream >> m_port;
    // пароль
    stream >> m_password;
    // координаты:
    // X
    int posX;
    stream >> posX;
    this->setX(posX);
    // Y
    int posY;
    stream >> posY;
    this->setY(posY);
    // здание
    stream >> m_buildNum;
    // этаж
    stream >> m_floorNum;
    // режим отображения
    stream >> (qint32&)m_sendMode;
}


void IssoCamera::saveToStream(QDataStream &stream)
{
    // ID
    stream << m_id;
    // IP-адрес
    stream << m_ip.toIPv4Address();
    // порт
    stream << m_port;
    // пароль
    stream << m_password;
    // координаты
    int posX = this->x();
    stream << posX;
    int posY = this->y();
    stream << posY;
    // здание
    stream << m_buildNum;
    // этаж
    stream << m_floorNum;
    // режим отображения
    stream << (qint32&)m_sendMode;
}


QRectF IssoCamera::boundingRect() const
{
    return m_rect;
}


QPainterPath IssoCamera::shape() const
{
    return m_shape;
}


void IssoCamera::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                       QWidget *widget)
{
    // подавить предупреждения
    Q_UNUSED(widget);
    painter->setRenderHints(QPainter::Antialiasing |
                            QPainter::SmoothPixmapTransform);
    // если мышь находится над компонентом, сделать фон светлее
    QColor fillColor = m_fillColor;
    if (option->state & QStyle::State_MouseOver)
    {
        fillColor = (fillColor == OFFLINE_COLOR ? QColor(Qt::darkGray).darker()
                                                : fillColor.lighter(130));
    }
    // сохранить настройки
    painter->save();
    // настроить обводку
    QPen pen = painter->pen();
    pen.setColor(fillColor);            // установить цвет обводки
    painter->setPen(pen);               // установить настроенную обводку
    // залить фон модуля
    painter->setBrush(fillColor);       // заливка
    painter->drawPath(shape());         // отрисовка
    // заголовок
    QString title = QString::number(m_id);
    // цвет заголовка
    painter->setPen(Qt::white);
    // установить шрифт
    QFont font = QFont("Arial", TITLE_FONT_SIZE, QFont::Bold);
    painter->setFont(font);
    // измерить заголовок
    QFontMetrics fm = QFontMetrics(font);
    int strWidth = fm.width(title);
    int strHeight = fm.capHeight();
    // рассчитать координаты заголовка
    qreal strX = m_titleRect.center().x() - strWidth / 2;
    qreal strY = m_titleRect.center().y() + strHeight / 2;
    // вывести заголовок
    painter->drawText(strX, strY, title);
    // восстановить настройки
    painter->restore();
}


bool IssoCamera::online() const
{
    return m_online;
}


void IssoCamera::setOnline(bool online)
{
    m_online = online;
    // установить цвет согласно состоянию
    m_fillColor = (online ? ONLINE_COLOR : OFFLINE_COLOR);
    // обновить
    update();
}


int IssoCamera::id() const
{
    return m_id;
}

QHostAddress IssoCamera::ip() const
{
    return m_ip;
}

void IssoCamera::setIp(const QHostAddress &ip)
{
    m_ip = ip;
}

quint16 IssoCamera::port() const
{
    return m_port;
}

void IssoCamera::setPort(const quint16 &port)
{
    m_port = port;
}

int IssoCamera::buildNum() const
{
    return m_buildNum;
}

void IssoCamera::setBuildNum(int buildNum)
{
    m_buildNum = buildNum;
}

int IssoCamera::floorNum() const
{
    return m_floorNum;
}

void IssoCamera::setFloorNum(int floorNum)
{
    m_floorNum = floorNum;
}


QString IssoCamera::displayName() const
{
    return tr("Камера-%1").arg(m_id);
}


QString IssoCamera::password() const
{
    return m_password;
}


void IssoCamera::setPassword(const QString &password)
{
    m_password = password;
}


void IssoCamera::setMovable(bool movable)
{
    // задать возможность перемещения камеры
    setFlag(ItemIsMovable, movable);
}


IssoCameraSendMode IssoCamera::sendMode() const
{
    return m_sendMode;
}


void IssoCamera::setSendMode(const IssoCameraSendMode &sendMode)
{
    m_sendMode = sendMode;
}


void IssoCamera::createShape()
{
    // база
    m_titleRect = QRectF(0, 0, m_rect.width() * 0.75f, m_rect.height());
    // объектив
    QVector<QPointF> v;
    v.append(QPointF(m_rect.width() * 0.75f, m_rect.height() * 0.6f));
    v.append(QPointF(m_rect.width() * 0.75f, m_rect.height() * 0.4f));
    v.append(QPointF(m_rect.width(), m_rect.height() * 0.2f));
    v.append(QPointF(m_rect.width(), m_rect.height() * 0.8f));
    QPolygonF lens(v);
    // форма камеры
    m_shape.addRoundedRect(m_titleRect, 4, 4);
    m_shape.addPolygon(lens);
}


