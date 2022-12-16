#include "issoconfigutils.h"
#include "issoobjectitemdelegate.h"
#include "issoobjectquerymodel.h"
#include "objectsviewwindow.h"
#include "passworddialog.h"
#include "ui_objectsviewwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QMessageBox>
#include <QDialog>

ObjectsViewWindow::ObjectsViewWindow(IssoAppType appType, IssoAccount account,
                                     IssoGuiMode guiMode, IssoDataBase *db,
                                     QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ObjectsViewWindow)
{
    ui->setupUi(this);
    //
    // сформировать конекстное меню
    initMenu();
    // задать пользователя
    setAccount(account);
    // задать режим отображения
    setGuiMode(guiMode);
    // задать тип приложения
    setAppType(appType);
    // задать БД
    setDataBase(db);
    // задать тип приложения для опроса
    m_logPoller.setAppType(m_appType);
    // установить обработчики событий
    setEventHandlers();
    //
    // максимизировать окно
    setWindowState(Qt::WindowMaximized);
    // начать опрос сервера
    startPollingLog();
    //
    // обновить представление данных
    updateView();

//    // обновить выборку
//    selectObjects();
//    // получить общее состояние системы по состояниям объектов
//    IssoModuleState commonState = getCommonState(currentStates());
//    // обновить звуковое сопровождение по общему состоянию
//    setAlarmSoundByState(commonState);

    //
    ui->btnTest->setVisible(false);
}


ObjectsViewWindow::~ObjectsViewWindow()
{
    qDebug() << "~ObjectsViewWindow" << endl;
    // завершить опрос сервера
    stopPollingLog();
    delete ui;
}


IssoGuiMode ObjectsViewWindow::guiMode() const
{
    return m_guiMode;
}


void ObjectsViewWindow::setGuiMode(const IssoGuiMode &guiMode)
{
    m_guiMode = guiMode;
    // определить размер шрифта
    int fontSize = (guiMode == IssoGuiMode::STANDART_MONITOR ? 10 : 14);
    // определить размер отступов на кнопках
    int btnPadding = (guiMode == IssoGuiMode::STANDART_MONITOR ? 0 : 5);
    // определить минимальные размеры кнопок
    QSize minBtnSize = (guiMode == IssoGuiMode::STANDART_MONITOR ?
                                        QSize(80, 24) : QSize(100, 44));
    // сформировать дополнительный стиль
    QString extraStyle = QString("QWidget { font-size: %1pt; } "
                                 "QHeaderView { font-size: %1pt; } "
                                 "QPushButton { padding: %2px %2px; "
                                               "min-width: %3px; min-height: %4px; } ")
                                .arg(fontSize)
                                .arg(btnPadding)
                                .arg(minBtnSize.width())
                                .arg(minBtnSize.height());
    // получить новый стиль
    QString newStyle = styleSheet() + extraStyle;
    // установить новый стиль центральному виджету
    centralWidget()->setStyleSheet(newStyle);
    // установить новый стиль окну (нужно для передачи стиля диалогам)
    setStyleSheet(centralWidget()->styleSheet());
}


IssoAccount ObjectsViewWindow::account() const
{
    return m_account;
}


void ObjectsViewWindow::setAccount(const IssoAccount &account)
{
    m_account = account;
    ui->gbControls->setVisible((account.isSuperUser()));
}


void ObjectsViewWindow::displaySelectedObject()
{
    // определить выбранный модельный индекс
    QModelIndex curIndex = ui->tvObjects->selectionModel()->currentIndex();
    if (!curIndex.isValid())
        return;
    // отобразить графплан объекта
    // получить прокси-модель из индекса
    const QSortFilterProxyModel* model =
            dynamic_cast<const QSortFilterProxyModel*>(curIndex.model());
    if (!model)
        return;
    // получить значение поля id объекта
    int id = model->data(model->index(curIndex.row(), IssoObjectQueryModel::COL_ID/*0*/)).toInt();
    //
    // временные рамки событий (за текущие сутки)
    QDateTime now = QDateTime::currentDateTime();
    QDate date = now.date();
//    QTime timeFrom = now.addSecs(-1 * 60 * 60).time();
    QTime timeFrom = QTime(0, 0, 0);
    QTime timeTo = QTime(23, 59, 59);
    //
    // показать окно наблюдения с заданным ID объекта
    displayAppWindow(id, date, timeFrom, timeTo);
    // отобразить все события за последний час
    updateAppWindowEvents(id, date, timeFrom, timeTo);
    //
    // получить текущие состояния объектов
    QMap<int, QString> objStates = currentStates();
    // получить имя состояния выбранного объекта
    QString objStateName = objStates.value(id, IssoCommonData::stringByModuleState(UNKNOWN));
    // обновить статус связи с отображаемым объектом (СНК)
    updateDisplayingObjectOnlineState(id, objStateName);
}


void ObjectsViewWindow::turnSelectedObjectSoundOff()
{
    // определить модельный индекс выбранного объекта
    QModelIndex curIndex = ui->tvObjects->selectionModel()->currentIndex();
    if (!curIndex.isValid())
        return;
    // выключить звук
    turnObjectSound(curIndex, false);
}


