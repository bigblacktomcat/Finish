#include "issosensor.h"

#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOptionGraphicsItem>


IssoSensor::IssoSensor() : IssoSensor(0, 0, 0, 0, SMOKE_CHAIN)
{
}


IssoSensor::IssoSensor(quint8 id, int moduleUniqueId, int moduleDisplayId,
                       quint8 chainId, IssoChainType chainType,
                       QGraphicsItem *parent)
    : QGraphicsObject(parent),
      m_id(id),
      m_moduleUniqueId(moduleUniqueId),
      m_moduleDisplayId(moduleDisplayId),
      m_chainId(chainId),
      m_chainType(chainType),
      m_rect(QRectF(0, 0, DEFAULT_SIZE, DEFAULT_SIZE))
{
    // сделать компонент выделяемым и перемещаемым
    setFlags(ItemIsSelectable | ItemIsMovable | ItemSendsGeometryChanges);
    // установить реакцию на вход / выход мыши из границ компонента
    setAcceptHoverEvents(true);
    // сформировать геометрию модуля
    if (id != 0)
        createShape();
    // задать исходное состояние
    setState(STATE_UNDEFINED);
}


IssoSensor::~IssoSensor()
{
    stopMoveDetectionTimer();
    stopBlinking();
//    qDebug() << "~IssoSensor" << endl;
}


void IssoSensor::loadFromStream(QDataStream &stream)
{
    // номер датчика, уникальный в рамках шлейфа
    stream >> m_id;
    // уникальный ID модуля
    stream >> m_moduleUniqueId;
    // отображаемый ID модуля
    stream >> m_moduleDisplayId;
    // ID шлейфа
    stream >> m_chainId;
    // тип шлейфа, на котором расположен датчик
    stream >> m_chainType;

    // информация о расположении
    stream >> m_locationInfo;

    // признак активации
    stream >> m_activated;
    // позиция
    QPointF p;
    stream >> p;
    setPos(p);
    // сформировать геометрию модуля
    createShape();
}


void IssoSensor::saveToStream(QDataStream &stream)
{
    // номер датчика, уникальный в рамках шлейфа
    stream << m_id;
    // уникальный ID модуля
    stream << m_moduleUniqueId;
    // отображаемый ID модуля
    stream << m_moduleDisplayId;
    // ID шлейфа
    stream << m_chainId;
    // тип шлейфа, на котором расположен датчик
    stream << m_chainType;

    // информация о расположении
    stream << m_locationInfo;

    // признак активации
    stream << m_activated;
    // позиция
    stream << pos();
}


QRectF IssoSensor::boundingRect() const
{
    return m_rect;
}


QPainterPath IssoSensor::shape() const
{
    return m_shape;
}


