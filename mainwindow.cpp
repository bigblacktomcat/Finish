#include "mainwindow.h"
#include "issomodule.h"
#include "ui_form.h"
#include "issomodulerequest.h"
#include "issomodulereply.h"
#include "issomoduledescriptor.h"
#include "passworddialog.h"
#include "issologmessagebuilder.h"
#include "issocamera.h"
#include "issochainconfig.h"
#include "issomoduleconfig.h"
#include "issoobjectutils.h"

#include <QDataStream>
#include <QMessageBox>


MainWindow::MainWindow(IssoAppType appType, IssoAccount account,
                       IssoGuiMode guiMode, IssoObject* guardedObject,
                       IssoDataBase* db, QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainForm),
      m_account(account)
{
    ui->setupUi(this);
    //
    if (!guardedObject)
        return;
    //
    // инициализировать ядро
    m_core.init(guardedObject);
    //
    // зарегистрировать пользовательские типы для передачи в сигналы/слоты
    registerMetaTypes();
    //
    // установить тип приложения
    setAppType(appType);
    // инициализировать форму
    init();
    // установить режим отображения ГПИ
    ui->tabGraphPlan->init(appType, guiMode);
    //
    // задать охраняемый объект для всех вкладок
    setGuardedObject(guardedObject);

    //
    // инициализировать БД
    setDataBase(db);
    // запустить таймер пинга (только для СНК)
    if (m_appType == IssoAppType::SNK)
    {
        // если режим админа, запустить обмен данными без опроса модулей,
        // иначе запустить обмен данными с опросом модулей
        m_core.exchanger()->startExchanging(!account.isSuperUser());
        // установить интервал отправки пинга на сервер
        // PING_INTERVAL - по умолчанию 2500 мс
        // m_pingTimer.setInterval(PING_INTERVAL);
         m_pingTimer.setInterval(1500);
        // запустить таймер пинг
        m_pingTimer.start();
    }
    //
    // выбрать вкладку Графплан
    processTabChanged(0);
    //

    // установить режим отображения админ / оператор
    setSuperUserMode(account.isSuperUser());
    //
//    // обмен данными с модулями возможен только в СНК
//    if (appType == IssoAppType::SNK)
//    {
//        // если режим админа, запустить обмен данными без опроса модулей,
//        // иначе запустить обмен данными с опросом модулей
//        m_core.exchanger()->startExchanging(!account.isSuperUser());
//    }

    //
    // установить заголовок окна
    setWindowTitle(tr("Система наблюдения и контроля (%1)")
                                .arg(guardedObject->displayName()));
    // максимизировать окно
    setWindowState(Qt::WindowMaximized);
    //
//    // запустить таймер пинга (только для СНК)
//    if (m_appType == IssoAppType::SNK)
//    {
//        // установить интервал отправки пинга на сервер
//        m_pingTimer.setInterval(PING_INTERVAL);
//        // запустить таймер пинг
//        m_pingTimer.start();
//    }
    //
    // скрыть вкладку Датчики (устарела)
    ui->tabWidget->removeTab(5);

//    // скрыть вкладку Графики (временно)
//    ui->tabWidget->removeTab(6);

//    objectCfgTest();
//    foreach (IssoModuleConfig* moduleCfg, guardedObject->moduleConfigs())
//    {
//        // получить ссылку на конфиг цифрового шлейфа
//        IssoChainConfigLink link = moduleCfg->chainConfigLink(MULTISENSOR);
//        // получить конфиг шлейфа по имени
//        IssoChainConfig* chainCfg = guardedObject->chainConfig(link.chainCfgName());
//        // если конфиг шлейфа не найден, либо шлейф не цифровой
//        if (!chainCfg || (chainCfg->type() != DIGITAL))
//            return;
//        // привести к типу цифрового шлейфа
//        IssoDigitalChainConfig* digitalCfg = dynamic_cast<IssoDigitalChainConfig*>(chainCfg);
//        if (!digitalCfg)
//            return;
//        qDebug() << "Адреса мультидатчиков: " << digitalCfg->multiSensorIds().toHex(' ') << endl;
//    }

    //
    // создать отладочное окно
    initDebugInfo();
    //
    // создать виджет для тестирования
    initTestingWidget();




//    // инициализировать БД
//    setDataBase(db);
//    // запустить таймер пинга (только для СНК)
//    if (m_appType == IssoAppType::SNK)
//    {
//        // если режим админа, запустить обмен данными без опроса модулей,
//        // иначе запустить обмен данными с опросом модулей
//        m_core.exchanger()->startExchanging(!account.isSuperUser());
//        // установить интервал отправки пинга на сервер
//        m_pingTimer.setInterval(PING_INTERVAL);
//        // запустить таймер пинг
//        m_pingTimer.start();
//    }
//    //
//    // выбрать вкладку Графплан
//    processTabChanged(0);


//    if (guardedObject)
//     addDebugMsg(IssoObjectUtils::objectSchemeAsString(guardedObject));
}


MainWindow::~MainWindow()
{
    qDebug() << "~MainWindow" << endl;
    //
    //
    // отменить текущую фоновую задачу
    m_core.cancelBackgroundTask();
    // остановить таймер пинга
    m_pingTimer.stop();
    // подготовка к завершению работы
    uninit();
    delete ui;
}


IssoAppType MainWindow::getAppType() const
{
    return m_appType;
}


void MainWindow::setAppType(const IssoAppType &appType)
{
    m_appType = appType;
}


void MainWindow::updateEventTableView()
{
    ui->tabGraphPlan->selectEvents();
}


void MainWindow::updateModuleParams(const QString &moduleName,
                                    const IssoParamBuffer &paramBuffer)
{
    if (m_core.modulesController())
        m_core.modulesController()->updateModuleParams(moduleName, paramBuffer);
}


void MainWindow::setObjectOnline()
{
    if (guardedObject())
    {
        // перевести все модули объекта в online
        guardedObject()->setOnline();
        // обновить все визуальные списки
        ui->tabGraphPlan->updateAllLists();
        // обновить состояния датчиков / реле выбранного модуля
        ui->tabGraphPlan->updateSelectedModuleInfo();
    }
}

void MainWindow::setObjectOffline()
{
    if (guardedObject())
    {
        // перевести все модули объекта в offline
        guardedObject()->setOffline();
        // обновить все визуальные списки
        ui->tabGraphPlan->updateAllLists();
        // обновить состояния датчиков / реле выбранного модуля
        ui->tabGraphPlan->updateSelectedModuleInfo();
    }
}


void MainWindow::setModuleOnline(const QString &moduleName)
{
    if (guardedObject())
    {
        // перевести модуль в offline
        guardedObject()->setModuleOnline(moduleName);
        // обновить все визуальные списки
        ui->tabGraphPlan->updateAllLists();
        // если модуль выбран, обновить состояния его датчиков / реле
        if (guardedObject()->selectedModule()->displayName() == moduleName)
            ui->tabGraphPlan->updateSelectedModuleInfo();
    }
}


void MainWindow::setModuleOffline(const QString &moduleName)
{
    if (guardedObject())
    {
        // перевести модуль в offline
        guardedObject()->setModuleOffline(moduleName);
        // обновить все визуальные списки
        ui->tabGraphPlan->updateAllLists();
        // если модуль выбран, обновить состояния его датчиков / реле
        if (guardedObject()->selectedModule()->displayName() == moduleName)
            ui->tabGraphPlan->updateSelectedModuleInfo();
    }
}


void MainWindow::setEventFilter(const QDate &date, const QTime &timeFrom,
                                const QTime &timeTo, const QString &eventType)
{
    ui->tabGraphPlan->setEventFilter(date, timeFrom, timeTo, eventType);
}


void MainWindow::setEventFilterEnabled(bool enabled)
{
    ui->tabGraphPlan->setEventFilterEnabled(enabled);
}


void MainWindow::init()
{
    // подготовить к работе ядро приложения
    initEventHandlers();
    //
    // DEBUG
    setTestEventHandlers();
}


void MainWindow::uninit()
{
    uninitEventHandlers();
    ui->tabGraphPlan->uninit();
}