void ObjectsViewWindow::turnSelectedObjectSoundOn()
{
    // определить модельный индекс выбранного объекта
    QModelIndex curIndex = ui->tvObjects->selectionModel()->currentIndex();
    if (!curIndex.isValid())
        return;
    // включить звук
    turnObjectSound(curIndex, true);
}


void ObjectsViewWindow::processContextMenuRequested(const QPoint &pos)
{
    // определить строку таблицы, над которой был клик мыши
    QModelIndex index = ui->tvObjects->indexAt(pos);
    if (!index.isValid())
        return;
    // задать видимость пунктов меню
    setMenuItemsVisible(index);
    // показать меню
    m_contextMenu.popup(ui->tvObjects->viewport()->mapToGlobal(pos));
}


void ObjectsViewWindow::initMenu()
{
    initMenuActions();
    // задать способ обработки контекстного меню
    ui->tvObjects->setContextMenuPolicy(Qt::CustomContextMenu);
    // обработчик вызова контекстного меню
    connect(ui->tvObjects, SIGNAL(customContextMenuRequested(QPoint)),
            this,          SLOT(processContextMenuRequested(QPoint)));
}


void ObjectsViewWindow::initMenuActions()
{
    // сформировать пункты меню
    m_contextMenu.addAction(ACT_DISPLAY_OBJECT,
                            this, SLOT(displaySelectedObject()));
    m_contextMenu.addAction(ACT_SOUND_ON,
                            this, SLOT(turnSelectedObjectSoundOn()));
    m_contextMenu.addAction(ACT_SOUND_OFF,
                            this, SLOT(turnSelectedObjectSoundOff()));
}


void ObjectsViewWindow::setMenuItemsVisible(const QModelIndex &curIndex)
{
    // получить модель
    auto model = curIndex.model();
    // определить состояние звука
    QString soundStateStr = model->data(model->index(curIndex.row(),
                                                     IssoObjectQueryModel::COL_SOUND)).toString();
    bool soundOn = (soundStateStr == tr("Вкл."));
    // задать активность пунктов
    foreach (QAction* action, m_contextMenu.actions())
    {
        if (action->text() == ACT_SOUND_ON)
            action->setVisible(!soundOn);
        else if (action->text() == ACT_SOUND_OFF)
            action->setVisible(soundOn);
    }
}


void ObjectsViewWindow::setDataBase(IssoDataBase *db)
{
    m_dataBase = db;
    // задать модель данных
    ui->tvObjects->setModel(m_dataBase->objectsProxyModel());
    // задать режим выделения одной строки
    ui->tvObjects->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tvObjects->setSelectionMode(QAbstractItemView::SingleSelection);
    // запретить редактирование БД
    ui->tvObjects->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // отслеживать курсор мыши
    ui->tvObjects->viewport()->setAttribute(Qt::WA_Hover);
    // установить делегат для кастомной отрисовки элементов журнала событий
    ui->tvObjects->setItemDelegate(new IssoObjectItemDelegate(this));
}


void ObjectsViewWindow::selectObjects()
{
    m_dataBase->selectAllObjects();
    // скрыть столбец id
    ui->tvObjects->horizontalHeader()->setSectionHidden(IssoObjectQueryModel::COL_ID/*0*/, true);
    // подогнать высоту строк под содержимое
    ui->tvObjects->resizeColumnsToContents();
    // адрес должен заполнять свободное пространство
    ui->tvObjects->horizontalHeader()->
            setSectionResizeMode(IssoObjectQueryModel::COL_ADDRESS/*4*/, QHeaderView::Stretch);
    // подогнать высоту строк под содержимое
    ui->tvObjects->resizeRowsToContents();
}


void ObjectsViewWindow::setEventHandlers()
{
    connect(ui->btnLoad,    SIGNAL(clicked(bool)),
            this,           SLOT(processBtnLoadClicked()));
    connect(ui->btnDelete,  SIGNAL(clicked(bool)),
            this,           SLOT(processBtnDeleteClicked()));
    connect(&m_logPoller, SIGNAL(sensorEventsReceived(QList<IssoEventData>)),
            this,         SLOT(processSensorEventsReceived(QList<IssoEventData>)));
    connect(&m_logPoller, SIGNAL(photoEventsReceived(QList<IssoEventData>)),
            this,         SLOT(processPhotoEventsReceived(QList<IssoEventData>)));

    connect(&m_logPoller, SIGNAL(pingEventsReceived(QList<IssoEventData>)),
            this,         SLOT(processPingEventsReceived(QList<IssoEventData>)));
    connect(&m_logPoller, &IssoRemoteLogPoller::remoteRequestFailed,
            [=]() { processPingEventsReceived(QList<IssoEventData>()); });
}