void IssoSensor::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                       QWidget *widget)
{
    // подавить предупреждения
    Q_UNUSED(widget);
    painter->setRenderHints(QPainter::Antialiasing |
                            QPainter::SmoothPixmapTransform);
    // если мышь находится над компонентом, сделать фон светлее
    QColor fillColor = m_brushColor;
    if (option->state & QStyle::State_MouseOver)
    {
        fillColor = fillColor.lighter(130);
    }
    // сохранить настройки
    painter->save();
    // толщина обводки
    auto lineWidth = 1;
    // задать обводку
    painter->setPen(QPen(QBrush(Qt::black), lineWidth, Qt::SolidLine, Qt::RoundCap));
    // задать цвет заливки
    painter->setBrush(fillColor);
    // залить фон модуля
    painter->drawPath(shape());
    //
    // отобразить текст
    //
    // толщина обводки
    //lineWidth = 0.1; 2020-08-18 странно задавать целому числу такое значение
    lineWidth = 1;
    // задать обводку
    painter->setPen(QPen(QBrush(m_penColor), lineWidth, Qt::SolidLine, Qt::RoundCap));
    // задать цвет заливки
    painter->setBrush(m_penColor);
    // нарисовать путь
    painter->drawPath(m_textPath);
    //
    // отобразить значок
    //
    if (!m_iconPath.isEmpty())
    {
        switch(m_chainType)
        {
            case MOVE_DETECTOR_CHAIN:
            {
                // если состояние Тревога, отобразить таймер обнаружения
                if (m_state == STATE_ALARM)
                {
                    // сохранить состояние
                    painter->save();
                    // создать текстовое изображение времени таймера
                    QPainterPath timerPath =
                            makeTextPath(QFont("Arial", TIMER_FONT_SIZE, QFont::Bold),
                                         QString::number(m_detectionSeconds));
                    // поместить таймер рядом со датчиком
                    timerPath.translate(boundingRect().width() + 4,
                                        (boundingRect().height() - timerPath.boundingRect().height()) / 2);
                    // толщина обводки
                    //lineWidth = 0.1; 2020-08-18 странно задавать целому числу такое значение
                    lineWidth = 1;
                    // задать обводку
                    painter->setPen(QPen(QBrush(m_brushColor), lineWidth, Qt::SolidLine, Qt::RoundCap));
                    // задать цвет заливки
                    painter->setBrush(m_brushColor);
                    // нарисовать путь
                    painter->drawPath(timerPath);
                    // восстановить состояние
                    painter->restore();
                }
                //
                // настроить отрисовку пиктограммы
                lineWidth = 3;
                // задать цвет обводки
                painter->setPen(QPen(QBrush(m_penColor), lineWidth, Qt::SolidLine, Qt::RoundCap));
                // задать цвет заливки
                painter->setBrush(m_penColor);
                break;
            }
            default:
            {
                lineWidth = 3;
                // задать цвет обводки
                painter->setPen(QPen(QBrush(m_penColor), lineWidth, Qt::SolidLine, Qt::RoundCap));
                // задать цвет заливки
                painter->setBrush(QColor(Qt::transparent));
                break;
            }
        }
        // нарисовать значок
        painter->drawPath(m_iconPath);
    }
    // восстановить настройки
    painter->restore();
}


QColor IssoSensor::fillColor() const
{
    return m_brushColor;
}


void IssoSensor::setColors(const QColor &penColor, const QColor &brushColor)
{
    m_penColor = penColor;
    m_brushColor = brushColor;;
    update();
}


void IssoSensor::setMovable(bool movable)
{
    // задать возможность перемещения датчика
    setFlag(ItemIsMovable, movable);
}


quint8 IssoSensor::id() const
{
    return m_id;
}


int IssoSensor::moduleUniqueId() const
{
    return m_moduleUniqueId;
}


int IssoSensor::moduleDisplayId() const
{
    return m_moduleDisplayId;
}


quint8 IssoSensor::chainId() const
{
    return m_chainId;
}


IssoChainType IssoSensor::chainType() const
{
    return m_chainType;
}


IssoState IssoSensor::state() const
{
    return m_state;
}


void IssoSensor::setState(IssoState state)
{
    m_state = state;
    // остановить мигание
    stopBlinking();
    // получить цвет текста
    QColor stateTextColor = IssoCommonData::foregroundByState(state);
    // получить цвет фона
    QColor stateFillColor = IssoCommonData::backgroundByState(state);
    // задать цвета
    setColors(stateTextColor, stateFillColor);
    // если состояние Внимание, запустить мигание
    switch(state)
    {
        case STATE_WARNING:
        case STATE_BREAK:
        {
            // получить цвет текста мигания
            QColor blinkTextColor = IssoCommonData::foregroundByState(STATE_UNDEFINED);
            // получить цвет фона мигания
            QColor blinkFillColor = IssoCommonData::backgroundByState(STATE_UNDEFINED);
            // запустить мигание
            startBlinking(stateTextColor, stateFillColor, blinkTextColor, blinkFillColor);
            break;
        }
        default:
            break;
    }
    if (state == STATE_ALARM)
        startMoveDetectionTimer();
    else
        stopMoveDetectionTimer();
}


QColor IssoSensor::textColor() const
{
    return m_penColor;
}


bool IssoSensor::activated() const
{
    return m_activated;
}

void IssoSensor::setActivated(bool activated)
{
    m_activated = activated;
}

QString IssoSensor::locationInfo() const
{
    return m_locationInfo;
}

void IssoSensor::setLocationInfo(const QString &locationInfo)
{
    m_locationInfo = locationInfo;
}