void MainWindow::initEventHandlers()
{
    connect(ui->tabWidget, SIGNAL(currentChanged(int)),
            this,          SLOT(processTabChanged(int)));
    // обработчики сигналов менеджера обмена
    connect(&m_core, SIGNAL(requestSent(IssoModuleRequest)),
            this,    SLOT(processRequestSent(IssoModuleRequest)));
    connect(&m_core, SIGNAL(replyReceived(IssoModuleReply,bool)),
            this,    SLOT(processReplyReceived(IssoModuleReply,bool)));
    connect(&m_core, SIGNAL(replyTimedOut(IssoModuleRequest)),
            this,    SLOT(processReplyTimedOut(IssoModuleRequest)));
    connect(&m_core, SIGNAL(replyDataReceived(QHostAddress,quint16,QByteArray)),
            this,    SLOT(processReplyDataReceived(QHostAddress,quint16,QByteArray)));
    connect(&m_core, SIGNAL(alarmDataReceived(QHostAddress,quint16,QByteArray)),
            this,    SLOT(processAlarmDataReceived(QHostAddress,quint16,QByteArray)));
    //
    // обработчики сигналов удленного регистратора событий
    //
    connect(&m_core, SIGNAL(remoteLogCompleted(QString)),
            this,    SLOT(processRemoteLogCompleted(QString)));
    connect(&m_core, SIGNAL(remoteLogFailed(QString)),
            this,    SLOT(processRemoteLogFailed(QString)));
    //
    // обработчики сигналов видеостримера
    //
    connect(&m_core, SIGNAL(openVideoStreamFailed(QHostAddress)),
            this,    SLOT(processOpenVideoStreamFailed(QHostAddress)));
    connect(&m_core, SIGNAL(readVideoStreamFailed(QHostAddress)),
            this,    SLOT(processReadVideoStreamFailed(QHostAddress)));
    //
    // вкладка Графплан:
    //
    // 2 обработчика сигналов изменения состояний датчиков
    connect(&m_core,            SIGNAL(sensorStatesChanged(QString)),
            ui->tabGraphPlan,   SLOT(processSensorStatesChanged(QString)));
    connect(&m_core,
            SIGNAL(multiSensorRawDataReceived(QString,quint8, IssoState,QMap<IssoDigitalSensorId,IssoState>, QMap<IssoDigitalSensorId,quint16>)),
            SLOT(processMultiSensorRawDataReceived(QString,quint8,IssoState, QMap<IssoDigitalSensorId,IssoState>, QMap<IssoDigitalSensorId,quint16>)));
    // обработчик сигналов изменения состояний реле
    connect(&m_core,            SIGNAL(relayStateChanged(QString,IssoParamId,bool)),
            ui->tabGraphPlan,   SLOT(processRelayStateChanged(QString,IssoParamId,bool)));
    // обработчик запроса записи в БД комментария к событию
    connect(ui->tabGraphPlan,   SIGNAL(writeEventCommentRequested(int,QString)),
            this,               SLOT(writeComment(int,QString)));
    // обработчик запроса информации о состоянии модуля
    connect(ui->tabGraphPlan,   SIGNAL(moduleInfoRequested(int)),
            &m_core,            SLOT(requestModuleInfo(int)));
    // обработчик запроса переключения реле модуля
    connect(ui->tabGraphPlan,   SIGNAL(turnModuleRelayRequested(int,IssoParamId,bool)),
            &m_core,            SLOT(turnModuleRelayAsync(int,IssoParamId,bool)));
    // обработчик широковещательный запрос переключения реле
    connect(ui->tabGraphPlan,   SIGNAL(turnBroadcastRelayRequested(IssoParamId,bool)),
            &m_core,            SLOT(turnBroadcastRelayAsync(IssoParamId,bool)));
    // обработчик запроса воспроизведения записи голоса
    connect(ui->tabGraphPlan,   SIGNAL(playVoiceRecordRequested(QHostAddress,quint16,QByteArray)),
            &m_core,            SLOT(playVoiceRecord(QHostAddress,quint16,QByteArray)));
    // обработчик запроса воспроизведения аудиозаписи из памяти модуля
    connect(ui->tabGraphPlan,   SIGNAL(playModuleRecordRequested(QHostAddress,quint16,quint8)),
            &m_core,            SLOT(playModuleAudioRecord(QHostAddress,quint16,quint8)));
    // обработчик запроса воспроизведения аудиозаписи из памяти ПК
    connect(ui->tabGraphPlan,   SIGNAL(playPcRecordRequested(QHostAddress,quint16,QString)),
            &m_core,            SLOT(playPcAudioRecord(QHostAddress,quint16,QString)));
    // обработчик запроса выборки событий из БД
    connect(ui->tabGraphPlan,   SIGNAL(selectEventsRequested(QDate,QTime,QTime,QString,int,bool)),
            this,               SLOT(processSelectEventsRequested(QDate,QTime,QTime,QString,int,bool)));
    // обработчика запроса запуска / остановки сценария
    connect(ui->tabGraphPlan,   SIGNAL(changeScenarioStateRequested(IssoScenarioInfo,bool)),
            &m_core,            SLOT(setScenarioActive(IssoScenarioInfo,bool)));
    // обработчик извещения о запуске / остановки сценария
    connect(&m_core,            SIGNAL(scenarioStateChanged(IssoScenarioInfo,bool)),
            ui->tabGraphPlan,   SLOT(processScenarioStateChanged(IssoScenarioInfo,bool)));
    // событие выбора действия над камерой
    connect(ui->tabGraphPlan, SIGNAL(cameraTurned(int,bool)),
            this,             SLOT(processCameraTurned(int,bool)));
    // событие выбора действия над датчиком
    connect(ui->tabGraphPlan,
            SIGNAL(sensorActionPerformed(quint8,int,quint8,IssoChainType,IssoSensorAction)),
            &m_core,
            SLOT(handleSensorAction(quint8,int,quint8,IssoChainType,IssoSensorAction)));
    // событие запроса обновления конфига объекта на сервере
    connect(ui->tabGraphPlan, SIGNAL(objectConfigUpdateRequested()),
            this,             SLOT(sendObjectConfigToServer()));
    //
    // вкладка Модули:
    //
    // связать сигнал запроса чтения дескриптора со слотом отправки этого запроса
    connect(ui->tabModules, SIGNAL(readDescriptorRequested(int)),
            &m_core,        SLOT(readDescriptor(int)));
    // связать сигнал запроса записи дескриптора со слотом отправки этого запроса
    connect(ui->tabModules, SIGNAL(writeDescriptorRequested(int,IssoModuleDescriptor)),
            &m_core,        SLOT(writeDescriptor(int,IssoModuleDescriptor)));

//    // посылать запрос состояния модулю по сигналу изменения настроек модуля
//    connect(ui->tabModules, SIGNAL(moduleSettingsChanged(int)),
//            &m_core,        SLOT(requestModuleInfo(int)));
    connect(ui->tabModules, SIGNAL(moduleSettingsChanged(int)),
            &m_core,        SLOT(updateModuleSettings(int)));

    // посылать запрос на запись конфигурации в модуль по кнопке Обновить вкладки "Модули"
    connect(ui->tabModules, SIGNAL(updateModuleConfigRequested(int)),
            &m_core,        SLOT(updateModuleConfig(int)));
    // связать сигнал чтения дескриптора с соответствующим сигналом вкладки Модули
    connect(&m_core,        SIGNAL(descriptorRead(QString,IssoModuleDescriptor)),
            ui->tabModules, SIGNAL(descriptorRead(QString,IssoModuleDescriptor)));
    // связать сигнал записи дескриптора с соответствующим сигналом вкладки Модули
    connect(&m_core,        SIGNAL(descriptorWritten(QString,IssoModuleDescriptor)),
            ui->tabModules, SIGNAL(descriptorWritten(QString,IssoModuleDescriptor)));
    // связать сигнал ошибки чтения / записи дескриптора с соответствующим сигналом вкладки Модули
    connect(&m_core,        SIGNAL(descriptorFailed(QString,IssoCmdId)),
            ui->tabModules, SIGNAL(descriptorFailed(QString,IssoCmdId)));
    // связи для активации мультидатчиков
    connect(ui->tabModules, SIGNAL(multiSensorActivationRequested(int,QList<quint8>)),
            &m_core,        SIGNAL(multiSensorActivationRequested(int,QList<quint8>)));
    connect(&m_core,        SIGNAL(multiSensorActivated(QHostAddress,quint8)),
            ui->tabModules, SLOT(processMultiSensorActivated(QHostAddress,quint8)));
    // связи для записи регистров мультидатчика
    connect(ui->tabModules, SIGNAL(multiSensorRegActionRequested(IssoMsRegAction,int,quint8)),
            &m_core,        SLOT(handleMultiSensorRegAction(IssoMsRegAction,int,quint8)));
    connect(&m_core,        SIGNAL(multiSensorRegActionCompleted(IssoMsRegAction,QHostAddress,quint8,bool)),
            ui->tabModules, SLOT(processMultiSensorRegActionCompleted(IssoMsRegAction,QHostAddress,quint8,bool)));

    //
//    connect(ui->tabChart,   SIGNAL(multiSensorRawInfoRequested(int,quint8)),
//            &m_core,        SLOT(requestMultiSensorRawInfo(int,quint8)));
    connect(ui->tabChart,   SIGNAL(multiSensorRegActionRequested(IssoMsRegAction,int,quint8)),
            &m_core,        SLOT(handleMultiSensorRegAction(IssoMsRegAction,int,quint8)));


    //
    if (m_appType == IssoAppType::SNK)
    {
        // регистрировать события только в СНК
        connect(&m_core,    SIGNAL(sensorStatesChanged(QString)),
                this,       SLOT(processSensorStatesChanged(QString)));
        connect(&m_core,    SIGNAL(moduleStateChanged(QString,IssoModuleState)),
                this,       SLOT(processModuleStateChanged(QString,IssoModuleState)));
        // отправлять события на серевер только в СНК
        connect(&m_pingTimer, SIGNAL(timeout()),
                this,         SLOT(sendPingToServer()));
        // записать фотоснимок в БД
        connect(&m_core, SIGNAL(cameraSnapshotReady(QDateTime,QString,int,int,int,QImage)),
                this,    SLOT(processCameraSnapshotReady(QDateTime,QString,int,int,int,QImage)));
    }

    connect(this,    SIGNAL(windowClosed()),
            &m_core, SLOT(stopVideo()));
}