void ObjectsViewWindow::displayAppWindow(int objectId,
                                         const QDate &date,
                                         const QTime &timeFrom,
                                         const QTime &timeTo)
{
    // если окно наблюдения уже видимое
    if (m_appWindow)
    {
        // активировать его
        m_appWindow->activateWindow();
        // и выйти
        return;
    }
    //
    // загрузить охраняемый объект из БД
    IssoObject* obj = IssoConfigUtils::loadObject(m_dataBase, objectId);
    // создать окно приложения с новым объектом
    m_appWindow = new MainWindow(m_appType, m_account, m_guiMode, obj, m_dataBase);
    // установить заголовок окна
    m_appWindow->setWindowTitle(tr("Глобальный ситуационный центр - Объект-%2 (%1)")
//                                                .arg(obj->objectCard().name())
                                                .arg(obj->displayName())
                                                .arg(obj->id()));
    // установить фильтр событий
    m_appWindow->setEventFilter(date, timeFrom, timeTo, "");
    // активизировать фильтр событий
    m_appWindow->setEventFilterEnabled(true);
    //
    // уничтожить окно при закрытии
    m_appWindow->setAttribute(Qt::WA_DeleteOnClose, true);
    // обнулять указатель на окно при завершении
    connect(m_appWindow, &MainWindow::destroyed,
            [this]()
            {
                this->m_appWindow = nullptr;
            });
    // отобразить окно на весь экран
    m_appWindow->show();
}


void ObjectsViewWindow::startPollingLog()
{
    m_logPoller.start(POLL_INTERVAL);
}


void ObjectsViewWindow::stopPollingLog()
{
    m_logPoller.stop();
}


bool ObjectsViewWindow::checkEventIsAlarmed(const QString &stateName)
{
    // получить состояние датчика по его описанию
    IssoState state = IssoCommonData::sensorStateByString(stateName);
    // признак тревоги
    bool alarmed = false;
    // обработать в заисимости от типа приложения
    switch(m_appType)
    {
        case IssoAppType::GSC:
        {
            alarmed = (state == STATE_WARNING) ||
                        (state == STATE_ALARM);
            break;
        }
        case IssoAppType::SOO:
        {
            alarmed = (state == VOLTAGE_LOW) ||
                        (state == STATE_ERROR) ||
                        (state == STATE_SHORT_CIRCUIT) ||
                        (state == STATE_BREAK) ||
                        (state == STATE_FAIL);
            break;
        }
        default:
            break;
    }
    return alarmed;
}


bool ObjectsViewWindow::checkEventIsActual(const QString &stateName)
{
    // получить состояние датчика по его описанию
    IssoState state = IssoCommonData::sensorStateByString(stateName);
    // признак тревоги
    bool result = false;
    // обработать в заисимости от типа приложения
    switch(m_appType)
    {
        case IssoAppType::GSC:
        {
            result = (state == STATE_NORMAL) ||
                     (state == STATE_ALARM) ||
                     (state == STATE_WARNING) ||
                     (state == STATE_UNDEFINED);
            break;
        }
        case IssoAppType::SOO:
        {
            result = (state == VOLTAGE_LOW) ||
                     (state == VOLTAGE_MEDIUM) ||
                     (state == VOLTAGE_HIGH) ||
                     (state == STATE_NORMAL) ||
                     (state == STATE_ERROR) ||
                     (state == STATE_SELF_CONTROL) ||
                     (state == STATE_SHORT_CIRCUIT) ||
                     (state == STATE_BREAK) ||
                     (state == STATE_FAIL) ||
                     (state == STATE_SERVICE) ||
                     (state == STATE_UNDEFINED);
            break;
        }
        default:
            break;
    }
    return result;
}


void ObjectsViewWindow::updateAppWindowEvents(int objectId, const QDate &date,
                                              const QTime &timeFrom, const QTime &timeTo)
{
    if (!m_appWindow || !m_dataBase)
        return;
    // считать последние события объекта
    QList<IssoEventData> events;
    m_dataBase->readLastEvents(objectId, date, timeFrom, timeTo, events);
    // проанализировать события
    displayEvents(events);
}



