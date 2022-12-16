#include "issofloorview.h"
#include "issomodule.h"
#include <QWheelEvent>
#include <QtMath>
#include <QMessageBox>


IssoFloorView::IssoFloorView(QWidget *parent)
    : QGraphicsView(parent),
      m_background(QImage()),
      m_cursorPos(QPoint(0, 0))
{
    // настроить компонент
    this->setRenderHint(QPainter::Antialiasing, true);
    this->setDragMode(QGraphicsView::ScrollHandDrag);
    this->setOptimizationFlags(QGraphicsView::DontSavePainterState);
    this->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    this->setTransformationAnchor(QGraphicsView::NoAnchor);
    this->setScene(new QGraphicsScene(this));
    this->setCacheMode(QGraphicsView::CacheBackground);
    // убрать скроллбары
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // подключить обработку жестов
    grabGesture(Qt::PinchGesture);
    // инициализировать контекстное меню
    initMenu();
}


void IssoFloorView::setBackgroundImage(const QPixmap &pixmap)
{
    this->setBackgroundImage(pixmap.toImage());
}


void IssoFloorView::setBackgroundImage(const QImage &img)
{
    m_background = QImage(img);
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    // задать отображаемую область
    scene()->setSceneRect(0, 0, img.width(), img.height());
}


void IssoFloorView::selectModules(const QStringList &displayNames)
{
    // сформировать графический путь для выделения выбранных модулей
    QPainterPath path;
    // проитись по все модулям сцены
    foreach (QGraphicsItem* item, scene()->items())
    {
        IssoModule* module = (IssoModule*)item;
        bool selected = displayNames.contains(module->displayName());
        if (selected)
        {
            // получить регион отображения модуля в координатах сцены
            QRegion region = module->boundingRegion(module->sceneTransform());
            // добавить регион в графический путь
            path.addRegion(region);
        }
    }
    // задать выбранную область на сцене
    scene()->setSelectionArea(path);
}


void IssoFloorView::deselectAllModules()
{
    scene()->clearSelection();
}


QRectF IssoFloorView::visibleRect() const
{
    return mapToScene(viewport()->geometry()).boundingRect();
}


void IssoFloorView::setVisibleRect(const QRectF &rect, qreal scale)
{
    // сбросить трансформации
    resetTransform();
    // создать пустую трансформацию
    QTransform transform;
    // сохранить коэффициент масштабирования
    m_scaleFactor = scale;
    // задать новый масштаб
    qreal exp = qPow(qreal(2), m_scaleFactor);
    transform.scale(exp, exp);
    // сместить
    transform.translate(-rect.x(), -rect.y());
    // задать трансформацию
    setTransform(transform);
    // отобразить изменения
    update();
}


void IssoFloorView::setViewportOffset(const QPointF& offset)
{
    // сбросить трансформации
    resetTransform();
    // создать пустую трансформацию
    QTransform transform;
    // задать текущий масштаб
    qreal exp = qPow(qreal(2), m_scaleFactor);
    transform.scale(exp, exp);
    // сместить область отображения
    transform.translate(-offset.x(), -offset.y());
    // задать трансформацию
    setTransform(transform);
    // отобразить изменения
    update();
}


void IssoFloorView::addGraphicsItem(QGraphicsItem *item)
{
    // добавить объект на сцену
    scene()->addItem(item);
}


void IssoFloorView::removeGraphicsItem(QGraphicsItem *item)
{
    // удалить объект со сцены
    scene()->removeItem(item);
}


void IssoFloorView::initMenu()
{
    // обработчик пункта меню
    connect(&m_contextMenu, SIGNAL(triggered(QAction*)),
            this,           SLOT(processMenuTriggered(QAction*)));
}