void MainWindow::uninitEventHandlers()
{
    m_core.disconnect();
    ui->tabWidget->disconnect();
    ui->tabGraphPlan->disconnect();
    ui->tabModules->disconnect();
    ui->tabChart->disconnect();
}


void MainWindow::setDataBase(IssoDataBase* db)
{
    m_dataBase = db;
    // обновить данные модели событий
    m_dataBase->update(guardedObject());
    // задать модель данных
    ui->tabGraphPlan->setDataBaseModel(m_dataBase->eventsProxyModel());
}



void MainWindow::setGuardedObject(IssoObject *guardedObject)
{
//    // задать охраняемый объект
//    m_core.setGuardedObject(guardedObject);

    // инициализировать все вкладки
    ui->tabGraphPlan->setObject(guardedObject);
    ui->tabModules->setObject(guardedObject);
    ui->tabSensors->setObject(guardedObject);
    ui->tabCameras->setObject(guardedObject);
    ui->tabModuleConfigs->setObject(guardedObject);
    ui->tabChainConfigs->setObject(guardedObject);
    ui->tabScenarios->setObject(guardedObject);
    ui->tabChart->setObject(guardedObject);
}



void MainWindow::setSuperUserMode(bool superUser)
{
    // выбрать вкладку Графплан
    ui->tabWidget->setCurrentWidget(ui->tabGraphPlan);
    // если не админ, скрыть вкладки настроек
    if (!superUser)
    {
        while(ui->tabWidget->count() > 1)
            ui->tabWidget->removeTab(ui->tabWidget->count() - 1);
    }
    // установить режим отображения админ / оператор
    ui->tabGraphPlan->setSuperUserMode(superUser);
}


void MainWindow::registerMetaTypes()
{
    qRegisterMetaType<IssoModule>("IssoModule");
    qRegisterMetaType<IssoModuleRequest>("IssoModuleRequest");
    qRegisterMetaType<IssoModuleReply>("IssoModuleReply");
    qRegisterMetaType<QHostAddress>("QHostAddress");
    qRegisterMetaType<IssoModuleDescriptor>("IssoModuleDescriptor");
    qRegisterMetaType<IssoScenarioInfo>("IssoScenarioInfo");
    qRegisterMetaType<IssoModuleState>("IssoModuleState");
    qRegisterMetaType<IssoMsRegAction>("IssoMsWriteAction");
    qRegisterMetaType<IssoParamId>("IssoParamId");
}


//void MainWindow::displayCamera(int cameraId)
//{
//    // найти камеру по ID
//    IssoCamera* camera = guardedObject()->findCameraById(cameraId);
//    if (camera)
//    {
//        // отобразить видеопоток
//        m_core.startVideo(camera->ip(),
//                          camera->port(),
//                          camera->password());
//    }
//}


//void MainWindow::hideCamera()
//{
//    m_core.stopVideo();
//}


void MainWindow::processOpenVideoStreamFailed(const QHostAddress &ip)
{
    QString msg = tr("Невозможно получить видеопоток c IP-камеры (%1)").arg(ip.toString());
    QMessageBox::warning(this, tr("Внимание"), msg.toUtf8().data());
}


void MainWindow::processReadVideoStreamFailed(const QHostAddress &ip)
{
    QString msg = tr("Прерван видеопоток c IP-камеры (%1)").arg(ip.toString());
    QMessageBox::critical(this, tr("Ошибка"), msg.toUtf8().data());
}


QList<IssoEventData> MainWindow::getModuleDBEvents(IssoModule *module,
                                                     const QDateTime &dateTime)
{
    // ВРЕМЕННАЯ ПРОВЕРКА
    // получить конфиг адресного шлейфа модуля
    auto msChainConfig =
            guardedObject()->moduleAddressChainConfig(module->moduleConfigName());
    // получить список мультдачиков из конфига адресного шлейфа
    auto msIds = msChainConfig ? msChainConfig->multiSensorIdList()
                               : QList<quint8>();
    //
    //
    QList<IssoEventData> dbEvents;
    // обработать все параметры, изменившие состояния
    foreach (QSharedPointer<IssoParam> p, module->changedParams())
    {
        // получить параметр состояния датчика
        IssoSensorStateParam* stateParam =
                dynamic_cast<IssoSensorStateParam*>(p.data());
        if (!stateParam)
            continue;

        // найти имя типа датчика / шлейфа
        QString sensorName;
//        QString sensorName = IssoCommonData::stringByParamId(stateParam->id());
        // строковое описание состояния
        QString stateName;
        //
        // расширенное значние параметра
        quint32 extValue = 0;
        //
        // если параметр описывает состояние адресного шлейфа,
        // обработать каждый мультидатчик
        if (stateParam->id() == MULTISENSOR)
        {
            IssoMultiSensorStateParam* multiParam =
                    dynamic_cast<IssoMultiSensorStateParam*>(stateParam);
            if (!multiParam)
                continue;
            // обойти каждый мультидатчик
            foreach (IssoMultiSensorData data, multiParam->dataMap())
            {
                // ВРЕМЕННАЯ ПРОВЕРКА
                // обрабатывать только данные мультидатчиков из конфига адрерсного шлейфа
                if (!msIds.contains(data.multiSensorId()))
                    continue;
                //
                //
                // имя мультидатчика
                sensorName = IssoCommonData::makeMultiSensorName(data.multiSensorId());
                // имя состояния
                stateName = IssoCommonData::stringBySensorState(data.state());
                // расположение мультидатчика
                QString location = findMsLocation(module->moduleConfigName(),
                                                  data.multiSensorId());
                // расширенное значение
                extValue = data.detailsValue();
                // сформировать параметр и добавить в коллекцию
                dbEvents << IssoEventData(dateTime,
                                            module->displayName(),
                                            sensorName,
                                            stateName,
                                            extValue,
                                            guardedObject()->id(),
                                            location);
            }
        }
        // иначе обработать параметр
        else
        {
            // имя датчика
            sensorName = IssoCommonData::stringByParamId(stateParam->id());
            // имя состояния
            stateName = IssoCommonData::stringBySensorState(stateParam->state());
            //
            // если параметр имеет расширенное значение, получить его
            IssoSensorExtStateParam* extStateParam =
                    dynamic_cast<IssoSensorExtStateParam*>(stateParam);
            if (extStateParam)
            {
                // расширенное значение
                extValue = (quint32)(qRound(extStateParam->value() * 10));
            }
            // сформировать параметр и добавить в коллекцию
            dbEvents << IssoEventData(dateTime,
                                        module->displayName(),
                                        sensorName,
                                        stateName,
                                        extValue,
                                        guardedObject()->id());
        }
    }
    return dbEvents;
}