void ObjectsViewWindow::displayEvents(const QList<IssoEventData> &events)
{
    // коллекция буферов параметров модулей
    QMap<QString, IssoParamBuffer*> map;
    // события мультидатчиков сгруппированные по времени
    // [eventId : [sensorName : data]]
    QMap<QString, QMap<QString, IssoEventData>> msEvents;
    //
    // преобразовать события в параметры
    foreach (IssoEventData event, events)
    {
//        // если событие - пинг (не обрабатывать)
//        if (!event.hasEventData())
//            continue;

        qDebug() << QString("[%1] %2: %3")
                    .arg(event.dateTime.toString("dd.MM.yyyy HH:mm:ss"))
                    .arg(event.moduleName)
                    .arg(event.sensorStateName) << endl;

        // если буфер данного модуля отсутствует в коллекции, добавить
        if (!map.contains(event.moduleName))
            map.insert(event.moduleName, new IssoParamBuffer());
        // получить буфер модуля
        IssoParamBuffer* buffer = map.value(event.moduleName);
        //
        // если имя датчика пусто, значит, модуль Неактивен
        if (event.sensorName.isEmpty())
        {
            buffer->clearParams();
        }
        else
        {
            // ID параметра
            IssoParamId paramId;
            // если имя датчика включает имя параметра адресного шлейфа,
            // значит, получено событие мультидатчика
            if (event.sensorName.contains(IssoCommonData::stringByParamId(MULTISENSOR)))
                paramId = MULTISENSOR;
            else
                // иначе получить ID параметра по имени датчика
                paramId = IssoCommonData::paramIdByString(event.sensorName);
            //
            // получить состояние параметра из имени состояния датчика
            IssoState paramState = IssoCommonData::sensorStateByString(event.sensorStateName);
            // сформировать параметр состояния
            IssoSensorStateParam* param = nullptr;
            switch(paramId)
            {
                // если параметр описывает состояние мультидатчика
                case MULTISENSOR:
                {
                    QString msEventId = event.moduleName;
                    // получить события мультидатчиков модуля
                    QMap<QString, IssoEventData> msDataMap = msEvents.value(msEventId);
                    // добавить событие мультидатчика в коллекцию событий модуля
                    msDataMap.insert(event.sensorName, event);
                    // добавить коллекцию событий модуля в коллекцию событий для последующей обработки
                    msEvents.insert(msEventId, msDataMap);
                    break;
                }
                //
                // состояние датчика со значением
                case VOLT_BATTERY:
                case VOLT_BUS:
                case VOLT_SENSORS:
                case TEMP_OUT:
                {
                    // получить сырое значение датчика
                    float rawValue = (float)event.extValue / 10.0f;
                    // сформировать параметр датчика со значением
                    param = new IssoSensorExtStateParam(paramId, paramState, rawValue);
                    break;
                }
                //
                // состояние встроенного датчика
                case BTN_FIRE:
                case BTN_SECURITY:
                case SENSOR_SMOKE:
                case SENSOR_CO:
                case SUPPLY_EXTERNAL:
                case SUPPLY_INTERNAL:
                case VIBRATION:
                case SENSOR_OPEN:
                // состояние аналогового шлейфа
                case ANALOG_CHAIN_1:
                case ANALOG_CHAIN_2:
                case ANALOG_CHAIN_3:
                case ANALOG_CHAIN_4:
                case ANALOG_CHAIN_5:
                case ANALOG_CHAIN_6:
                // состояние реле (кнопки)
                case INT_RELAY_1:
                case INT_RELAY_2:
                case INT_RELAY_3:
                case INT_RELAY_4:
                case INT_RELAY_5:
                case INT_RELAY_6:
                case EXT_RELAY_1:
                case EXT_RELAY_2:
                case EXT_RELAY_3:
                case EXT_RELAY_4:
                case EXT_RELAY_5:
//                case OUT_ALARM:
//                case OUT_SIGNAL:
                {
                    param = new IssoSensorStateParam(paramId, paramState);
                    break;
                }
                default:
                {
                    break;
                }
            }
            if (!param)
                continue;
            // если параметр - не мультидатчик, добавить в буфер
            if (paramId != MULTISENSOR)
                buffer->insertParamPtr(QSharedPointer<IssoParam>(param));
        }
    }
    // если есть события мультидатчиков, обработать
    parseMsEvents(msEvents, map);
    //
    // если окно наблюдения видимое
    if (m_appWindow)
    {
        // обработать буфер параметров каждого модуля
        foreach (QString moduleName, map.keys())
        {
            // получить буфер модуля
            IssoParamBuffer* buffer = map.value(moduleName);
            // обновить параметры модуля
            m_appWindow->updateModuleParams(moduleName, *buffer);
            // удалить буфер
            delete buffer;
        }
    }
}


void ObjectsViewWindow::parseMsEvents(const QMap<QString, QMap<QString, IssoEventData>>& msEvents,
                                      QMap<QString, IssoParamBuffer *> &bufferMap)
{
    // для события адресного шлейфа обработать событие каждого мультидатчка
    foreach (QString eventId, msEvents.keys())
    {
        // имя модуля
        QString moduleName = "";
        // коллекция данных мультидатчиков модуля
        QMap<quint8, IssoMultiSensorData> dataMap;
        //
        // цикл по событиям мультидатчиков
        foreach (IssoEventData event, msEvents.value(eventId).values())
        {
//            qDebug() << QString("--- [%1]   %2:   %3 ")
//                                .arg(event.dateTime.toString("yyyy.MM.dd HH:mm:ss"))
//                                .arg(event.sensorName)
//                                .arg(event.sensorStateName) << endl;
            //
            // запомнить имя модуля
            if (moduleName.isEmpty())
                moduleName = event.moduleName;
            // выделить номер мультидатчика из имени
            int idx = event.sensorName.lastIndexOf("-");
            if (idx == -1)
                continue;
            // получить номер мультидатчика
            quint8 multiSensorId = event.sensorName.mid(idx + 1).trimmed().toInt();
            // получить состояния мультидатчика
            IssoState sensorState = IssoCommonData::sensorStateByString(event.sensorStateName);
            // получить расширенное значение мультидатчика
            quint16 detailsValue = (quint16)event.extValue;
            //
            // сформировать данные мультидатчика
            IssoMultiSensorData sensorData(multiSensorId,
                                           IssoMultiSensorData::stateToValue(sensorState),
                                           detailsValue);
            // добавить событие мультидатчика в коллекцию
            dataMap.insert(sensorData.multiSensorId(), sensorData);
        }
        // сформировать параметр шлейфа мультидатчиков
        auto param = new IssoMultiSensorStateParam(MULTISENSOR, dataMap);
        // если буфер данного модуля отсутствует в коллекции, добавить
        if (!bufferMap.contains(moduleName))
            bufferMap.insert(moduleName, new IssoParamBuffer());
        // получить буфер модуля
        IssoParamBuffer* buffer = bufferMap.value(moduleName);
        // добавить параметр в буфер
        buffer->insertParamPtr(QSharedPointer<IssoParam>(param));
    }
}