void IssoFloorView::updateMenuItems(QGraphicsItem *item)
{
    // очистить контектсное меню
    m_contextMenu.clear();
    //
    // вывести пункты меню, соответствующие выбранному объекту:
    //
    // объект не выбран
    if (!item)
    {
        fillMenuItemsForView();
    }
    // датчик
    else if (auto sensor = dynamic_cast<IssoSensor*>(item))
    {
        fillMenuItemsFor(sensor);
    }
    // модуль
    else if (auto module = dynamic_cast<IssoModule*>(item))
    {
        fillMenuItemsFor(module);
    }
    // камера
    else if (auto camera = dynamic_cast<IssoCamera*>(item))
    {
        fillMenuItemsFor(camera);
    }
}


void IssoFloorView::fillMenuItemsFor(IssoSensor *sensor)
{
    if (!sensor)
        return;
    // заполнить контекстное меню согласно типу
    switch(sensor->chainType())
    {
//        case ANALOG:
//        {
//            m_contextMenu.addAction(tr("Сбросить тревоги"))->
//                    setData(QVariant::fromValue(RESET_ALARMS));
//            m_contextMenu.addAction(tr("Запросить расширенное состояние"))->
//                    setData(QVariant::fromValue(GET_RAW_DATA));
//            break;
//        }
        case MULTICRITERIAL:
        {
            m_contextMenu.addAction(tr("Сбросить тревогу"))->
                    setData(QVariant::fromValue(RESET_ALARM));
            m_contextMenu.addAction(tr("Включить сервисный режим"))->
                    setData(QVariant::fromValue(SERVICE_MODE_ON));
            m_contextMenu.addAction(tr("Отключить сервисный режим"))->
                    setData(QVariant::fromValue(SERVICE_MODE_OFF));
            m_contextMenu.addAction(tr("Запросить расширенное состояние"))->
                    setData(QVariant::fromValue(GET_RAW_DATA));
            m_contextMenu.addAction(tr("Автокалибровка"))->
                    setData(QVariant::fromValue(SELF_CALIBRATION));
            break;
        }
        case MOVE_DETECTOR_CHAIN:
        {
            m_contextMenu.addAction(tr("Сбросить тревогу"))->
                    setData(QVariant::fromValue(RESET_ALARM));
            m_contextMenu.addAction(tr("Сбросить все тревоги"))->
                    setData(QVariant::fromValue(RESET_ALL_ALARMS));
            break;
        }
        default:
            break;
    }
}


void IssoFloorView::fillMenuItemsFor(IssoModule *module)
{
    if (!module)
        return;
    m_contextMenu.addAction(tr("Показать тревожные датчики"),
                            [=]() { module->displayAlarmSensors(); });
    m_contextMenu.addAction(tr("Показать все датчики"),
                            [=]() { module->displayAllSensors(); });
//    m_contextMenu.addAction(tr("Скрыть все датчики"),
//                            [=]() { module->hideAllSensors(); });
}


void IssoFloorView::fillMenuItemsFor(IssoCamera *camera)
{
    if (!camera)
        return;
    // если камера отображается, добавить пункт "Скрыть"
    if (camera->online())
    {
        m_contextMenu.addAction(tr("Скрыть видео"),
                                [this, camera]()
                                { emit cameraTurned(camera->id(), false); });
    }
    // иначе добавить пункт "Показать"
    else
    {
        m_contextMenu.addAction(tr("Показать видео"),
                                [this, camera]()
                                { emit cameraTurned(camera->id(), true); });
    }
}


void IssoFloorView::fillMenuItemsForView()
{
    m_contextMenu.addAction(tr("Отправить конфигурацию объекта на сервер"),
                            [=]() { emit objectConfigUpdateRequested(); });
}


void IssoFloorView::contextMenuEvent(QContextMenuEvent *event)
{
    // определить элемент сцены, над которой был клик мыши
    QGraphicsItem* item = itemAt(event->pos());
//    if (!item)
//        return;
    // задать видимость пунктов меню
    updateMenuItems(item);
    // если меню не пусто, показать
    if (!m_contextMenu.isEmpty())
        m_contextMenu.popup(viewport()->mapToGlobal(event->pos()));
}


void IssoFloorView::addModule(IssoModule *module)
{
    if (!module)
        return;
    // добавить модуль на сцену
    addGraphicsItem(module);
    // отобразить шлейфы модуля, если они не в норме
    module->displayAlarmSensors();
}