void MainWindow::logChangedSensorStates(const QDateTime &dateTime, IssoModule* module)
{
    if (!module)
        return;

//    QTime time;
//    time.start();
//    //
//    // начать транзакцию
//    m_dataBase->beginTransaction();

    // если модуль не активен
    if (module->moduleState() == INACTIVE)
    {
        // зарегистрировать на удаленном сервере
        sendEventToServer(dateTime, module);
//        // отправить событие на удаленный сервер
//        sendEventToServer(dateTime, module->displayName(), "", "", 0, "");

        // добавить событие в БД
        writeEventToDB(dateTime, module);
//        m_dataBase->insertEvent(dateTime, module->displayName(), "", "", true, objectId(), 0);
    }
    // если модуль активен
    else
    {
        // получить список событий модуля
        auto dbEvents = getModuleDBEvents(module, dateTime);
        // зарегистрировать на удаленном сервере
        sendEventToServer(dateTime, module, dbEvents);
        // добавить событие в БД
        writeEventToDB(dateTime, module, dbEvents);
    }
        //

//        // зарегистрировать в БД
//        foreach (auto event, dbEvents)
//        {
//            // состояние мультидатчика
//            IssoState state = IssoCommonData::sensorStateByString(event.sensorStateName);
//            // статус события
//            bool alarmed = (state == STATE_ALARM) ||
//                           (state == STATE_WARNING) ||
//                           (state == STATE_SHORT_CIRCUIT) ||
//                           (state == STATE_BREAK) ||
//                           (state == STATE_ERROR) ||
//                           (state == STATE_FAIL) ||
//                           (state == VOLTAGE_LOW);
////            // отправить событие на удаленный сервер
////            sendEventToServer(event.dateTime, event.moduleName, event.sensorName,
////                              event.sensorStateName, event.extValue, event.location);
//            // добавить событие в БД
//            m_dataBase->insertEvent(event.dateTime, event.moduleName,
//                                    event.sensorName, event.sensorStateName,
//                                    alarmed, event.objectId, event.extValue);
//        }
//    }
//    // завершить транзакцию
//    m_dataBase->endTransaction();
//    // обновить выборку событий
//    updateEventTableView();
    //
//    qDebug() << "Время вставки в БД: " << time.elapsed() << endl;

//    // DEBUG
//    if (guardedObject())
//    {
//        int alarmModules, alarmMs, alarmChannels;
//        guardedObject()->calcAlarmSummary(alarmModules, alarmMs, alarmChannels);

//        qDebug().noquote() << QString("Модули: %1 / %2 \nДатчики: %3 / %4 \nКаналы: %5 / %6")
//                    .arg(guardedObject()->totalModules()).arg(alarmModules)
//                    .arg(guardedObject()->totalMs()).arg(alarmMs)
//                    .arg(guardedObject()->totalChannels()).arg(alarmChannels) << endl;
//    }
}


void MainWindow::sendEventToServer(const QDateTime &dateTime, IssoModule *module,
                                   const QList<IssoEventData>& events)
{
//    // найти модуль
//    IssoModule* module = m_core.findModuleByDisplayName(moduleName);

    // проверить объект и модуль
    if (!guardedObject() || !module)
        return;
    // остановить таймер пинга
    m_pingTimer.stop();
    //
    // время события
    QString dateTimeStr = dateTime.toString("dd.MM.yyyy HH:mm:ss");
    // создать конструктор сообщений
    IssoLogMessageBuilder builder(guardedObject(), EVENT_SENSORS, dateTimeStr);
    // добавить данные модуля
    builder.addModuleData(module);
    //
    // добавить все события в сообщение
    foreach (auto event, events)
    {
        // если имя датчика и имя состояния не пусто, добавить
        if (!event.sensorName.isEmpty() && !event.sensorStateName.isEmpty())
        {
            // добавить данные датчиков
            builder.addSensorData(event.sensorName, event.sensorStateName,
                                  event.extValue, event.location);
        }
    }
    // сформировать сообщение
    QString msg = builder.getMessage();
    // отправить сообщение на удаленный сервер
    m_core.sendMessageToRemoteLog(msg);
    // запустить таймер пинга
    m_pingTimer.start();
    //
    // DEBUG
    addDebugMsg(msg);
}


void MainWindow::sendPhotoToServer(const QDateTime &dateTime,
                                   const QString& moduleName,
                                   const QString& sensorName,
                                   int cameraId, int cameraIp,
                                   const QString& base64Data)
{
    // проверить объект
    if (!guardedObject())
        return;
    // найти модуль
    auto module = m_core.findModuleByDisplayName(moduleName);
    if (!module)
        return;
    // остановить таймер пинга
    m_pingTimer.stop();
    //
    // время события
    QString dateTimeStr = dateTime.toString("dd.MM.yyyy HH:mm:ss");
    // создать конструктор сообщений
    IssoLogMessageBuilder builder(guardedObject(), EVENT_PHOTO, dateTimeStr);
    // добавить данные модуля
    builder.addModuleData(module);
    // добавить данные фотоснимка
    builder.addPhotoData(sensorName, cameraId, cameraIp, base64Data);
    // сформировать сообщение
    QString msg = builder.getMessage();
    // отправить сообщение на удаленный сервер
    m_core.sendMessageToRemoteLog(msg);
    // запустить таймер пинга
    m_pingTimer.start();
    //
    // DEBUG
    addDebugMsg(msg);
}


void MainWindow::writeEventToDB(const QDateTime &dateTime, IssoModule *module,
                                const QList<IssoEventData> &events)
{
    // проверить объект и модуль
    if (!guardedObject() || !module)
        return;
//    QTime time;
//    time.start();
    //
    // начать транзакцию
    m_dataBase->beginTransaction();
    // если список событий пуст, значит, модуль неактивен
    if (events.isEmpty())
    {
        // добавить событие в БД
        m_dataBase->insertEvent(dateTime, module->displayName(), "", "", true, objectId(), 0);
    }
    else
    {
        // иначе зарегистрировать их в БД
        foreach (auto event, events)
        {
            // состояние мультидатчика
            IssoState state = IssoCommonData::sensorStateByString(event.sensorStateName);
            // статус события
            bool alarmed = (state == STATE_ALARM) ||
                           (state == STATE_WARNING) ||
                           (state == STATE_SHORT_CIRCUIT) ||
                           (state == STATE_BREAK) ||
                           (state == STATE_ERROR) ||
                           (state == STATE_FAIL) ||
                           (state == VOLTAGE_LOW);
            // добавить событие в БД
            m_dataBase->insertEvent(event.dateTime, event.moduleName,
                                    event.sensorName, event.sensorStateName,
                                    alarmed, event.objectId, event.extValue);
        }
    }
    // завершить транзакцию
    m_dataBase->endTransaction();
    // обновить выборку событий
    updateEventTableView();
//    qDebug() << "Время вставки в БД: " << time.elapsed() << endl;
}


//void MainWindow::logChangedSensorStates(const QDateTime &dateTime, IssoModule* module)
//{
//    if (!module)
//        return;

////    QTime time;
////    time.start();
//    //
//    // начать транзакцию
//    m_dataBase->beginTransaction();