void IssoSensor::createShape()
{
    m_shape = QPainterPath();
    // форма модуля
    if (m_chainType == MULTICRITERIAL)
        m_shape.addEllipse(m_rect);
    else
        m_shape.addRect(m_rect);
    //
    // измерить шрифт
    QFont font("Arial", TOP_TITLE_FONT_SIZE, QFont::Medium);
    QFontMetrics fm(font);
    // смещение от верхней и нижней границы
    qreal shift = fm.height() / 4;
    // прямоугольник верхней части
    m_topTitleRect = this->shape().boundingRect();
    m_topTitleRect.setHeight(fm.height());
    m_topTitleRect.moveTop(shift);
    // прямоугольник нижней части
    m_bottomTitleRect = this->shape().boundingRect();
    m_bottomTitleRect.adjust(0, m_topTitleRect.height() + shift, 0, -shift);
    //
    // сформировать значок
    //
    // размер значка
    qreal iconSize = m_bottomTitleRect.height() * 0.8;
    // создать изображение значка
    QPainterPath iconPath = makeIcon(iconSize);
    // сместить значок
    iconPath.translate(0, m_bottomTitleRect.y() + (m_bottomTitleRect.height() - iconSize) / 2);
    // добавить к пути значка
    m_iconPath.addPath(iconPath);
    //
    // сформировать заголовки:
    //
    // верхний заголовок (id модуля)
    //
    // создать текстовое изображение верхнего заголовка
    QPainterPath topTextPath =
            makeTextPath(QFont("Arial", TOP_TITLE_FONT_SIZE, QFont::Normal),
                         topTitle());
    // сместить заголовок в центр верхней области
    topTextPath.translate((m_topTitleRect.width() - topTextPath.boundingRect().width()) / 2,
                           (m_topTitleRect.height() - topTextPath.boundingRect().height()) / 2);
    // добавить к текстовому пути
    m_textPath.addPath(topTextPath);
    //
    // нижний заголовок (id датчика / шлейфа)
    //
    // для адресного - id датчика, для неадресного - id шлейфа
    QString bottomText = (m_chainType == MULTICRITERIAL ?
                              QString("%1").arg(m_id, 3, 10, QLatin1Char('0')) :
                              QString("%1").arg(m_chainId, 2, 10, QLatin1Char('0')));
    // создать текстовое изображение нижнего заголовка
    int fontSize = (m_chainType == MULTICRITERIAL ? 11 : 14);
    QPainterPath bottomTextPath =
            makeTextPath(QFont("Arial", fontSize, QFont::Normal), bottomText);
    // поместить id рядом со значком
    bottomTextPath.translate(iconSize,
                             m_bottomTitleRect.y() +
                             (m_bottomTitleRect.height() - bottomTextPath.boundingRect().height()) / 2);
    // добавить к текстовому пути
    m_textPath.addPath(bottomTextPath);
}


QString IssoSensor::topTitle() const
{
    return QString("%1").arg(m_moduleDisplayId, 3, 10, QLatin1Char('0'));
}


QString IssoSensor::bottomTitle() const
{
    QString str;
    switch(m_chainType)
    {
        case SMOKE_CHAIN:
        case TEMP_CHAIN:
        case ALARM_BUTTON_CHAIN:
        case MOVE_DETECTOR_CHAIN:
        {
            str = QString("Н%1").arg(m_chainId);
            break;
        }
        case MULTICRITERIAL:
        {
            str = QString("А%1")
                    .arg(m_id, 3, 10, QLatin1Char('0'));
            break;
        }
        default:
        {
            str = "";
            break;
        }
    }
    return str;
}


void IssoSensor::startBlinking(const QColor &textColor1, const QColor &fillColor1,
                               const QColor &textColor2, const QColor &fillColor2,
                               int interval)
{
    // задать интервал мигания
    m_blinkTimer.setInterval(interval);
    // задать метод смены цвета по таймеру
    connect(&m_blinkTimer, &QTimer::timeout,
            [this, textColor1, fillColor1, textColor2, fillColor2]()
            {
                // сменить цвета текста и заливки
                if (m_brushColor == fillColor1)
                    setColors(textColor2, fillColor2);
                else
                    setColors(textColor1, fillColor1);
            });
    // запустить мигание
    m_blinkTimer.start();
}