bool ObjectsViewWindow::checkIfObjectDataChanged(const IssoEventData &event, bool &stateChanged)
{
    int totalModules, totalMs, totalChannels;
    int alarmModules, alarmMs, alarmChannels;

    // инициализировать по умолчанию
    stateChanged = false;
    // получить модельный индекс по ID объекта
    QModelIndex idx = modelIndexByObjectId(event.objectId);
    if (!idx.isValid())
        return false;
    //
    // модель
    auto* model = ui->tvObjects->model();
    // состояние объекта
    QString state = model->index(idx.row(), IssoObjectQueryModel::COL_STATE/*5*/).data().toString();
    // общее число модулей
    totalModules = model->index(idx.row(), IssoObjectQueryModel::COL_MODULES_TOTAL/*6*/).data().toInt();
    // число тревожных модулей
    alarmModules = model->index(idx.row(), IssoObjectQueryModel::COL_MODULES_ALARM/*7*/).data().toInt();
    // общее число МД
    totalMs = model->index(idx.row(), IssoObjectQueryModel::COL_MS_TOTAL/*8*/).data().toInt();
    // число тревожных МД
    alarmMs = model->index(idx.row(), IssoObjectQueryModel::COL_MS_ALARM/*9*/).data().toInt();
    // общее число каналов
    totalChannels = model->index(idx.row(), IssoObjectQueryModel::COL_CHANNELS_TOTAL/*10*/).data().toInt();
    // число тревожных каналов
    alarmChannels = model->index(idx.row(), IssoObjectQueryModel::COL_CHANNELS_ALARM/*11*/).data().toInt();
    //
    // признак изменения состояния объекта
    stateChanged = (state != event.objectState);
    // признак изменения сводных данных
    bool summaryChanged = (totalModules != event.totalModules) ||
                          (alarmModules != event.alarmModules) ||
                          (totalMs != event.totalMs) ||
                          (alarmMs != event.alarmMs) ||
                          (totalChannels != event.totalChannels) ||
                          (alarmChannels != event.alarmChannels);
    //
    // вернуть результат
    return (stateChanged || summaryChanged);
}



void ObjectsViewWindow::updateAlarmSound()
{
//    qDebug() << IssoCommonData::stringByModuleState(state) << endl;

    // получить общее состояние звука
    auto state = getSoundState();
    // обработать состояние звука
    switch(state)
    {
        // тревога
        case ALARM:
        // неактивный модуль
        case INACTIVE:
        {
            // воспроизвести звук
            m_soundPlayer.playSound(SOUND_ALARM);
            break;
        }
        // не в сети
        case UNKNOWN:
        {
            // воспроизвести звук
            m_soundPlayer.playSound(SOUND_FAIL);
            break;
        }
        // иначе (если тревог и неисправностей не осталось)
        default:
        {
            // остановить звук
            m_soundPlayer.stopSound();
            break;
        }
    }
}

IssoModuleState ObjectsViewWindow::getSoundState()
{
    // текущее состояние всех объектов
    IssoModuleState resultState = ACTIVE;
    // обойти состояния всех объектов модели
    auto* model = ui->tvObjects->model();
    for (int row = 0; row < model->rowCount(); ++row)
    {
        // состояние звука
        QString soundState =
                model->data(model->index(row, IssoObjectQueryModel::COL_SOUND)).toString();
        // если звук не включен, не обрабатывать
        if (!soundState.contains(tr("Вкл.")))
            continue;
        // имя состояния объекта
        QString stateName =
                model->data(model->index(row, IssoObjectQueryModel::COL_STATE/*5*/)).toString();
        // получить состояние объекта по имени
        IssoModuleState state = IssoCommonData::moduleStateByString(stateName);
        // получить результирующее состояние
        resultState = (state < resultState ? state : resultState);
    }
    return resultState;
}


//IssoModuleState ObjectsViewWindow::getCommonState(const QMap<int, QString> &objectStates)
//{
//    // текущее состояние всех объектов
//    IssoModuleState commonState = ACTIVE;
//    // обновить состояние каждого объекта, если оно изменилось
//    foreach (QString newStateName, objectStates)
//    {
//        // получить новое состояние объекта по имени
//        IssoModuleState newObjectState =
//                IssoCommonData::moduleStateByString(newStateName);
//        // получить результирующее состояние
//        commonState = (newObjectState < commonState ? newObjectState : commonState);
//    }
//    return commonState;
//}


void ObjectsViewWindow::updateView()
{
    // обновить выборку
    selectObjects();
    // обновить звуковое сопровождение
    updateAlarmSound();
}


QSet<int> ObjectsViewWindow::objectIds()
{
    QSet<int> ids;
    auto* model = ui->tvObjects->model();
    for (int row = 0; row < model->rowCount(); ++row)
    {
        ids << model->index(row, IssoObjectQueryModel::COL_ID/*0*/).data().toInt();
    }
    return ids;
}