//    // если модуль не активен
//    if (module->moduleState() == INACTIVE)
//    {
//        // отправить событие на удаленный сервер
//        sendEventToServer(dateTime, module->displayName(), "", "", 0, "");
//        // добавить событие в БД
//        m_dataBase->insertEvent(dateTime, module->displayName(), "", "", true, objectId(), 0);
//    }
//    // если модуль активен
//    else
//    {
//        auto dbEvents = getModuleDBEvents(module, dateTime);
//        foreach (auto event, dbEvents)
//        {
//            // состояние мультидатчика
//            IssoState state = IssoCommonData::sensorStateByString(event.sensorStateName);
//            // статус события
//            bool alarmed = (state == STATE_ALARM) ||
//                           (state == STATE_WARNING) ||
//                           (state == STATE_SHORT_CIRCUIT) ||
//                           (state == STATE_BREAK) ||
//                           (state == STATE_ERROR) ||
//                           (state == STATE_FAIL) ||
//                           (state == VOLTAGE_LOW);
//            // отправить событие на удаленный сервер
//            sendEventToServer(event.dateTime, event.moduleName, event.sensorName,
//                              event.sensorStateName, event.extValue, event.location);
//            // добавить событие в БД
//            m_dataBase->insertEvent(event.dateTime, event.moduleName,
//                                    event.sensorName, event.sensorStateName,
//                                    alarmed, event.objectId, event.extValue);
//        }
//    }
//    // завершить транзакцию
//    m_dataBase->endTransaction();
//    // обновить выборку событий
//    updateEventTableView();
//    //
////    qDebug() << "Время вставки в БД: " << time.elapsed() << endl;

////    // DEBUG
////    if (guardedObject())
////    {
////        int alarmModules, alarmMs, alarmChannels;
////        guardedObject()->calcAlarmSummary(alarmModules, alarmMs, alarmChannels);

////        qDebug().noquote() << QString("Модули: %1 / %2 \nДатчики: %3 / %4 \nКаналы: %5 / %6")
////                    .arg(guardedObject()->totalModules()).arg(alarmModules)
////                    .arg(guardedObject()->totalMs()).arg(alarmMs)
////                    .arg(guardedObject()->totalChannels()).arg(alarmChannels) << endl;
////    }
//}


//void MainWindow::sendEventToServer(const QDateTime& dateTime,
//                                   const QString &moduleName,
//                                   const QString &sensorName,
//                                   const QString &sensorState,
//                                   quint32 extValue,
//                                   const QString& location)
//{
//    // найти модуль
//    IssoModule* module = m_core.findModuleByDisplayName(moduleName);
//    // проверить объект и модуль
//    if (!guardedObject() || !module)
//        return;
//    // остановить таймер пинга
//    m_pingTimer.stop();
//    //
//    // время события
//    QString dateTimeStr = dateTime.toString("dd.MM.yyyy HH:mm:ss");
//    // создать конструктор сообщений
//    IssoLogMessageBuilder builder(guardedObject(), dateTimeStr);
//    // добавить данные модуля
//    builder.addModuleData(module);
//    // если имя датчика или имя состояния не пусто, добавить
//    if (!sensorName.isEmpty() && !sensorState.isEmpty())
//    {
//        // добавить данные датчиков
//        builder.addSensorData(sensorName, sensorState,
//                              extValue, location);
//    }
//    // сформировать сообщение
//    QString msg = builder.getMessage();
//    // отправить сообщение на удаленный сервер
//    m_core.sendMessageToRemoteLog(msg);
//    // запустить таймер пинга
//    m_pingTimer.start();
//}


void MainWindow::sendPingToServer()
{
    if (m_appType != IssoAppType::SNK)
        return;
    //
    // остановить таймер пинга
    m_pingTimer.stop();
    // время события
    QString dateTimeStr =
            QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm:ss");
    // создать конструктор сообщений
    IssoLogMessageBuilder builder(guardedObject(), EVENT_PING, dateTimeStr);
    // сформировать сообщение
    QString msg = builder.getMessage();
    // отправить сообщение на удаленный сервер
    m_core.sendMessageToRemoteLog(msg);
    // запустить таймер пинга
    m_pingTimer.start();
}


void MainWindow::processCameraSnapshotReady(const QDateTime &dateTime,
                                            const QString &moduleName, int multiSensorId,
                                            int cameraId, int cameraIp,
                                            const QImage &frame)
{
    if (m_dataBase)
    {
        // сформировать имя датчика
        auto sensorName = IssoCommonData::makeMultiSensorName(multiSensorId);

//        // записать изображение в массив
//        QByteArray frameData = IssoCommonData::convertImageToByteArray(frame, "JPG");
        // преобразовать изображение в base64
        QString frameData = IssoCommonData::convertImageToBase64(frame, "JPG");

        // сохранить фотоснимок в БД
        if (m_dataBase->insertPhoto(dateTime, moduleName, sensorName,
                                    cameraId, cameraIp, frameData))
        {
            // обновить выборку событий
            updateEventTableView();
        }
//        // отправить фотоснимок на сервер
//        m_core.sendPhotoToServer(dateTime.toTime_t(),
//                                 IssoCommonData::convertImageToBase64(frame, "JPG"));

        //
        // отправить фото на сервер
        sendPhotoToServer(dateTime, moduleName, sensorName, cameraId, cameraIp,
                          frameData);
//                          QString::fromUtf8(frameData.toBase64().data()));
    }
}



void MainWindow::runScenarios(IssoModule *module)
{
    if (!module)
        return;
    // обрабатывать только Тревогу и Неисправность
    if ((module->moduleState() != ALARM) && (module->moduleState() != FAIL))
        return;
    // получить конфиг модуля
    IssoModuleConfig* cfg = guardedObject()->moduleConfig(module->moduleConfigName());
    if (!cfg)
        return;
    // получить имя сценария по состоянию
    QString scenarioName = cfg->scenarioNameByModuleState(module->moduleState());
    // если нет связанных сценариев, выход
    if (scenarioName.isEmpty())
        return;
    // запустить сценарий
    m_core.setScenarioActive(IssoScenarioInfo(scenarioName,
                                              module->buildNum(),
                                              module->floorNum()),
                             true);
}


void MainWindow::handleChangedMsParam(const QDateTime& dateTime, IssoModule *module,
                                      QSharedPointer<IssoParam> param)
{
    if (!guardedObject())
        return;
    // получить параметр состояния адресного шлейфа
    auto msChainParam = dynamic_cast<IssoMultiSensorStateParam*>(param.data());
    if (!msChainParam)
        return;
    // статус события
    bool alarmed = (msChainParam->state() == STATE_ALARM) ||
                   (msChainParam->state() == STATE_WARNING);
    if (!alarmed)
        return;

    // получить конфиг адресного шлейфа модуля
    auto msChainConfig =
            guardedObject()->moduleAddressChainConfig(module->moduleConfigName());

    if (!msChainConfig)
        return;
    //
    // ВРЕМЕННАЯ ПРОВЕРКА
    // получить список мультдачиков из конфига адресного шлейфа
    auto msIds = msChainConfig->multiSensorIdList();
    //
    //
    // найти тревожный мультидатчик, связанный с камерой
    foreach (auto msData, msChainParam->dataMap())
    {
        // ВРЕМЕННАЯ ПРОВЕРКА
        // обрабатывать только данные мультидатчиков из конфига адрерсного шлейфа
        if (!msIds.contains(msData.multiSensorId()))
            continue;
        //
        //
        // состояние мультидатчика
        bool stateAlarmed = (msData.state() == STATE_ALARM) ||
                            (msData.state() == STATE_WARNING);
        if (!stateAlarmed)
            continue;
        // получить конфиг мультидатчика по ID
        auto msConfig = msChainConfig->multiSensorConfig(msData.multiSensorId());
        // если к мультидатчику привязана камера
        if (msConfig /*&& (msConfig->cameraId() != -1)*/)
        {
            // если видео с камеры уже воспроизводится
            if (m_core.isDisplayingVideo())
            {
                // если отображаемая камера не связана с текущим мультидатчиком
                if (m_core.displayingCameraId() != msConfig->cameraId())
                    continue;
            }
            // если видео не запущено
            else
            {
                // если мультидатчик не связан с камерой, ищем далее
                if (msConfig->cameraId() == -1)
                    continue;
            }


            qDebug() << QString("Включение камеры-%1 (модуль %2, мультидатчик %3)")
                                .arg(msConfig->cameraId())
                                .arg(module->displayName())
                                .arg(msData.multiSensorId()) << endl;

//            // тест
//            QFile file("base64Data.txt");
//            if (file.open(QIODevice::ReadWrite))
//            {
//                QByteArray ba = file.readAll();
//                m_core.sendPhotoToServer(dateTime.toTime_t(), ba);
//            }

            // если не удалось отобразить видео, найти следующую камеру
            if (!m_core.displayCameraByAlarm(dateTime,
                                             module->displayName(),
                                             msData.multiSensorId(),
                                             msConfig->cameraId()))
                continue;
            // завершить обработку
            return;
        }
    }
}