void IssoSensor::stopBlinking()
{
    // остановить мигание
    m_blinkTimer.stop();
    // отключить обработик таймера
    disconnect(&m_blinkTimer, 0, 0, 0);
}


void IssoSensor::startMoveDetectionTimer()
{
    if (!m_moveDetectionTimer.isActive())
    {
        m_detectionSeconds = 0;
        connect(&m_moveDetectionTimer, &QTimer::timeout,
                [this]()
                {
                    m_detectionSeconds++;
                    update();
                });
        m_moveDetectionTimer.start(1000);
    }
}


void IssoSensor::stopMoveDetectionTimer()
{
    m_moveDetectionTimer.stop();
    m_moveDetectionTimer.disconnect();
    m_detectionSeconds = 0;
}


QPainterPath IssoSensor::makeIcon(qreal iconSize)
{
    QPainterPath path;
    switch(m_chainType)
    {
        case SMOKE_CHAIN:
        {
            QPainterPath smokePath;
            // отступ от края
            qreal iconMargin = iconSize * 0.15;
            // область, занимаемая значком
            QRectF rect(0, 0, iconSize, iconSize);
            // значок должен занимать половину заданной ширины
            rect.adjust(0, 0, -rect.width() / 2, 0);
            // уменьшить на резмер отступа с каждой стороны
            rect.adjust(0, 0, -iconMargin * 2, -iconMargin * 2);
            // нарисовать значок
            smokePath.moveTo(rect.width(), 0);
            smokePath.lineTo(0, rect.height() * 0.25);
            smokePath.moveTo(0, rect.height() * 0.25);
            smokePath.lineTo(rect.width(), rect.height() * 0.75);
            smokePath.moveTo(rect.width(), rect.height() * 0.75);
            smokePath.lineTo(0, rect.height());
            // сместить значок в центр
            smokePath.translate((iconSize - rect.width()) / 2,
                                (iconSize - rect.height()) / 2);
            // добавить значок к результату
            path.addPath(smokePath);
            break;
        }
        case TEMP_CHAIN:
        {
            QPainterPath tempPath;
            // отступ от края
            qreal iconMargin = iconSize * 0.15;
            // область, занимаемая значком
            QRectF rect(0, 0, iconSize, iconSize);
            // значок должен занимать половину заданной ширины
            rect.adjust(0, 0, -rect.width() / 2, 0);
            // уменьшить на резмер отступа с каждой стороны
            rect.adjust(0, 0, -iconMargin * 2, -iconMargin * 2);
            //
            // область круга
            int radius = 3;
            // центр круга
            auto ellipseCenter = QPointF(rect.center().x(),
                                         rect.height() - radius);
            // нарисовать значок
            tempPath.addEllipse(ellipseCenter, radius, radius);

            tempPath.moveTo(rect.center().x(), 0);
            tempPath.lineTo(rect.center().x(),
                            rect.height() -  2 * radius );
            // сместить значок в центр
            tempPath.translate((iconSize - rect.width()) / 2,
                               (iconSize - rect.height()) / 2);
            // добавить значок к результату
            path.addPath(tempPath);
            break;
        }
        case MULTICRITERIAL:
        {
            // отступ от края
            qreal iconMargin = iconSize * 0.15;
            // область, занимаемая значком
            QRectF rect(0, 0, iconSize, iconSize);
            // значок должен занимать половину заданной ширины
            rect.adjust(0, 0, -rect.width() / 2, 0);
            // уменьшить на резмер отступа с каждой стороны
            rect.adjust(0, 0, -iconMargin * 2, -iconMargin * 2);
            //
            // значок #1
            //
            QPainterPath smokePath;
            // нарисовать значок #1
            smokePath.moveTo(rect.width(), 0);
            smokePath.lineTo(0, rect.height() * 0.25);
            smokePath.moveTo(0, rect.height() * 0.25);
            smokePath.lineTo(rect.width(), rect.height() * 0.75);
            smokePath.moveTo(rect.width(), rect.height() * 0.75);
            smokePath.lineTo(0, rect.height());
            //
            // значок #2
            //
            QPainterPath tempPath;
            // область круга
            int radius = 3;
            // центр круга
            auto ellipseCenter = QPointF(rect.center().x(),
                                         rect.height() - radius);
            // нарисовать значок
            tempPath.addEllipse(ellipseCenter, radius, radius);

            tempPath.moveTo(rect.center().x(), 0);
            tempPath.lineTo(rect.center().x(),
                            rect.height() -  2 * radius );
            // разместить значок #2 справа от значка #1
            tempPath.translate(smokePath.boundingRect().width() + iconMargin, 0);
            // добавить значок #2 к значку #1
            smokePath.addPath(tempPath);
            //
            // сместить значок в центр
            smokePath.translate((iconSize - smokePath.boundingRect().width()) / 2,
                                (iconSize - smokePath.boundingRect().height()) / 2);
            // добавить значок к результату
            path.addPath(smokePath);
            break;
        }
        case ALARM_BUTTON_CHAIN:
        {
            // отступ от края
            qreal iconMargin = iconSize * 0.15;
            // прямоугольник для вписывания дуги
            QRectF arcRect(-iconSize * 0.5, -iconSize * 0.5, iconSize, iconSize);
            // уменьшить на резмер отступа с каждой стороны
            arcRect.adjust(iconMargin, iconMargin, -iconMargin, -iconMargin);
            // нарисовать дугу
            path.moveTo(arcRect.left(), arcRect.center().y());
            path.arcTo(arcRect, 180, 180);
            // нарисовать линию
            path.moveTo(0, arcRect.bottom());
            path.lineTo(0, iconSize - iconMargin * 2);
            // сместить значок в центр
            path.translate(path.boundingRect().width() / 2 + (iconSize - path.boundingRect().width()) / 2,
                           (iconSize - path.boundingRect().height()) / 2);
            break;
        }
        case MOVE_DETECTOR_CHAIN:
        {
            int lineWidth = 3;
            // размер датчика
            qreal size = iconSize;
            // тело
            qreal bodyWidth = 1.5 * lineWidth;
            qreal bodyX1 = size / 2 - bodyWidth / 2;
            qreal bodyX2 = bodyX1 + bodyWidth;
            qreal bodyY1 = size / 3;
            qreal bodyY2 = size / 3 * 2;
            qreal bodyHeight = bodyY2 - bodyY1;
            // создать прямоугольник тела
            QRectF body(QPointF(bodyX1, bodyY1),
                        QPointF(bodyX2, bodyY2));
            // добавить тело
            path.addRoundRect(body, 6);
            //
            // голова
            qreal headR = size / 3 - size / 4;
            qreal headX = size / 2;
            qreal headY = bodyY1 - headR - lineWidth;
            // добавить голову
            path.addEllipse(QPointF(headX, headY), headR, headR);
            //
            // руки
            qreal leftHandX1 = bodyX1;
            qreal rightHandX1 = bodyX2;
            qreal leftHandX2 = leftHandX1 - bodyHeight / 2;
            qreal rightHandX2 = rightHandX1 + bodyHeight / 2;
            qreal handY1 = bodyY1;
            qreal handY2 = handY1 + bodyHeight / 3 * 2;
            // добавить левую руку
            path.moveTo(leftHandX1, handY1);
            path.lineTo(leftHandX2, handY2);
            // добавить правую руку
            path.moveTo(rightHandX1, handY1);
            path.lineTo(rightHandX2, handY2);
            //
            // ноги
            qreal legY1 = bodyY2;
            qreal legY2 = size - lineWidth;
            // добавить левую ногу
            path.moveTo(bodyX1, legY1);
            path.lineTo(bodyX1, legY2);
            // добавить правую ногу
            path.moveTo(bodyX2, legY1);
            path.lineTo(bodyX2, legY2);
            break;
        }
        default:
            break;
    }
    return path;
}


QPainterPath IssoSensor::makeTextPath(const QFont &font, const QString &text)
{
    // измерить заголовок
    QFontMetrics fm = QFontMetrics(font);
    int textHeight = fm.capHeight();
    // базовая линия текста
    QPointF baseline(0, textHeight);
    // создать путь
    QPainterPath textPath;
    // добавить текст к пути
    textPath.addText(baseline, font, text);
    return textPath;
}