QMap<int, QString> ObjectsViewWindow::currentStates()
{
    QMap<int, QString> states;
    auto* model = ui->tvObjects->model();
    for (int row = 0; row < model->rowCount(); ++row)
    {
        // ID объекта
        int id = model->index(row, IssoObjectQueryModel::COL_ID/*0*/).data().toInt();
        // состояние объекта
        QString state = model->index(row, IssoObjectQueryModel::COL_STATE/*5*/).data().toString();
        // добавить в коллекцию
        states.insert(id, state);
    }
    return states;
}


QModelIndex ObjectsViewWindow::modelIndexByObjectId(int objectId)
{
    auto* model = ui->tvObjects->model();
    for (int row = 0; row < model->rowCount(); ++row)
    {
        // текущий модельный индекс
        QModelIndex curIndex = model->index(row, IssoObjectQueryModel::COL_ID/*0*/);
        // ID текущей записи
        int curId = curIndex.data().toInt();    /*model->data(curIndex).toInt();*/
        // если ID найден, вернуть модельный индекс
        if (curId == objectId)
            return curIndex;
    }
    return QModelIndex();
}


bool ObjectsViewWindow::objectExists(int objectId)
{
    return objectIds().contains(objectId);
}


QString ObjectsViewWindow::objectStateName(int objectId)
{
    // получить модельный индекс объекта
    auto idx = modelIndexByObjectId(objectId);
    if (!idx.isValid())
        return "";
    // вернуть строковое представление состояния объекта
    return idx.model()->index(idx.row(), IssoObjectQueryModel::COL_STATE/*5*/).data().toString();
}


void ObjectsViewWindow::updateDisplayingObjectOnlineState(int objectId,
                                                          const QString &objStateName)
{
    // если окно отображено
    if (m_appWindow)
    {
        // сверить с ID отображаемого объекта
        if (objectId != m_appWindow->guardedObject()->id())
            return;
        // если состояние объекта "Не в сети"
        if (objStateName == IssoCommonData::stringByModuleState(UNKNOWN))
            // отобразить объект как Offline
            m_appWindow->setObjectOffline();
        else
            // иначе как Online
            m_appWindow->setObjectOnline();
    }
}


void ObjectsViewWindow::turnObjectSound(const QModelIndex &objectIndex, bool soundOn)
{
    if (!objectIndex.isValid())
        return;
    // получить прокси-модель из индекса
    const QSortFilterProxyModel* proxyModel =
            dynamic_cast<const QSortFilterProxyModel*>(objectIndex.model());
    if (!proxyModel)
        return;
    // получить модель объектов из прокси-модели
    IssoObjectQueryModel* model =
            dynamic_cast<IssoObjectQueryModel*>(proxyModel->sourceModel());
    if (!model)
        return;
    // задать состояние звука (вкл./ выкл.)
    model->setData(objectIndex.model()->index(objectIndex.row(),
                                              IssoObjectQueryModel::COL_SOUND), soundOn);
    //
    // обновить состояние звука
    updateAlarmSound();
}


void ObjectsViewWindow::turnObjectSound(int objectId, bool soundOn)
{
    // получить модельный индекс объекта по ID
    QModelIndex index = modelIndexByObjectId(objectId);
    // установить состояние звука объекта
    turnObjectSound(index, soundOn);
}


void ObjectsViewWindow::closeEvent(QCloseEvent *event)
{
    // вывести диалог ввода пароля
    bool res = PasswordDialog::inputPassword(this, m_account.password(),
                                         tr("Введен неверный пароль! "
                                            "Приложение не будет завершено"));
    if (res)
    {
        // если окно наблюдения видимое, закрыть его
        if (m_appWindow)
            m_appWindow->close();
        // подтвердить закрытие
        event->accept();
    }
    else
        // отменить закрытие
        event->ignore();
}


IssoAppType ObjectsViewWindow::appType() const
{
    return m_appType;
}


void ObjectsViewWindow::setAppType(const IssoAppType &appType)
{
    // установить тип приложения
    m_appType = appType;
    // задать заголовок формы
    setWindowTitle(IssoCommonData::getAppTypeName(appType));
    // сформировать список анализируемых состояний датчиков
    // РЕАЛИЗОВАТЬ!!!

}


void ObjectsViewWindow::processBtnLoadClicked()
{
    QString fileName =
            QFileDialog::getOpenFileName(this,
                                         tr("Загрузить конфигурацию объекта"),
                                         QDir::currentPath(),
                                         tr("Конфигурация объекта (*.issocfg)"));
    if(fileName.isEmpty())
        return;

    QFile file(fileName);
    // открыть файл для чтения
    if (!file.open(QIODevice::ReadOnly))
        return;

    // прочитать данные конфигурации объекта
    QByteArray configData = file.readAll();
    //
    // загрузить объект из массива
    IssoObject obj;
    obj.load(configData);
    // если объект с таким ID уже содержится в БД,
    // известить и выйти
    if (IssoConfigUtils::objectExists(m_dataBase, obj.id()))
    {
        QMessageBox::warning(this, tr("Внимание"),
                             tr("Невозможно добавить объект с ID = %1, "
                                "т.к. объект с таким ID уже существует.")
                                                            .arg(obj.id()));
        return;
    }
    //
    // добавить конфигурацию в БД
    if (IssoConfigUtils::insertConfig(m_dataBase, configData))
        selectObjects();
}