QString MainWindow::findMsLocation(const QString &moduleCfgName, int msId)
{
    if (!guardedObject())
        return "";
    // получить конфиг мультидатчика
    auto msCfg = guardedObject()->findMultiSensorCfgByModuleCfgName(moduleCfgName, msId);
    // вернуть описание расположения мультидатчика
    return (msCfg ? msCfg->locationDesc() : "");
}


//void MainWindow::runScenarios(IssoModuleState newState)
//{
//    // обрабатывать только Тревогу и Неисправность
//    if ((newState != ALARM) && (newState != FAIL))
//        return;
//    //
//    foreach (auto module, guardedObject()->locatedModules())
//    {
//        // если модуль неактивен
//        if (module->moduleState() == INACTIVE)
//            continue;
//        // получить конфиг модуля
//        IssoModuleConfig* cfg = guardedObject()->moduleConfig(module->getModuleConfigName());
//        if (!cfg)
//            continue;
//        // получить имя сценария по состоянию
//        QString scenarioName = cfg->scenarioNameByModuleState(newState);
//        // если нет связанных сценариев, выход
//        if (scenarioName.isEmpty())
//            continue;
//        // запустить сценарий
//        m_core.setScenarioActive(IssoScenarioInfo(scenarioName,
//                                                  module->buildNum(),
//                                                  module->floorNum()),
//                                 true);
//    }
//}


void MainWindow::initDebugInfo()
{
    m_debugInfo = new QTextEdit();
    m_debugInfo->setWindowTitle(tr("Отладочная информация"));
    m_debugInfo->setGeometry(0, 0, 500, 200);
    m_debugInfo->setWindowFlag(Qt::WindowStaysOnTopHint, true);
    //
    connect(this,        SIGNAL(windowClosed()),
            m_debugInfo, SLOT(deleteLater()));
}


void MainWindow::addDebugMsg(const QString &msg)
{
    if (!m_debugInfo)
        return;
    // если лог слишком большой, очистить перед добавлением
    if (m_debugInfo->toPlainText().length() > 1000000)
        m_debugInfo->clear();
    // добавить сообщение
    m_debugInfo->append(msg);
}


void MainWindow::initTestingWidget()
{
    m_testWidget = new TestingWidget(&m_core
                                    /*m_core.guardedObject(),
                                     m_core.exchanger(),
                                     m_core.modulesController()*/);
    connect(this,         SIGNAL(windowClosed()),
            m_testWidget, SLOT(deleteLater()));
}


void MainWindow::writeComment(int eventId, const QString &comment)
{
    // обновить поле comment выбранной записи
    if (m_dataBase->updateEventComment(eventId, comment))
    {
        // если успешно, обновить выборку
        ui->tabGraphPlan->selectEvents();
    }
}


void MainWindow::processSelectEventsRequested(QDate date, QTime timeFrom,
                                              QTime timeTo, const QString eventType,
                                              int objectId, bool filtered)
{
    // если фильтрация активирована
    if (filtered)
    {
        // отфильтровать события
        m_dataBase->selectFilteredEvents(date, timeFrom, timeTo, eventType, objectId);
    }
    else
    {
        // вывести все события
        m_dataBase->selectAllEvents(objectId);
    }
}


int MainWindow::objectId()
{
    return m_core.guardedObject()->id();
}


IssoObject *MainWindow::guardedObject()
{
    return m_core.guardedObject();
}


void MainWindow::setTestEventHandlers()
{
    connect(ui->tabGraphPlan,           SIGNAL(testCtrlReplyReceived(IssoModuleReply,bool)),
            m_core.modulesController(), SLOT(processReplyReceived(IssoModuleReply,bool)));
    connect(ui->tabGraphPlan,           SIGNAL(testCtrlAlarmReceived(IssoModuleReply)),
            m_core.modulesController(), SLOT(processAlarmReceived(IssoModuleReply)));
    connect(ui->tabGraphPlan,           SIGNAL(testCtrlReplyTimedOut(IssoModuleRequest)),
            m_core.modulesController(), SLOT(processReplyTimedOut(IssoModuleRequest)));
    connect(ui->tabGraphPlan, &GraphicPlanTabWidget::testClearRemoteLogRequested,
            [this]() { m_core.clearRemoteLog(); });
}


void MainWindow::moduleCfgTest()
{
    //  Сохраняемая конфигурация модуля
    //
    QByteArray ba;
    QDataStream stream(&ba, QIODevice::ReadWrite);
    //
    IssoAnalogChainConfig* analogCfg1 = new IssoAnalogChainConfig(SMOKE_CHAIN, "Analog Chain - 1", true);
    analogCfg1->insertStateRange(STATE_NORMAL, IssoStateRange(2.0, 5.0));
    analogCfg1->insertStateRange(STATE_WARNING, IssoStateRange(5.0, 10.0));
    analogCfg1->insertStateRange(STATE_ALARM, IssoStateRange(10.0, 20.0));
    //
    IssoAnalogChainConfig* analogCfg2 = new IssoAnalogChainConfig(SMOKE_CHAIN, "Analog Chain - 2", true);
    analogCfg2->insertStateRange(STATE_NORMAL, IssoStateRange(1.0, 15.0));
    analogCfg2->insertStateRange(STATE_WARNING, IssoStateRange(15.0, 120.0));
    analogCfg2->insertStateRange(STATE_ALARM, IssoStateRange(120.0, 230.0));
    //
    IssoModuleConfig moduleCfg("Module Config");
    moduleCfg.insertSensorConfig(SENSOR_SMOKE, analogCfg1);
    moduleCfg.insertSensorConfig(SENSOR_CO, analogCfg2);
    stream << &moduleCfg;
    //
    // вывести имя
    //
    qDebug() << "==========" << endl
             << "Save: " << moduleCfg.name() << endl
             << "==========" << endl;
    // вывести встроенные датчики
    foreach (IssoParamId id, moduleCfg.sensorConfigs().keys())
    {
        // id и имя конифигурации встроенного датчика
        IssoAnalogChainConfig* sensorCfg = moduleCfg.sensorConfig(id);
        qDebug() << QString("ID = %1;   Sensor config name = %2")
                    .arg(id)
                    .arg(sensorCfg->name()) << endl;
        // вывести диапазоны значений состояний
        foreach (IssoState state, sensorCfg->stateRanges().keys())
        {
            // получить диапазон
            IssoStateRange range = sensorCfg->stateRange(state);
            qDebug() << QString("State = %1;   Range = [%2, %3]")
                        .arg(state)
                        .arg(range.min())
                        .arg(range.max()) << endl;
            qDebug() << "----------" << endl;
        }
    }
    //
    //  Загружаемая конфигурация модуля
    //
    stream.device()->reset();
    //
    IssoModuleConfig resultCfg;
    stream >> &resultCfg;
    //
    // вывести имя
    qDebug() << "==========" << endl
             << "Load: " << resultCfg.name() << endl
             << "==========" << endl;
    // вывести встроенные датчики
    foreach (IssoParamId id, resultCfg.sensorConfigs().keys())
    {
        // id и имя конифигурации встроенного датчика
        IssoAnalogChainConfig* sensorCfg = resultCfg.sensorConfig(id);
        qDebug() << QString("ID = %1;   Sensor config name = %2")
                    .arg(id)
                    .arg(sensorCfg->name()) << endl;
        // вывести диапазоны значений состояний
        foreach (IssoState state, sensorCfg->stateRanges().keys())
        {
            // получить диапазон
            IssoStateRange range = sensorCfg->stateRange(state);
            qDebug() << QString("State = %1;   Range = [%2, %3]")
                        .arg(state)
                        .arg(range.min())
                        .arg(range.max()) << endl;
            qDebug() << "----------" << endl;
        }
    }
}