void IssoFloorView::addModules(const QList<IssoModule *> &modules)
{
    foreach (IssoModule* module, modules)
        addModule(module);
}


void IssoFloorView::removeModule(IssoModule *module)
{
    // скрыть все шлейфы модуля
    module->hideAllSensors();
    // убрать модуль со сцены
    removeGraphicsItem(module);
}


void IssoFloorView::removeAllModules()
{
    foreach (QGraphicsItem* item, scene()->items())
    {
        // если объект - модуль, удалить со сцены
        IssoModule* module = dynamic_cast<IssoModule*>(item);
        if (module)
            removeModule(module);
    }
}


void IssoFloorView::addCamera(IssoCamera *camera)
{
    addGraphicsItem(camera);
}


void IssoFloorView::removeCamera(IssoCamera *camera)
{
    removeGraphicsItem(camera);
}


void IssoFloorView::addCameras(const QList<IssoCamera *> &cameras)
{
    foreach (IssoCamera* camera, cameras)
        addCamera(camera);
}


void IssoFloorView::removeAllCameras()
{
    for (int i = scene()->items().size() - 1; i >= 0; --i)
    {
        // получить графический объект
        QGraphicsItem* item = scene()->items().at(i);
        // если объект - камера, удалить со сцены
        IssoCamera* camera = dynamic_cast<IssoCamera*>(item);
        if (camera)
            scene()->removeItem(camera);
    }
}


qreal IssoFloorView::getScaleFactor() const
{
    return m_scaleFactor;
}


void IssoFloorView::setScaleFactor(const qreal &scaleFactor)
{
    // сбросить трансформации
    resetTransform();
    // сохранить коэффициент мастабирования
    m_scaleFactor = scaleFactor;
    // создать пустую трансформацию
    QTransform transform;
    // задать новый масштаб
    qreal exp = qPow(qreal(2), m_scaleFactor);
    transform.scale(exp, exp);
    // задать трансформацию
    setTransform(transform);
    // отобразить изменения
    update();
}


void IssoFloorView::zoomToCursor(qreal zoomDelta, QPoint widgetCursorPos)
{
    // преобразовать позицию курсора из координат виджета к координатам сцены
    QPointF sceneCursorPos = mapToScene(widgetCursorPos);
    // получить коэффициент масштабирования
    qreal scale = m_scaleFactor + zoomDelta;
    // получить текущие трансформации сцены
    QTransform currentTransform = this->transform();
    // получитить позицию курсора в координатах текущих трансформаций сцены
    QPointF mappedPos = currentTransform.map(sceneCursorPos);
    // создать пустую трансформацию
    QTransform transform;
    // сместить начало координат сцены в позицию курсора
    transform.translate(mappedPos.x(), mappedPos.y());
    // задать новый масштаб
    qreal exp = qPow(qreal(2), scale);
    transform.scale(exp, exp);
    // смастить сцену обратно при заданном масштабе
    transform.translate(-sceneCursorPos.x(), -sceneCursorPos.y());
    // если обе стороны масштабированного изображения меньше
    // соответствующей стороны области отображения
    QRect scaledRect = transform.mapRect(sceneRect().toRect());
    if ((scaledRect.width() < viewport()->geometry().width()) &&
        (scaledRect.height() < viewport()->geometry().height()))
    {
        // подогнать размеры под область отображения
        fitSceneToViewport();
        return;
    }
    // сохранить коэффициент масштабирования
    m_scaleFactor = scale;
    // задать трансформацию
    setTransform(transform);
    // отобразить изменения
    update();
}