void ObjectsViewWindow::processBtnDeleteClicked()
{
    if (ui->tvObjects->selectionModel()->selectedIndexes().size() == 0)
        return;
    // получить индекс выделенной строки
    QModelIndex index = ui->tvObjects->selectionModel()->selectedRows().first();
    // получить прокси-модель из индекса
    const QSortFilterProxyModel* model =
            static_cast<const QSortFilterProxyModel*>(index.model());
    if (!model)
        return;
    // получить id (стоблец 0) выбранной конфмгурации
    int id = model->data(model->index(index.row(), IssoObjectQueryModel::COL_ID/*0*/)).toInt();
    // получить имя объекта
    QString name = model->data(model->index(index.row(),
                                            IssoObjectQueryModel::COL_OBJ_NAME/*1*/)).toString();
    // запросить подтверждение у оператора
    if (!IssoCommonData::showQuestionDialog(this, tr("Подтверждение удаления"),
                               tr("Вы действительно хотите удалить "
                                  "конфигурацию объекта <%1>?").arg(name)))
        return;
    // удалить конфигурацию из БД
    if (IssoConfigUtils::deleteConfig(m_dataBase, id))
        selectObjects();
}


void ObjectsViewWindow::processSensorEventsReceived(const QList<IssoEventData> &events)
{
    if (events.isEmpty() || !m_dataBase)
        return;

    // начать транзакцию
    m_dataBase->beginTransaction();
    // обработать каждое JSON-сообщение
    foreach (IssoEventData rec, events)
    {
        // если объект - источник события не найден в таблице,
        // не обрабатывать
        if (!objectExists(rec.objectId))
            continue;
        // проверить, нужно ли обрабатывать сообщение
        if (!checkEventIsActual(rec.sensorStateName))
            continue;
        //
        // определить тревожность события
        bool alarmed;
        // если датчик не указан
        if (rec.sensorName.isEmpty())
            // значит модуль неактивен, т.е. тревога
            alarmed = true;
        else
            // определить наличие тревоги по состоянию датчика
            alarmed = checkEventIsAlarmed(rec.sensorStateName);
        //
//        // проверить наличие события в БД
//        bool eventExists = m_dataBase->checkEventExists(rec.dateTime, rec.moduleName,
//                                                        rec.sensorName, rec.sensorStateName,
//                                                        rec.extValue, rec.objectId);
//        // если событие новое - добавить
//        if (!eventExists)
//        {
            // добавить событие в БД
            m_dataBase->insertEvent(rec.dateTime, rec.moduleName, rec.sensorName,
                                    rec.sensorStateName, alarmed, rec.objectId, rec.extValue);
//        }
    }
    // завершить транзакцию
    m_dataBase->endTransaction();
    // если окно СНК отображено
    if (m_appWindow)
    {
        // обновить визуальную таблицу событий
        m_appWindow->updateEventTableView();
        // отобразить события на графплане
        displayEvents(events);
    }
}


void ObjectsViewWindow::processPhotoEventsReceived(const QList<IssoEventData> &events)
{
    if (events.isEmpty() || !m_dataBase)
        return;
    // начать транзакцию
    m_dataBase->beginTransaction();
    // обработать каждое JSON-сообщение
    foreach (IssoEventData rec, events)
    {
        // если объект - источник события не найден в таблице,
        // не обрабатывать
        if (!objectExists(rec.objectId))
            continue;
        //
        // заменить пробелы на +
        rec.frameData = rec.frameData.replace(" ", "+");
        // сохранить фотоснимок в БД
        m_dataBase->insertPhoto(rec.dateTime, rec.moduleName, rec.sensorName,
                                rec.cameraId, rec.cameraIp, rec.frameData);
    }
    // завершить транзакцию
    m_dataBase->endTransaction();
    // если окно СНК отображено
    if (m_appWindow)
    {
        // обновить визуальную таблицу событий
        m_appWindow->updateEventTableView();
    }
}


void ObjectsViewWindow::processPingEventsReceived(const QList<IssoEventData>& pingEvents)
{
    bool statesChanged = false;
    // коллекция ID объектов
    auto objIds = objectIds();
    //
    // начать транзакцию
    m_dataBase->beginTransaction();
    // обновить состояние каждого объекта, если оно изменилось
    // (анализируются только данные объектов из таблицы)
    foreach (auto event, pingEvents)
    {
        // если получено событие от объекта не из таблицы, игнорировать
        if (!objIds.contains(event.objectId))
            continue;
        // удалить обработанный ID объекта из
        objIds.remove(event.objectId);
        //
        // определить, изменились ли данные объекта
        bool objStateChanged;
        // если данные объекта не изменились, переходим к следующему событию
        if (!checkIfObjectDataChanged(event, objStateChanged))
            continue;
        // обновить данные об объекте в БД
        m_dataBase->updateObjectInfo(event.objectId, event.objectState,
                                      event.totalModules, event.alarmModules,
                                      event.totalMs, event.alarmMs,
                                      event.totalChannels, event.alarmChannels);
        // отметить общий признак изменения состояний
        statesChanged = true;
        //
        // если состояние объекта изменилось
        if (objStateChanged)
        {
            // обновить статус связи с объектом (СНК)
            updateDisplayingObjectOnlineState(event.objectId, event.objectState);
            //
            // если объект вернулся в Норму, включить звук
            if (IssoCommonData::moduleStateByString(event.objectState) == ACTIVE)
                turnObjectSound(event.objectId, true);
//            //
//            // отметить общий признак изменения состояний
//            statesChanged = true;
        }
    }
    //
    // если остались необработанные ID объектов, значит
    // объект не прислал ping
    QString offlineStateName = IssoCommonData::stringByModuleState(UNKNOWN);
    foreach (auto objId, objIds)
    {
        // если объект не в состоянии "Не в сети"
        if (objectStateName(objId) != offlineStateName)
        {
            // записать состояние "Не в сети" в БД
            m_dataBase->updateObjectState(objId, offlineStateName);
            // отметить общий признак изменения состояний
            statesChanged = true;
        }
    }
    //
    // завершить транзакцию
    m_dataBase->endTransaction();
    //
    // если состояние хотя бы одного объекта изменилось
    if (statesChanged)
    {
        // обновить представление данных
        updateView();
    }
}