void MainWindow::objectCfgTest()
{
    //
    //  Конфигурация модуля
    //
    QByteArray ba;
    QDataStream stream(&ba, QIODevice::ReadWrite);
    // Конфигурация аналогового шлейфа - 1
    IssoAnalogChainConfig* sensorCfg1 = new IssoAnalogChainConfig(SMOKE_CHAIN, "Smoke cfg", 6, true);
    sensorCfg1->insertStateRange(STATE_NORMAL, IssoStateRange(2.0, 5.0));
    sensorCfg1->insertStateRange(STATE_WARNING, IssoStateRange(5.0, 10.0));
    sensorCfg1->insertStateRange(STATE_ALARM, IssoStateRange(10.0, 20.0));
    // Конфигурация аналогового шлейфа - 2
    IssoAnalogChainConfig* sensorCfg2 = new IssoAnalogChainConfig(SMOKE_CHAIN, "CO cfg", 6, true);
    sensorCfg2->insertStateRange(STATE_NORMAL, IssoStateRange(1.0, 15.0));
    sensorCfg2->insertStateRange(STATE_WARNING, IssoStateRange(15.0, 120.0));
    sensorCfg2->insertStateRange(STATE_ALARM, IssoStateRange(120.0, 230.0));
    //
    IssoModuleConfig* moduleCfg = new IssoModuleConfig("Module Config - 1");
    moduleCfg->insertSensorConfig(SENSOR_SMOKE, sensorCfg1);
    moduleCfg->insertSensorConfig(SENSOR_CO, sensorCfg2);
    //
    // Конфигурация аналогового шлейфа
    //
    IssoAnalogChainConfig* analogChainCfg = new IssoAnalogChainConfig(SMOKE_CHAIN, "Analog Chain cfg", 6, true);
    analogChainCfg->insertStateRange(STATE_NORMAL, IssoStateRange(1.0, 3.0));
    analogChainCfg->insertStateRange(STATE_WARNING, IssoStateRange(3.0, 5.0));
    analogChainCfg->insertStateRange(STATE_ALARM, IssoStateRange(5.0, 10.0));
    //
    // Конфигурация цифрового шлейфа
    //
    IssoAddressChainConfig* digitalChainCfg = new IssoAddressChainConfig("Digital Chain cfg", true);
    qDebug() << "digitalChainCfg = " << digitalChainCfg->name();
        // мультидатчик - 1
        IssoMultiSensorConfig* multiCfg1 = new IssoMultiSensorConfig();

        // цифровой датчик - 1_1
        auto digitalCfg11 = multiCfg1->digitalSensorConfig(DIGITALSENSOR_SMOKE_O);
        digitalCfg11->setStateValue(STATE_NORMAL, 40);
        digitalCfg11->setStateValue(STATE_WARNING, 50);
        digitalCfg11->setStateValue(STATE_ALARM, 60);
        digitalCfg11->setCondition(CONDITION_AND);
        // цифровой датчик - 1_2
        auto digitalCfg12 = multiCfg1->digitalSensorConfig(DIGITALSENSOR_TEMP_A);
        digitalCfg12->setStateValue(STATE_NORMAL, 10);
        digitalCfg12->setStateValue(STATE_WARNING, 20);
        digitalCfg12->setStateValue(STATE_ALARM, 30);
        digitalCfg12->setCondition(CONDITION_OR);
//            // цифровой датчик - 1_1
//            IssoDigitalSensorConfig digitalCfg11(DIGITALSENSOR_SMOKE_O);
//            digitalCfg11.setStateValue(STATE_NORMAL, 40);
//            digitalCfg11.setStateValue(STATE_WARNING, 50);
//            digitalCfg11.setStateValue(STATE_ALARM, 60);
//            digitalCfg11.setCondition(CONDITION_AND);
//            // цифровой датчик - 1_2
//            IssoDigitalSensorConfig digitalCfg12(DIGITALSENSOR_TEMP_A);
//            digitalCfg12.setStateValue(STATE_NORMAL, 10);
//            digitalCfg12.setStateValue(STATE_WARNING, 20);
//            digitalCfg12.setStateValue(STATE_ALARM, 30);
//            digitalCfg12.setCondition(CONDITION_OR);
//        // добавить цифровые датчики в мультидатчик
//        multiCfg1->insertDigitalSensorConfig(digitalCfg11);
//        multiCfg1->insertDigitalSensorConfig(digitalCfg12);
    // добавить мультидатчики в цифровой шлейф
    digitalChainCfg->insertMultiSensorConfig(multiCfg1);
    //
    // Объект - сохраняемый
    IssoObject objSave;
    // добавить конфигурации модулей
    objSave.insertModuleConfig(moduleCfg);
    objSave.saveModuleConfigs(stream);
    // добавить конфигурации шлейфов
    objSave.insertChainConfig(analogChainCfg);
    objSave.insertChainConfig(digitalChainCfg);
    objSave.saveChainConfigs(stream);
    //
    stream.device()->reset();
    //
    // Объект - загружаемый
    IssoObject objLoad;
    // загрузить конфигурации модулей
    objLoad.loadModuleConfigs(stream);
    // загрузить конфигурации шлейфов
    objLoad.loadChainConfigs(stream);
    //
    //
    // вывести в строку
    QTextStream textStream(stdout);
    // вывести конфигурации модулей
    foreach (IssoModuleConfig* cfg, objLoad.moduleConfigs().values())
    {
        cfg->toString(textStream);
    }
    // вывести конфигурации шлейфов
    foreach (IssoChainConfig* cfg, objLoad.chainConfigs().values())
    {
        cfg->toString(textStream);
    }
    qDebug() << "Test finished" << endl;
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    // если СНК
    if (m_appType == IssoAppType::SNK)
    {
        // вывести диалог ввода пароля
        bool res = PasswordDialog::inputPassword(this, m_account.password(),
                                             tr("Введен неверный пароль! Приложение не будет завершено"));
        if (res)
        {
            emit windowClosed();
//            // закрыть все окна
//            qApp->closeAllWindows();
            event->accept();
        }
        else
            event->ignore();
    }
    // если ГСЦ / СОО
    else
    {
        emit windowClosed();
        event->accept();
    }
}


void MainWindow::keyPressEvent(QKeyEvent *event)
{
    switch(event->key())
    {
        // показать / скрыть виджет тестирования
        case Qt::Key_F1:
        {
            if ((event->modifiers() & Qt::ControlModifier) && m_testWidget)
                m_testWidget->setVisible(!m_testWidget->isVisible());
            break;
        }
        // показать / скрыть панель индикаторов
        case Qt::Key_F2:
        {
            if (event->modifiers() & Qt::ControlModifier)
                ui->tabGraphPlan->setIndicatiorsVisible(!ui->tabGraphPlan->indicatorsVisible());
            break;
        }
        // показать / скрыть отладочное окно
        case Qt::Key_F12:
        {
            if ((event->modifiers() & Qt::ControlModifier) && m_debugInfo)
                m_debugInfo->setVisible(!m_debugInfo->isVisible());
            break;
        }
        default:
            break;
    }

//    // показать / скрыть отладочное окно
//    if ((m_debugInfo) &&
//        (event->modifiers() & Qt::ControlModifier) &&
//        (event->key() == Qt::Key_F12))
//    {
//        m_debugInfo->setVisible(!m_debugInfo->isVisible());
//    }
}


void MainWindow::processRemoteLogCompleted(const QString &replyMsg)
{
    Q_UNUSED(replyMsg);
//    QMessageBox::information(this, tr("Ответ"), replyMsg);
}


