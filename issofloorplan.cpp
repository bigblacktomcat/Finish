#include "issofloorplan.h"

#include <QWheelEvent>
#include <QtMath>
#include <QHBoxLayout>
#include <QBuffer>
#include <QPixmap>
#include <QImageWriter>
#include <QImageReader>
#include <QDir>


IssoFloorPlan::IssoFloorPlan(int number, int buildNum, QObject *parent)
    : QObject(parent),
      m_number(number),
      m_buildNum(buildNum),
      m_scale(0),
      m_visibleRect(QRectF(0, 0, 0, 0)),
      m_backgroundBase64(QString()),
      m_backgroundSize(QSize(0, 0)),
      m_curModulePos(QPoint(0, 0))
{
}


QList<IssoCamera *> IssoFloorPlan::cameras() const
{
    return m_cameras;
}


QSize IssoFloorPlan::backgroundSize() const
{
    return m_backgroundSize;
}

IssoFloorPlan::IssoFloorPlan() : IssoFloorPlan(0, 0)
{
}


IssoFloorPlan::~IssoFloorPlan()
{
    qDebug() << "~IssoFloorPlan - " << this->displayName() << endl;
    //
    removeAllModules();
    removeAllCameras();
}


qreal IssoFloorPlan::scale() const
{
    return m_scale;
}


void IssoFloorPlan::setScale(const qreal &scale)
{
    m_scale = scale;
}


QRectF IssoFloorPlan::visibleRect() const
{
    return m_visibleRect;
}


void IssoFloorPlan::setVisibleRect(const QRectF &visibleRect)
{
    m_visibleRect = visibleRect;
}


int IssoFloorPlan::nextAvailableModuleDisplayId()
{
    QSet<int> displayIds;
    // заполнить набор порядковых номеров модулей
    foreach (IssoModule* module, m_modules)
        displayIds.insert(module->displayId());
    // найти незанятый номер
    for (int i = 1; i <= 999; ++i)
    {
        if (!displayIds.contains(i))
            return i;
    }
    // если все номера заняты, вернуть -1
    return -1;
}


void IssoFloorPlan::loadFromStream(QDataStream &stream)
{
//    /*======= Способ #1 =======*/
//    // считать фоновое изображение из массива:
//    //
//    // считать размер массива из потока
//    int imgSize;
//    stream >> imgSize;
//    // считать массив из потока
//    QByteArray ba = QByteArray(imgSize, 0);
//    QBuffer buffer(&ba);
//    buffer.open(QIODevice::ReadOnly);
//    stream.readRawData(ba.data(), ba.size());
//    // сформировать изображение из массива
//    QImage image;
//    image.load(&buffer, "JPG");
//    // установить фон
////    this->setBackgroundImage(image);
//    this->saveBackground(image);
//    // DEBUG
//    if (image.isNull())
//    {
//        qDebug() << "Размер изображения = " << imgSize;
//    }

//    /*======= Способ #2 =======*/
//    // считать фоновое изображение из массива:
//    //
//    // считать строку
//    QString strBase64;
//    stream >> strBase64;
//    // преобразовать строку в массив данных изображения
//    QByteArray ba = QByteArray::fromBase64(strBase64.toUtf8());
//    // сформировать изображение
//    QImage image = QImage::fromData(ba, "JPG");
//    // установить изображение на этаж
//    this->setBackgroundImage(image);
//    // DEBUG
//    if (image.isNull())
//    {
//        qDebug() << "Позиция в файле = " << stream.device()->pos();
//    }

    // номер здания
    stream >> m_number;
    // номер этажа
    stream >> m_buildNum;
    // подложка:
    //
    // считать строку
    stream >> m_backgroundBase64;
    // преобразовать строку в изображение
    QImage image = IssoCommonData::convertBase64ToImage(m_backgroundBase64);
    // размер подложки
    m_backgroundSize = image.size();
    //
    // масштаб
    stream >> m_scale;
    // отображаемая область
    stream >> m_visibleRect;
}


void IssoFloorPlan::saveToStream(QDataStream &stream)
{
//    // записать фоновое изображение в массив
//    QByteArray ba;
//    QBuffer buffer(&ba);
//    buffer.open(QIODevice::WriteOnly);
////    m_background.save(&buffer, "JPG");
//    QPixmap::fromImage(m_background).save(&buffer, "JPG");

//    /*======= Способ #1 =======*/
//    // записать размер массива в поток
//    stream << ba.size();
//    // записать массив в поток
//    stream.writeRawData(ba.data(), ba.size());

//    /*======= Способ #2 =======*/
//    // преобразовать изображение в строку
//    QString imgBase64 = QString::fromUtf8(ba.toBase64().data());
//    // записать строку в поток
//    stream << imgBase64;

    //
    // номер здания
    stream << m_number;
    // номер этажа
    stream << m_buildNum;
    //
    // подложка:
    //
    // записать строку в поток
    stream << m_backgroundBase64;
    //
    // масштаб
    stream << m_scale;
    // отображаемая область
    stream << m_visibleRect;
}


QImage IssoFloorPlan::background()
{
    return IssoCommonData::convertBase64ToImage(m_backgroundBase64);
}


void IssoFloorPlan::setBackground(const QImage &background)
{
    m_backgroundBase64 = IssoCommonData::convertImageToBase64(background);
    // запомнить размер подложки
    m_backgroundSize = background.size();
    // известить о смене подложки
    emit backgroundChanged(background);
}


void IssoFloorPlan::changeVisibleRect(const QRectF &rect, qreal scale)
{
    m_scale = scale;
    m_visibleRect = rect;

//    qDebug() << "changeVisibleRect";
//    qDebug() << QString("View <Этаж-%1>:    scale = %2      topleft = %3 : %4")
//                .arg(m_number)
//                .arg(m_scale)
//                .arg(m_visibleRect.x())
//                .arg(m_visibleRect.y()) << endl;
}