//void ObjectsViewWindow::processRemotePingReceived(const QMap<int, QString> &recvStates)
//{
//    bool statesChanged = false;
//    // результирующие состояние объектов
//    QMap<int, QString> resultStates;
//    // получить текущие состояния объектов
//    QMap<int, QString> curStates = currentStates();
//    //
//    // начать транзакцию
//    m_dataBase->beginTransaction();
//    // обновить состояние каждого объекта, если оно изменилось
//    // (анализируются только данные объектов из таблицы)
//    foreach (int objId, curStates.keys())
//    {
//        // если коллекция новых состояний не содержит данных о текущем объекте
//        if (!recvStates.contains(objId))
//            // задать для объекта состояние "Не в сети" (не прислал ping)
//            resultStates.insert(objId, IssoCommonData::stringByModuleState(UNKNOWN));
//        else
//            // иначе копировать состояние объекта
//            resultStates.insert(objId, recvStates.value(objId));
//        //
//        // получить новое состояние объекта
//        QString newStateName = resultStates.value(objId);
//        // получить текущее состояние объекта
//        QString curStateName = curStates.value(objId);
//        // если состояние изменилось, записать его в БД
//        if (curStateName != newStateName)
//        {
//            // обновить состояние объекта в БД
//            m_dataBase->updateObjectState(objId, newStateName);
//            //
//            // обновить статус связи с объектом (СНК)
//            updateDisplayingObjectOnlineState(objId, newStateName);
//            //
//            // если объект вернулся в Норму, включить звук
//            if (IssoCommonData::moduleStateByString(newStateName) == ACTIVE)
//                turnObjectSound(objId, true);
//            //
//            // отметить признак изменения состояний
//            statesChanged = true;
//        }
//    }
//    // завершить транзакцию
//    m_dataBase->endTransaction();
//    //
//    // если состояние хотя бы одного объекта изменилось
//    if (statesChanged)
//    {
//        // обновить представление данных
//        updateView();

////        // обновить визуальную таблицу объектов
////        selectObjects();
////        // получить общее состояние системы по состояниям объектов
//////        IssoModuleState commonState = getCommonState(resultStates);
////        IssoModuleState commonState = getCommonState(currentStates());
////        // обновить звуковое сопровождение по общему состоянию
////        setAlarmSoundByState(commonState);
//    }
//}

//void ObjectsViewWindow::processRemotePingReceived(const QMap<int, QString> &objStates)
//{
//    bool statesChanged = false;
//    // результирующие состояние объектов
//    auto resultStates = objStates;
//    // получить текущие состояния объектов
//    QMap<int, QString> curStates = readObjectStates();

//    // начать транзакцию
//    m_dataBase->beginTransaction();
//    // обновить состояние каждого объекта, если оно изменилось
//    foreach (int objId, curStates.keys())
//    {
//        // если состояние объекта не найдено, значит объект "Не в сети"
//        if (!resultStates.contains(objId))
//            resultStates.insert(objId, IssoCommonData::stringByModuleState(UNKNOWN));
//        // получить новое состояние объекта
//        QString newStateName = resultStates.value(objId);
//        // получить текущее состояние объекта
//        QString curStateName = curStates.value(objId);
//        // если состояние изменилось, записать его в БД
//        if (curStateName != newStateName)
//        {
//            // обновить состояние объекта в БД
//            m_dataBase->updateObjectState(objId, newStateName);
//            //
//            // обновить статус связи с объектом (СНК)
//            updateObjectConnectionState(newStateName);
//            //
//            // отметить признак изменения состояний
//            statesChanged = true;
//        }
//    }
//    // завершить транзакцию
//    m_dataBase->endTransaction();
//    //
//    // если состояние хотя бы одного объекта изменилось
//    if (statesChanged)
//    {
//        // обновить визуальную таблицу объектов
//        selectObjects();
//        // получить общее состояние системы по состояниям объектов
//        IssoModuleState commonState = getCommonState(resultStates);
//        // обновить звуковое сопровождение по общему состоянию
//        updateAlarmSoundByState(commonState);
//    }
//}