void MainWindow::processRemoteLogFailed(const QString& errorMsg)
{
//    Q_UNUSED(errorMsg);
//    // ВРЕМЕННО ОТКЛЮЧИТЬ
//    QMessageBox::warning(this, tr("Внимание"),
//                         tr("Не удалось зарегистрировать событие на удаленном сервере"));
//    if (m_debugInfo)
//    {
//        QString msg = tr("Не удалось зарегистрировать событие на удаленном сервере");
//        m_debugInfo->append(msg);
//    }
    if (m_debugInfo)
    {
        m_debugInfo->append(errorMsg);
    }
}


void MainWindow::processRequestSent(const IssoModuleRequest &request)
{
        QString msg = tr("Запрос [%1:%2]: %3")
                            .arg(request.address().toString())
                            .arg(request.port())
                            .arg(QString(request.bytes().toHex(' ').toUpper()));
        addDebugMsg(msg);
        //
        //
        QString detailMsg = tr("Команда запроса:   %1\n").arg(IssoCommonData::stringByCmdId(request.command().id()));
        for (int i = 0; i < request.command().paramCount(); ++i)
        {
            detailMsg += QString("%1\n").arg(request.command().paramPtrByIndex(i).data()->description());
        }
        addDebugMsg(detailMsg);
}


void MainWindow::processReplyReceived(const IssoModuleReply &reply, bool ok)
{
    if (m_debugInfo)
    {
        QString detailMsg = tr("Команда ответа:   %1\n").arg(IssoCommonData::stringByCmdId(reply.command().id()));
        for (int i = 0; i < reply.command().paramCount(); ++i)
        {
            detailMsg += QString("%1\n").arg(reply.command().paramPtrByIndex(i).data()->description());
        }
        addDebugMsg(detailMsg);
    }
}


void MainWindow::processReplyTimedOut(const IssoModuleRequest &request)
{
    QString msg = tr("Таймаут ответа [%1:%2]: ответ не получен или некорректен\n")
                        .arg(request.address().toString())
                        .arg(request.port());
    addDebugMsg(msg);
}


void MainWindow::processReplyDataReceived(const QHostAddress &ip, quint16 port, const QByteArray &data)
{
    QHostAddress ipv4(ip.toIPv4Address());
    QString msg = tr("Ответ [%1:%2]: %3")
                        .arg(ipv4.toString())
                        .arg(port)
                        .arg(QString(data.toHex(' ').toUpper()));
    addDebugMsg(msg);
}


void MainWindow::processAlarmDataReceived(const QHostAddress &ip, quint16 port, const QByteArray &data)
{
    QHostAddress ipv4(ip.toIPv4Address());
    QString msg = tr("Тревога [%1:%2]: %3")
                        .arg(ipv4.toString())
                        .arg(port)
                        .arg(QString(data.toHex(' ').toUpper()));
    addDebugMsg(msg);
}


void MainWindow::processSensorStatesChanged(const QString &moduleName)
{
    // получить текущее время
    auto dateTime = QDateTime::currentDateTime();
    //
    if (m_appType != IssoAppType::SNK)
        return;
    //
    IssoModule* module = m_core.findModuleByDisplayName(moduleName);
    if (!module)
        return;
    //
    // зарегистрировать события
    logChangedSensorStates(dateTime/*QDateTime::currentDateTime()*/, module);
//    // если видео с камеры уже воспроизводится, то выход
//    if (m_core.isDisplayingVideo())
//        return;
    //
    // обойти все параметры мультидатчиков, изменившие состояния
    foreach (QSharedPointer<IssoParam> p, module->changedParams())
    {
        // если параметр представляет адресный шлейф, обработать
        if (p.data()->id() == MULTISENSOR)
            handleChangedMsParam(dateTime, module, p);
    }
}


void MainWindow::processModuleStateChanged(const QString &moduleName,
                                           IssoModuleState moduleState)
{
    if (m_appType != IssoAppType::SNK)
        return;
    //
    IssoModule* module = m_core.findModuleByDisplayName(moduleName);
    if (!module)
        return;
    // получить состояние объекта
    IssoModuleState objectState = m_core.objectState();
    // проанализировать состояние модуля
    switch(objectState)
    {
        // тревога
        case ALARM:
        {
            // воспроизвести звук
            m_soundPlayer.playSound(SOUND_ALARM);
            break;
        }
        // неисправность или оффлайн
        case FAIL:
        case INACTIVE:
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
    //
    // запустить сценарии, соответствующие состоянию модуля (если Тревоги или Неисправность)
    runScenarios(module);
}

void MainWindow::processMultiSensorRawDataReceived(const QString& moduleName,
                                                    quint8 multiSensorId, IssoState multiSensorState,
                                                    const QMap<IssoDigitalSensorId, IssoState> &sensorStates,
                                                    const QMap<IssoDigitalSensorId, quint16> &sensorValues)
{
    // если проводятся измерения показаний встроенных датчиков МД
    if (ui->tabChart->isMeasuring())
    {
        // передать данные во вкладку Графики
        ui->tabChart->handleSensorData(sensorValues);
    }
    // иначе вывести данные в отдельном окне
    else
    {
        if (!guardedObject())
            return;
        // найти модуль
        auto module = guardedObject()->findModuleByDisplayName(moduleName);
        if (!module)
            return;
        // получить описание расположения мультидатчика
        QString location = findMsLocation(module->moduleConfigName(), multiSensorId);
        //
        // сформировать html-строку для отображения данных мультидатчика
        QString htmlHint = HtmlUtils::makeMultiSensorExtHint(module->displayName(),
                                                             multiSensorId,
                                                             multiSensorState,
                                                             location,
                                                             sensorStates,
                                                             sensorValues);
        // вывести немодальное окно с данными мультидатчика
        QMessageBox* msgBox = new QMessageBox(this);
        msgBox->setWindowTitle(QString("Данные %1-%2")
                                        .arg(IssoCommonData::stringByParamId(MULTISENSOR))
                                        .arg(multiSensorId));
        msgBox->setText(htmlHint);
        msgBox->setAttribute(Qt::WA_DeleteOnClose);
        msgBox->show();
    }
}


void MainWindow::processCameraTurned(int cameraId, bool turnOn)
{
    // если запрос на включение камеры
    if (turnOn)
    {
        // если видео уже отображается
        if (m_core.isDisplayingVideo())
        {
            // вывести сообщение
            QMessageBox::warning(this, tr("Внимание"),
                                 tr("Видеопоток уже запущен.\n"
                                    "Нельзя отобразить видео с нескольких камер."));
        }
        else
        {
            // отобразить видеопоток
            m_core.displayCamera(cameraId);
        }
    }
    // если запрос на выключение камеры
    else
    {
        // остановить видеопоток
        m_core.stopVideo();
    }
}


void MainWindow::sendObjectConfigToServer()
{
    if (!guardedObject())
        return;
    // получить JSON-строку конфигурации объекта
    QString objCfg = IssoObjectUtils::objectSchemeAsString(guardedObject());
    // отправить на сервер
    m_core.sendObjectCfgToRemoteLog(objCfg);

    addDebugMsg(objCfg);
}


void MainWindow::processTabChanged(int index)
{
    switch(index)
    {
        // вкладка "Графплан"
        case 0:
        {
            ui->tabGraphPlan->updateTab();
            // обновить данные модели событий
            m_dataBase->update(guardedObject());
            break;
        }
        // вкладка "Модули"
        case 1:
        {
            ui->tabModules->updateTab();
            break;
        }
        // вкладка "Конфигурации модулей"
        case 2:
        {
            ui->tabModuleConfigs->updateTab();
            break;
        }
        // вкладка "Конфигурации шлейфов"
        case 3:
        {
            ui->tabChainConfigs->updateTab();
            break;
        }
        // вкладка "Камеры"
        case 4:
        {
            ui->tabCameras->updateTab();
            break;
        }
        // вкладка "Автоматика"
        case 5:
        {
            ui->tabScenarios->updateTab();
            break;
        }
        // вкладка "Графики"
        case 6:
        {
            ui->tabChart->updateTab();
            break;
        }
        default:
            break;
    }
}