void IssoFloorView::fitSceneToViewport()
{
    // размеры сцены
    qreal sceneWidth = sceneRect().width();
    qreal sceneHeight = sceneRect().height();
    // размеры области отображения
    qreal viewWidth = viewport()->geometry().width();
    qreal viewHeight = viewport()->geometry().height();
    // соотношения сторон сцены и области отображения
    qreal coefWidth = viewWidth / sceneWidth;
    qreal coefHeight = viewHeight / sceneHeight;
    // выбираем соотношение, которое меньше
    // (в таком случае сцена полностью уместится в области отображения)
    qreal zoom = (coefWidth < coefHeight ? coefWidth : coefHeight);
    //
    // zoom = 2 ^ scaleFactor
    // => scaleFactor = log2(zoom)
    // коэффициент масштабирования
    qreal scaleFactor = log2(zoom);
    // установить сцене масштаб
    setScaleFactor(scaleFactor);
    // известить об изменении области отображения
    emit visibleRectChanged(visibleRect(), m_scaleFactor);
}

void IssoFloorView::locateViewportOnModule(IssoModule *module)
{
    if (!module)
        return;
    // позиционировать сцену на модуль
    centerOn(module);
    // известить об изменении области отображения
    emit visibleRectChanged(visibleRect(), m_scaleFactor);
}


void IssoFloorView::processMenuTriggered(QAction *action)
{
    // получить позицию объекта, для которого вызвано контекстное меню
    QPoint pos = viewport()->mapFromGlobal(m_contextMenu.pos());
    // определить объект по позиции
    auto sensor = dynamic_cast<IssoSensor*>(itemAt(pos));
    if (!sensor)
        return;
    // получить тип действия
    IssoSensorAction sensorAction = action->data().value<IssoSensorAction>();
    // инициировать событие выбора пункта меню
    emit sensorActionPerformed(sensor->id(),
                               sensor->moduleUniqueId(),
                               sensor->chainId(),
                               sensor->chainType(),
                               sensorAction);
}


bool IssoFloorView::gestureEvent(QGestureEvent *event)
{
    // если жест сужения / расширения 2мя пальцами
    if (QGesture *pinch = event->gesture(Qt::PinchGesture))
    {
        // обработать жест зумирования
        pinchTriggered(static_cast<QPinchGesture*>(pinch));
        // разрешить перемещение сцены во время масштабирования
        setDragMode(QGraphicsView::ScrollHandDrag);
    }
    return true;
}


void IssoFloorView::pinchTriggered(QPinchGesture *gesture)
{
    if (gesture->changeFlags() & QPinchGesture::ScaleFactorChanged)
    {
        qreal sign = (gesture->scaleFactor() > 1 ? 1 : -1);
        // изменить масштаб сцены
        zoomToCursor(sign * ZOOM_FACTOR_DELTA, gesture->centerPoint().toPoint());
    }
    // известить об изменении области отображения
    emit visibleRectChanged(visibleRect(), m_scaleFactor);
}


void IssoFloorView::wheelEvent(QWheelEvent *event)
{
    // если нажат Control, выполнить зум
    if (event->modifiers() & Qt::ControlModifier)
    {
        qreal sign = (event->delta() > 0 ? 1 : -1);
        // изменить масштаб сцены
        zoomToCursor(sign * ZOOM_FACTOR_DELTA, mapFromGlobal(cursor().pos()));
        // отметить событие как обработанное
        event->accept();
    }
    else
        // делегировать родителю
        QGraphicsView::wheelEvent(event);
    // известить об изменении области отображения
    emit visibleRectChanged(visibleRect(), m_scaleFactor);
}


void IssoFloorView::mouseMoveEvent(QMouseEvent *event)
{
    QGraphicsView::mouseMoveEvent(event);
    // если сцена смещается
    if (event->buttons() & Qt::LeftButton)
        // известить об изменении области отображения
        emit visibleRectChanged(visibleRect(), m_scaleFactor);
}


void IssoFloorView::drawBackground(QPainter *painter, const QRectF &rect)
{
    // если фон не задан, отрисовка по умолчанию
    if (m_background.isNull())
    {
        QGraphicsView::drawBackground(painter, rect);
        return;
    }
    if (!painter)
        return;
    // отобразить фон
    painter->drawImage(0, 0, m_background);
}


bool IssoFloorView::event(QEvent *event)
{
    // добавить обработку жестов
    if (event->type() == QEvent::Gesture)
        return gestureEvent(static_cast<QGestureEvent*>(event));
    // вызвать обработчик предка
    return QGraphicsView::event(event);
}