QPointF IssoFloorPlan::nextModulePosition()
{
    return m_curModulePos;
}


int IssoFloorPlan::number() const
{
    return m_number;
}


QString IssoFloorPlan::displayName() const
{
    return (m_number < 0 ? tr("Этаж-П%1").arg(abs(m_number)) :
                           tr("Этаж-%1").arg(m_number, 2, 10, QLatin1Char('0')));
}


void IssoFloorPlan::moduleNames(QStringList& names)
{
    names.clear();
    foreach (IssoModule* module, m_modules)
        names << module->displayName();
    names.sort();
}


QStringList IssoFloorPlan::moduleNamesByState(IssoModuleState state)
{
    QStringList names;
    foreach (IssoModule* module, m_modules)
    {
        if (module->moduleState() == state)
            names << module->displayName();
    }
    names.sort();
    return names;
}


void IssoFloorPlan::selectedModuleNames(QStringList &names)
{
    names.clear();
    foreach (IssoModule* module, m_modules)
    {
        if (module->isSelected())
            names << module->displayName();
    }
    names.sort();
}

QList<IssoModule*> IssoFloorPlan::modules()
{
    // сортировать списки по номеру здания
    std::sort(m_modules.begin(), m_modules.end(),
              [](const IssoModule* a, const IssoModule* b) ->
                    bool { return a->displayId() < b->displayId(); });
    return m_modules;
}


IssoModule *IssoFloorPlan::selectedModule()
{
    foreach (IssoModule* module, m_modules)
    {
        if (module->isSelected())
            return module;
    }
    return nullptr;
}


int IssoFloorPlan::buildNum() const
{
    return m_buildNum;
}


QSet<IssoModuleState> IssoFloorPlan::moduleStates() const
{
    QSet<IssoModuleState> states;
    foreach (IssoModule* module, m_modules)
    {
        // добавить состояние текущего модуля
        states.insert(module->moduleState());
    }
    return states;
}


int IssoFloorPlan::alarmCount() const
{
    int cnt = 0;
    foreach (IssoModule* module, m_modules)
    {
        if (module->moduleState() == ALARM)
            cnt++;
    }
    return cnt;
}


bool IssoFloorPlan::activated() const
{
    foreach (IssoModule* module, m_modules)
    {
        if (!module->isActivated())
            return false;
    }
    return true;
}


void IssoFloorPlan::addModule(IssoModule *module)
{
    // задать модулю номер здания
    module->setBuildNum(m_buildNum);
    // задать модулю номер этажа
    module->setFloorNum(m_number);
    // добавить модуль в список
    m_modules << module;
    // известить о добавлении модуля
    emit moduleAdded(module);

    // рассчитать позицию для добавления нового модуля
    if (m_curModulePos.x() + module->width() > m_backgroundSize.width())
    {
        m_curModulePos.setX(0);
        m_curModulePos.setY(m_curModulePos.y() + module->height());
    }
    else
    {
        m_curModulePos.setX(m_curModulePos.x() + module->width());
    }
}


bool IssoFloorPlan::removeModule(IssoModule *module)
{
    // удалить модуль из списка
    bool result = m_modules.removeOne(module);
    // если модуль удален из списка
    if (result)
    {
        // задать модулю номер здания
        module->setBuildNum(0);
        // задать модулю номер этажа
        module->setFloorNum(0);
        // известить об удалении модуля
        emit moduleRemoved(module);
    }
    return result;
}


void IssoFloorPlan::removeAllModules()
{
    // удалить все модули из списка этажа
    foreach (IssoModule* module, m_modules)
        removeModule(module);
}


IssoCamera *IssoFloorPlan::findCameraById(int id)
{
    foreach (IssoCamera* camera, m_cameras)
    {
        if (camera->id() == id)
            return camera;
    }
    return nullptr;
}


void IssoFloorPlan::addCamera(IssoCamera *camera)
{
    // добавить камеру в список
    m_cameras.append(camera);
    // задать камере номер здания
    camera->setBuildNum(m_buildNum);
    // задать камере номер этажа
    camera->setFloorNum(m_number);
    // известить о добавлении камеры
    emit cameraAdded(camera);
}


bool IssoFloorPlan::removeCamera(IssoCamera *camera)
{
    // задать камере номер здания
    camera->setBuildNum(0);
    // задать камере номер этажа
    camera->setFloorNum(0);
    // удалить камеру из списка объекта
    bool result = m_cameras.removeOne(camera);
    // если камера удалена, известить об удалении камеры
    if (result)
        emit cameraRemoved(camera);
    // вернуть результат
    return result;
}


void IssoFloorPlan::removeAllCameras()
{
    // удалить все камеры из списка этажа
    foreach (IssoCamera* camera, m_cameras)
        removeCamera(camera);
}


IssoModule *IssoFloorPlan::findByDisplayName(const QString &displayName) const
{
    foreach (IssoModule* module, m_modules)
    {
        if (module->displayName() == displayName)
            return module;
    }
    return nullptr;
}


IssoModule *IssoFloorPlan::findById(int id) const
{
    foreach (IssoModule* module, m_modules)
    {
        if (module->uniqueId() == id)
            return module;
    }
    return nullptr;
}


IssoModule *IssoFloorPlan::findByAddress(QHostAddress addr) const
{
    foreach (IssoModule* module, m_modules)
    {
        if (module->ip().isEqual(addr, QHostAddress::ConvertV4MappedToIPv4))
            return module;
    }
    return nullptr;
}
