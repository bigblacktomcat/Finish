#include "issocore.h"
#include "passworddialog.h"
#include <QGraphicsView>
#include <QProgressDialog>
#include <QtEndian>


IssoCore::IssoCore(QObject *parent)
    : QObject(parent)
{
//    init();
}


IssoCore::~IssoCore()
{
    qDebug() << "~IssoCore";
    //
    uninit();
}


IssoObject *IssoCore::guardedObject() const
{
    return m_object;
}


void IssoCore::setGuardedObject(IssoObject *guardedObject)
{
    // уничтожить теущий объект
    destroyGuardedObject();
    // сохранить ссылку на новый объект
    m_object = guardedObject;
//    //
//    // задать объект контроллеру
//    if (m_controller)
//        m_controller->setObject(guardedObject);
//    // задать объект менеджеру обмена
//    if (m_exchanger)
//        m_exchanger->setObject(guardedObject);
//    // задать объект исполнителю сценариев
//    if (m_scenarioExecutor)
//        m_scenarioExecutor->setObject(guardedObject);
//    // обновить конфигурацию
//    updateConfig();
}


void IssoCore::destroyGuardedObject()
{
    if (m_object)
    {
        // уничтожить охраняемый объект
        delete m_object;
        // обнулить указатель
        m_object = nullptr;
    }
}


QList<IssoModule *> IssoCore::modules() const
{
    return m_object->modules();
}


void IssoCore::addModule(IssoModule *module)
{
    m_object->addModule(module);
    //
//    // обновить конфигурацию
//    updateConfig();
}


QString IssoCore::addModule(int displayId, const QHostAddress &ip, quint16 port, int uniqueId)
{
    return m_object->addModule(displayId, ip, port, uniqueId);
}


void IssoCore::addModules(const QList<IssoModule *> &modules)
{
    m_object->addModules(modules);
}


void IssoCore::removeModule(IssoModule *module)
{
    m_object->removeModule(module);
//    // обновить конфигурацию
//    updateConfig();
}


bool IssoCore::removeModule(const QString &moduleName)
{
    return m_object->removeModule(moduleName);
}


bool IssoCore::addFloorToBuilding(const QString &buildName,
                                  int floorNum,
                                  const QPixmap &background)
{
    bool result = m_object->addFloorToBuilding(buildName, floorNum, background);
//    // обновить конфигурацию
//    this->updateConfig();
    return result;
}


bool IssoCore::removeFloorFromBuilding(const QString &buildName, const QString &floorName)
{
    bool result = m_object->removeFloorFromBuilding(buildName, floorName);
//    // обновить конфигурацию
//    this->updateConfig();
    return result;
}


QString IssoCore::addBuilding(const QString &name, int number)
{
    QString buildName = m_object->addBuilding(name, number);
//    // обновить конфигурацию
//    this->updateConfig();
    return buildName;
}


void IssoCore::removeBuilding(const QString &name)
{
    m_object->removeBuilding(name);
//    // обновить конфигурацию
//    this->updateConfig();
}


IssoModuleState IssoCore::objectState() const
{
    return m_object->state();
}


void IssoCore::sendMessageToRemoteLog(const QString &msg)
{
    if (m_remoteLogger)
        m_remoteLogger->addMessage(msg);
}


void IssoCore::clearRemoteLog()
{
    if (m_remoteLogger)
        m_remoteLogger->clearLog();
}


void IssoCore::sendObjectCfgToRemoteLog(const QString &cfg)
{
    if (m_remoteLogger)
        m_remoteLogger->addObject(cfg);
}


//void IssoCore::sendPhotoToServer(int id, const QString& base64Data)
//{
//    if (m_remoteLogger)
//        m_remoteLogger->sendEventPhoto(id, base64Data);
//}


void IssoCore::setCurrentIp(const QHostAddress &ip)
{
    m_object->setCurrentIp(ip);
}


void IssoCore::setCurrentMac(const QByteArray &mac)
{
    m_object->setCurrentMac(mac);
}


void IssoCore::addBuilding(int buildNum, int lowerFloorNum, int floorCount,
                           const QMap<QString, QString>& imageMap,
                           const QMap<int, int>& moduleCountMap,
                           const QSet<IssoParamId>& checkableParams)
{
    m_object->addBuilding(buildNum, lowerFloorNum, floorCount,
                           imageMap, moduleCountMap, checkableParams);
//    // обновить конфигурацию
//    updateConfig();
}


//void IssoCore::updateConfig()
//{
//    // обновить списки модулей менеджера обмена
//    if (m_exchanger)
//        m_exchanger->setModules(locatedModules());
//}


void IssoCore::requestMultiSensorRawInfo(int moduleId, quint8 multiSensorId)
{
    if (m_exchanger)
        m_exchanger->requestMsRawInfo(moduleId, multiSensorId);

//    if (!m_exchanger)
//        return;
////    // запросить подтверждение
////    bool res = IssoCommonData::showQuestionDialog(
////                    qApp->activeWindow(), tr("Подтверждение"),
////                    tr("Вы действительно хотите запросить "
////                    "расширенное состояние мультидатчика %1 модуля %2?")
////                                            .arg(multiSensorId)
////                                            .arg(module->displayName()));
//    // запросить расширенное состояние мультидатчика
////    if (res)
//        m_exchanger->requestMsRawInfo(moduleId->uniqueId(), multiSensorId);
}


void IssoCore::handleResetMultiSensorState(IssoModule *module, quint8 multiSensorId)
{
    if (!m_exchanger)
        return;
    // запросить подтверждение
    bool res = IssoCommonData::showQuestionDialog(
                    qApp->activeWindow(), tr("Подтверждение"),
                    tr("Вы действительно хотите сбросить "
                    "состояние мультидатчика %1 модуля %2?")
                        .arg(multiSensorId)
                        .arg(module->displayName()));
    if (!res)
        return;
    // вывести диалог ввода пароля
    res = PasswordDialog::inputPassword(nullptr, IssoConst::MODIFY_PASSWORD,
                                        tr("Введен неверный пароль! Действие отклонено"));
    // сбросить состояние детектора движения
    if (res)
    {
        // сбросить тревоги мультидатчика
        m_exchanger->resetMsState(module->uniqueId(), multiSensorId);
    }
}

/**
 * @brief IssoCore::handleResetMoveDetectorState сбросить состояние детектора движения
 * @param module
 * @param chainId
 * @param chainType
 */
void IssoCore::handleResetMoveDetectorState(IssoModule *module, quint8 chainId,
                                      IssoChainType chainType)
{
    if (!m_controller)
        return;
    // запросить подтверждение
    bool res = IssoCommonData::showQuestionDialog(
                    qApp->activeWindow(), tr("Подтверждение"),
                    tr("Вы действительно хотите сбросить "
                    "состояние выбранного детектора движения модуля %1?")
                                            .arg(module->displayName()));
    // сбросить состояние детектора движения
    if (res)
    {
        // получить ID параметра, соответствующего шлейфу
        IssoParamId paramId = IssoModule::chainIdToParamId(chainId, chainType);
        if (paramId == PARAM_UNDEFINED)
            return;
        // создать буфер параметров
        IssoParamBuffer buffer;
        // добавить в буфер параметр состояния детектора движения (Норма)
        IssoSensorStateParam* param = new IssoSensorStateParam(paramId, STATE_NORMAL);
        buffer.insertParamPtr(QSharedPointer<IssoParam>(param));
        // обновить буфер параметров модуля
        m_controller->updateModuleParams(module->displayName(), buffer);
    }
}


void IssoCore::handleResetAllMoveDetectorsStates()
{
    if (!m_object)
        return;
    // запросить подтверждение
    bool res = IssoCommonData::showQuestionDialog(
                    qApp->activeWindow(), tr("Подтверждение"),
                    tr("Вы действительно хотите сбросить "
                    "состояния всех детекторов движения системы?"));
    if (!res)
        return;
    // получить список распределенных модулей
    auto modules = m_object->locatedModules();
    //
    // вывести диалог прогресса операции
    QProgressDialog* dlg = IssoCommonData::makeProgressDialog(
                                "Подождите", "Выполняется сброс состояний детекторов движения...",
                                0, modules.size(), qApp->activeWindow());
    // настроить доступ к диалогу из другого потока
    connect(this, SIGNAL(progressCurrent(int)),
            dlg,  SLOT(setValue(int)));
    connect(this, SIGNAL(progressFinished()),
            dlg,  SLOT(close()));
    // вывести диалог прогресса
    dlg->show();
    // запустить метод сброса состояний детектора асинхронно
    QtConcurrent::run(this, &IssoCore::resetAllMoveDetectorsStates, modules);
}


void IssoCore::resetAllMoveDetectorsStates(const QList<IssoModule *> modules)
{
    if (!m_object || !m_controller)
        return;
    // получить список имен конфигов шлейфов типа Детектор движения
    QStringList detectorCfgNames = m_object->chainConfigNames(MOVE_DETECTOR_CHAIN);
    if (detectorCfgNames.isEmpty())
        return;
    //
    // количество обработанных модулей
    int moduleCount = 0;
    //
    // обойти все распределенные модули
    foreach (IssoModule* module, modules)
    {
        // получить конфиг модуля
        IssoModuleConfig* cfg = m_object->moduleConfig(module->moduleConfigName());
        if (!cfg)
            continue;
        //
        // найти все ссылки на конфиги детекторов движения
        foreach (QString detectorCfgName, detectorCfgNames)
        {
            // если ссылка на конфиг шлейфа найден в конфиге модуля
            if (cfg->containsChainConfigLink(detectorCfgName))
            {
                // получить ссылку на конфиг шлейфа по имени
                IssoChainConfigLink link = cfg->chainConfigLink(detectorCfgName);
                // получить параметр шлейфа
                IssoParamId paramId = link.paramId();
                //
                if (paramId == PARAM_UNDEFINED)
                    continue;
                // создать буфер параметров
                IssoParamBuffer buffer;
                // добавить в буфер параметр состояния детектора движения (Норма)
                IssoSensorStateParam* param = new IssoSensorStateParam(paramId, STATE_NORMAL);
                buffer.insertParamPtr(QSharedPointer<IssoParam>(param));
                // обновить буфер параметров модуля
                m_controller->updateModuleParams(module->displayName(), buffer);

//                qDebug() << "resetAllMoveDetectorsStates():   "
//                         << QString("ModuleName = %1").arg(module->displayName()) << endl;
            }
        }
        // обновить прогресс
        emit progressCurrent(++moduleCount);
    }
    emit progressFinished();
}


void IssoCore::sortModules()
{
    m_object->sortModules();
}


QList<IssoModule *> IssoCore::filteredModules(int buildNum, int floorNum)
{
    return m_object->filteredModules(buildNum, floorNum);
}


QList<IssoModule *> IssoCore::filteredModules(const QString &buildName,
                                              const QString &floorName)
{
    return m_object->filteredModules(buildName, floorName);
}


IssoObjectCard IssoCore::objectCard() const
{
    return m_object->objectCard();
}


void IssoCore::setObjectCard(const IssoObjectCard &objectCard)
{
    m_object->setObjectCard(objectCard);
}


void IssoCore::init(IssoObject *object)
{
//    m_object = new IssoObject();

    setGuardedObject(object);

    // инициализировать менеджер обмена пакетами
    initExchangeManager();
    // инициализировать обработчик ответных пакетов
    initModulesController();
    // инициализировать удаленный регистратор событий
    initRemoteLogger();
    // инициализировать захватчик видео
    initCameraCapture();
    // инициализировать аудио контроллер
    initAudioController();
    //
    initScenarioExecutor();
    //
    initVideoProcessor();
}


void IssoCore::uninit()
{
    // остановить обработку видео
    stopVideo();
    // отключить обработчики видеопроцессора
    uninitVideoProcessor();
    // уничтожить испонитель сценариев
    destroyScenarioExecutor();
    // уничтожить аудио контроллер
    destroyAudioController();
    // уничтожить захватчик видео
    destroyCameraCapture();
    // уничтожить удаленный регистратор событий
    destroyRemoteLogger();
    // уничтожить обработчик ответов
    destroyModulesController();
    // уничтожить менеджер обмена
    destroyExchangeManager();
    // уничтожить охраняемый объект
    destroyGuardedObject();
}

void IssoCore::initExchangeManager()
{
    if (!m_object)
        return;
    // получить список всех модулей системы
    QList<IssoModule*> modules = locatedModules();
    // контроллер модулей МАСО
    m_exchanger = new IssoExchangeManager(m_object->serverIp(),
                                          IssoConst::RECV_PORT,
                                          IssoConst::ALARM_PORT/*, modules*/);
    m_exchanger->setObject(m_object);
    // обработчики сигналов контроллера
    connect(m_exchanger, SIGNAL(requestSent(IssoModuleRequest)),
            this,        SIGNAL(requestSent(IssoModuleRequest)));
    connect(m_exchanger, SIGNAL(replyReceived(IssoModuleReply,bool)),
            this,        SIGNAL(replyReceived(IssoModuleReply,bool)));
    connect(m_exchanger, SIGNAL(replyTimedOut(IssoModuleRequest)),
            this,        SIGNAL(replyTimedOut(IssoModuleRequest)));
    connect(m_exchanger, SIGNAL(replyDataReceived(QHostAddress,quint16,QByteArray)),
            this,        SIGNAL(replyDataReceived(QHostAddress,quint16,QByteArray)));
    connect(m_exchanger, SIGNAL(alarmDataReceived(QHostAddress,quint16,QByteArray)),
            this,        SIGNAL(alarmDataReceived(QHostAddress,quint16,QByteArray)));

}


void IssoCore::destroyExchangeManager()
{
    if (m_exchanger)
    {
        m_exchanger->disconnect();

        m_exchanger->stopExchanging();
        delete m_exchanger;
        m_exchanger = nullptr;
    }
}


void IssoCore::initScenarioExecutor()
{
    m_scenarioExecutor = new IssoScenarioExecutor(m_exchanger);
    m_scenarioExecutor->setObject(m_object);

    connect(m_scenarioExecutor, SIGNAL(scenarioStateChanged(IssoScenarioInfo,bool)),
            this,               SIGNAL(scenarioStateChanged(IssoScenarioInfo,bool)));
}


void IssoCore::destroyScenarioExecutor()
{
    if (m_scenarioExecutor)
    {
        delete m_scenarioExecutor;
        m_scenarioExecutor = nullptr;
    }
}


void IssoCore::initModulesController()
{
    m_controller = new IssoModulesController(/*buildings()*/);
    m_controller->setObject(m_object);

    // связать менеджер обмена с контроллером модулей
    connect(m_exchanger,   SIGNAL(replyReceived(IssoModuleReply,bool)),
            m_controller,  SLOT(processReplyReceived(IssoModuleReply,bool)));
    connect(m_exchanger,   SIGNAL(replyTimedOut(IssoModuleRequest)),
            m_controller,  SLOT(processReplyTimedOut(IssoModuleRequest)));
    connect(m_exchanger,   SIGNAL(alarmReceived(IssoModuleReply)),
            m_controller,  SLOT(processAlarmReceived(IssoModuleReply)));
    // связать контроллер модулей с GUI
    connect(m_controller,  SIGNAL(descriptorRead(QString,IssoModuleDescriptor)),
            this,           SIGNAL(descriptorRead(QString,IssoModuleDescriptor)));
    connect(m_controller,  SIGNAL(descriptorWritten(QString,IssoModuleDescriptor)),
            this,           SIGNAL(descriptorWritten(QString,IssoModuleDescriptor)));
    connect(m_controller,  SIGNAL(descriptorFailed(QString,IssoCmdId)),
            this,           SIGNAL(descriptorFailed(QString,IssoCmdId)));
    connect(m_controller,  SIGNAL(sensorStatesChanged(QString)),
            this,           SIGNAL(sensorStatesChanged(QString)));
    connect(m_controller,  SIGNAL(relayStateChanged(QString,IssoParamId,bool)),
            this,           SLOT(processRelayStateChanged(QString,IssoParamId,bool)));
    connect(m_controller,  SIGNAL(moduleStateChanged(QString,IssoModuleState)),
            this,           SIGNAL(moduleStateChanged(QString,IssoModuleState)));
    connect(m_controller, SIGNAL(multiSensorRawDataReceived(QString,quint8,IssoState, QMap<IssoDigitalSensorId,IssoState>, QMap<IssoDigitalSensorId,quint16>)),
            this,         SIGNAL(multiSensorRawDataReceived(QString,quint8,IssoState, QMap<IssoDigitalSensorId,IssoState>, QMap<IssoDigitalSensorId,quint16>)));

    connect(m_controller, SIGNAL(multiSensorRegActionCompleted(IssoMsRegAction,QHostAddress,quint8,bool)),
            this,         SIGNAL(multiSensorRegActionCompleted(IssoMsRegAction,QHostAddress,quint8,bool)));

    // активация мультдатчика
    connect(this,        SIGNAL(multiSensorActivationRequested(int,QList<quint8>)),
            m_exchanger, SLOT(startMsActivation(int,QList<quint8>)));
    connect(m_exchanger, SIGNAL(multiSensorActivated(QHostAddress,quint8)),
            this,        SIGNAL(multiSensorActivated(QHostAddress,quint8)));
}


void IssoCore::destroyModulesController()
{
    if (m_controller)
    {
        m_controller->disconnect();

        delete m_controller;
        m_controller = nullptr;
    }
}


void IssoCore::initRemoteLogger()
{
    m_remoteLogger = new IssoRemoteLogger(IssoConst::LOG_SERVER_URL);
    // связать сигналы
    connect(m_remoteLogger, SIGNAL(requestCompleted(QString)),
            this,           SIGNAL(remoteLogCompleted(QString)));
    connect(m_remoteLogger, SIGNAL(requestFailed(QString)),
            this,           SIGNAL(remoteLogFailed(QString)));
}


void IssoCore::destroyRemoteLogger()
{
    if (m_remoteLogger)
    {
        m_remoteLogger->disconnect();

        delete m_remoteLogger;
        m_remoteLogger = nullptr;
    }
}


void IssoCore::initCameraCapture()
{
    // создать окно отображения видео
    m_cameraView = new CameraViewFrame();
    // создать захватчик видео
    m_cameraCapture = new IpCameraCapture();
    // задать обработчики
    //
    connect(m_cameraCapture, SIGNAL(started(QHostAddress)),
            this,             SLOT(processCameraCaptureStarted(QHostAddress)));
    connect(m_cameraCapture, SIGNAL(stopped(QHostAddress)),
            this,             SLOT(processCameraCaptureStopped(QHostAddress)));
    connect(m_cameraCapture, SIGNAL(frameCaptured(QImage)),
            m_cameraView,    SLOT(displayFrame(QImage)));

    connect(m_cameraCapture, SIGNAL(openStreamFailed(QHostAddress)),
            this,            SIGNAL(openVideoStreamFailed(QHostAddress)));
    connect(m_cameraCapture, SIGNAL(openStreamFailed(QHostAddress)),
            this,            SLOT(clearDisplayCameraId()));

    connect(m_cameraCapture, SIGNAL(readStreamFailed(QHostAddress)),
            this,            SIGNAL(readVideoStreamFailed(QHostAddress)));
    connect(m_cameraCapture, SIGNAL(readStreamFailed(QHostAddress)),
            this,            SLOT(clearDisplayCameraId()));

    // cameraView
    connect(m_cameraView,    SIGNAL(closed()),
            m_cameraCapture, SLOT(stop()));
}


void IssoCore::destroyCameraCapture()
{
    if (m_cameraCapture)
    {
        disconnect(m_cameraCapture, 0, 0, 0);
        delete m_cameraCapture;
        m_cameraCapture = nullptr;
    }
    if (m_cameraView)
    {
        disconnect(m_cameraView, 0, 0, 0);
        delete m_cameraView;
        m_cameraView = nullptr;
    }
}


void IssoCore::initAudioController()
{
    if (!m_object)
        return;
    m_audioController = new IssoAudioController(m_object->serverIp(),
                                                IssoConst::AUDIO_RECV_PORT,
                                                IssoConst::AUDIO_FORMAT_SAMPLE_RATE,
                                                IssoConst::AUDIO_FORMAT_CHANNEL_COUNT,
                                                IssoConst::AUDIO_FORMAT_SAMPLE_SIZE);
}


void IssoCore::destroyAudioController()
{
    if (m_audioController)
    {
        delete m_audioController;
        m_audioController = nullptr;
    }
}


void IssoCore::initVideoProcessor()
{
//    connect(&m_videoProcessor, &IssoVideoFrameProcessor::cameraSnapshotReady,
//            this,              &IssoCore::processCameraSnapshotReady);
    connect(&m_videoProcessor, &IssoVideoFrameProcessor::cameraSnapshotReady,
            this,              &IssoCore::cameraSnapshotReady);
}


void IssoCore::uninitVideoProcessor()
{
//    disconnect(&m_videoProcessor, &IssoVideoFrameProcessor::cameraSnapshotReady,
//               this,              &IssoCore::processCameraSnapshotReady);
    disconnect(&m_videoProcessor, &IssoVideoFrameProcessor::cameraSnapshotReady,
               this,              &IssoCore::cameraSnapshotReady);
}


bool IssoCore::displayCamera(int cameraId)
{
    // найти камеру
    IssoCamera* camera = m_object->findCameraById(cameraId);
    if (!camera)
        return false;
    // запустить видеозахват
    startVideo(camera->ip(), camera->port(), camera->password());
    return true;
}


//bool IssoCore::displayCamera(int cameraId)
bool IssoCore::displayCameraByAlarm(QDateTime dateTime, const QString &moduleName,
                                    int multiSensorId, int cameraId)
{
    // найти камеру
    IssoCamera* camera = m_object->findCameraById(cameraId);
    if (!camera)
        return false;
    // если режим передачи изображения - фото
    if (camera->sendMode() == CAMERA_SEND_PHOTO)
    {
        // задать обработчик видеокадра
        connect(m_cameraCapture, &IpCameraCapture::frameCaptured,
                this,            &IssoCore::processSnapshot);
        // запустить поток обработки
        m_videoProcessor.start(dateTime, moduleName, multiSensorId,
                               cameraId, camera->ip().toIPv4Address());
    }
    //
    // запустить видеозахват
    startVideo(camera->ip(), camera->port(), camera->password());
    return true;
}


void IssoCore::startVideo(const QHostAddress &ip, quint16 port,
                          const QString& password)
{
    Q_UNUSED(port);
    // начать захват видео
    m_cameraCapture->start(ip, password);
}


void IssoCore::stopVideo()
{
    // завершить захват видео
    m_cameraCapture->stop();
    // остановить поток обработки
    m_videoProcessor.stop();
}


void IssoCore::processSnapshot(const QImage &frame)
{
    // если обработчик видеокадров не остановлен
    if (!m_videoProcessor.isRunning())
        return;
    //
    // снять данный обработчик (нужно обработать только 1 кадр)
    disconnect(m_cameraCapture, &IpCameraCapture::frameCaptured,
               this,            &IssoCore::processSnapshot);
    //
    // добавить кадр в очередь на обработку
    m_videoProcessor.enqueueFrame(frame);
    // остановить обработку видеокадров
    m_videoProcessor.stop();
}


bool IssoCore::isDisplayingVideo() const
{
    return m_cameraCapture->isRunning();
}


int IssoCore::displayingCameraId() const
{
    return m_displayingCameraId;
}


void IssoCore::playVoiceRecord(const QHostAddress &ip, quint16 port,
                               const QByteArray &voiceRecord)
{
    m_audioController->playVoiceRecordAsync(ip, port, voiceRecord);
}


void IssoCore::playModuleAudioRecord(const QHostAddress &ip, quint16 port,
                                     quint8 recordNum)
{
    m_audioController->playModuleRecordAsync(ip, port, recordNum);
}


void IssoCore::playPcAudioRecord(const QHostAddress &ip, quint16 port,
                                 const QString &fileName)
{
    m_audioController->playMp3FileAsync(ip, port, fileName);
}


void IssoCore::turnModuleRelayAsync(int moduleId, const IssoParamId &relayId, bool turnOn)
{
    m_exchanger->turnRelay(moduleId, relayId, turnOn);
}


void IssoCore::turnBroadcastRelayAsync(const IssoParamId &relayId, bool turnOn)
{
    m_exchanger->turnRelayBroadcast(relayId, turnOn);
}


void IssoCore::readDescriptor(const int moduleId)
{
    if (m_exchanger)
        m_exchanger->readDescriptor(moduleId);
}

void IssoCore::writeDescriptor(const int moduleId, const IssoModuleDescriptor &desc)
{
    if (m_exchanger)
        m_exchanger->writeDescriptor(moduleId, desc);
}


void IssoCore::requestModuleInfo(int moduleId)
{
    if (m_exchanger)
        m_exchanger->requestModuleInfo(moduleId);
}

void IssoCore::updateModuleConfig(int moduleId)
{
    if (m_exchanger)
        m_exchanger->updateModuleConfig(moduleId);
}


void IssoCore::setScenarioActive(IssoScenarioInfo scenarioInfo, bool active)
{
    if (!m_scenarioExecutor)
        return;
    if (active)
        m_scenarioExecutor->start(scenarioInfo);
    else
        m_scenarioExecutor->stop(scenarioInfo);
}


void IssoCore::handleSensorAction(quint8 sensorId, int moduleId, quint8 chainId,
                                  IssoChainType chainType, IssoSensorAction action)
{
    //
//    qDebug() << QString("sensorId = %1; moduleId = %2; chainId = %3; "
//                        "chainType = %4; action = %5")
//                                        .arg(sensorId)
//                                        .arg(moduleId)
//                                        .arg(chainId)
//                                        .arg(chainType)
//                                        .arg(action) << endl;
    //
    if (!m_exchanger)
        return;
    // найти модуль по ID
    IssoModule* module = m_object->findModuleById(moduleId);
    if (!module)
        return;
    // выполнить действие
    switch (action)
    {
        // сбросить тревогу
        case RESET_ALARM:
        {
            // адресного шлейфа
            if (chainType == MULTICRITERIAL)
            {
                handleResetMultiSensorState(module, sensorId);
            }
            // детектора движения
            else if (chainType == MOVE_DETECTOR_CHAIN)
            {
                handleResetMoveDetectorState(module, chainId, chainType);
            }
            break;
        }
        // сбросить все тревоги
        case RESET_ALL_ALARMS:
        {
            // всех детекторов движения
            if (chainType == MOVE_DETECTOR_CHAIN)
            {
                handleResetAllMoveDetectorsStates();
            }
            break;
        }
        // включить сервисный режим датчика
        case SERVICE_MODE_ON:
        // отключить сервисный режим датчика
        case SERVICE_MODE_OFF:
        {
            module->setMultiSensorServiceMode(sensorId, action == SERVICE_MODE_ON);
            break;
        }
        // запросить расширенное состояние мультидатчика
        case GET_RAW_DATA:
        {
            if (chainType == MULTICRITERIAL)
            {
                requestMultiSensorRawInfo(module->uniqueId(), sensorId);
            }
            break;
        }
        default:
            break;
    }

}


void IssoCore::handleMultiSensorRegAction(IssoMsRegAction action, int moduleId, quint8 msId)
{
    if (!m_exchanger)
        return;

    switch(action)
    {
        case IssoMsRegAction::READ_THRESHOLD:
        {
            m_exchanger->readMsThresholds(moduleId, msId);
            break;
        }
        case IssoMsRegAction::WRITE_THRESHOLD:
        {
            m_exchanger->writeMsThresholds(moduleId, msId);
            break;
        }
        case IssoMsRegAction::RESET_STATE:
        {
            m_exchanger->resetMsState(moduleId, msId);
            break;
        }
        case IssoMsRegAction::READ_RAW_DATA:
        {
            m_exchanger->requestMsRawInfo(moduleId, msId);
            break;
        }
        case IssoMsRegAction::BEEP:
        {
            m_exchanger->shortBeep(moduleId, msId);
            break;
        }
        case IssoMsRegAction::CALIBRATE:
        {
            m_exchanger->startCalibration(moduleId, msId);
            break;
        }
        default:
            break;
    }
}


void IssoCore::updateModuleSettings(int moduleId)
{
//    updateConfig();
    requestModuleInfo(moduleId);
}


void IssoCore::processCameraCaptureStarted(const QHostAddress &ip)
{
    // изменить статус камеры
    if (m_object)
    {
        // найти камеру по ip
        IssoCamera* camera = m_object->findCameraByIp(ip);
        if (camera)
        {
            // установить статус камеры online
            camera->setOnline(true);
            // показать видео окно
            m_cameraView->show(tr("%1 (%2)")
                               .arg(camera->displayName())
                               .arg(camera->ip().toString()));
            // запомнить ID отображаемой камеры
            m_displayingCameraId = camera->id();
        }
    }
}


void IssoCore::processCameraCaptureStopped(const QHostAddress &ip)
{
    // скрыть видео окно
    //    m_cameraView->hide();
    if (m_cameraView->isVisible())
        m_cameraView->close();
    // изменить статус камеры
    if (m_object)
    {
        // найти камеру по ip
        IssoCamera* camera = m_object->findCameraByIp(ip);
        // установить статус камеры offline
        if (camera)
            camera->setOnline(false);
        // сбросить ID отображаемой камеры
        clearDisplayCameraId();
    }
}


void IssoCore::processRelayStateChanged(const QString &moduleName,
                                        IssoParamId paramId, bool pressed)
{
    // известить о событии
    emit relayStateChanged(moduleName, paramId, pressed);
    //
    // если реле Сброс включено, выключить через 2 сек
    if ((!m_watcher.isRunning()) && (paramId == EXT_RELAY_2) && (pressed))
    {
        QFuture<void> future =
                QtConcurrent::run(this, &IssoCore::sendDelayedReset, moduleName, RELAY_RESET_DELAY);
        m_watcher.setFuture(future);
    }
}

void IssoCore::clearDisplayCameraId()
{
    // сбросить ID отображаемой камеры
    m_displayingCameraId = -1;
}


//void IssoCore::processVideoFrameDataReady(const QDateTime &dateTime, const QString &moduleName,
//                                          int multiSensorId, int cameraId, int cameraIp,
//                                          const QImage& frame   /*const QString &base64Frame*/)
//{
////    // преобразовать изображение
////    QImage img = IssoCommonData::convertBase64ToImage(base64Frame);
////    // убедиться, что директория сохранения кадров существует
////    QDir dir("VideoFrames");
////    if (!dir.exists())
////        dir.mkpath(".");
////    // сформировать имя кадра
////    QString fileName = dir.absoluteFilePath(QString("[%1]_%2_ИПМ%3_Камера%4.jpg")
////                                            .arg(dateTime.toString("dd.MM.yyyy_HH-mm-ss"))
////                                            .arg(moduleName)
////                                            .arg(multiSensorId)
////                                            .arg(cameraId));
////    // сохранить видеокадр
////    img.save(fileName, "JPG");

//    // убедиться, что директория сохранения кадров существует
//    QDir dir("VideoFrames");
//    if (!dir.exists())
//        dir.mkpath(".");
//    // сформировать имя кадра
//    QString fileName = dir.absoluteFilePath(QString("[%1]_%2_ИПМ%3_Камера%4.jpg")
//                                            .arg(dateTime.toString("dd.MM.yyyy_HH-mm-ss"))
//                                            .arg(moduleName)
//                                            .arg(multiSensorId)
//                                            .arg(cameraId));
//    // сохранить видеокадр
//    frame.save(fileName, "JPG");
//}


void IssoCore::sendDelayedReset(const QString &moduleName, long secs)
{
    qDebug() << "IssoCore::sendDelayedReset" << endl;
    //
    QThread::sleep(secs);
    // найти модуль по имени
    IssoModule* module = findModuleByDisplayName(moduleName);
    if (module)
        turnModuleRelayAsync(module->uniqueId(), EXT_RELAY_2, false);
}


void IssoCore::cancelBackgroundTask()
{
    // если выполняется фоновая задача
    if (m_watcher.isRunning())
    {
        // отменить ее
        m_watcher.cancel();
        // и дождаться завершения
        m_watcher.waitForFinished();
    }
}


QList<int> IssoCore::availableBuildNumbers()
{
    return m_object->availableBuildNumbers();
}


QList<int> IssoCore::availableFloorNumbers()
{
    return m_object->availableFloorNumbers();
}


QList<IssoBuilding *> &IssoCore::buildings()
{
    return m_object->buildings();
}


IssoBuilding *IssoCore::currentBuilding()
{
    return m_object->currentBuilding();
}


void IssoCore::setCurrentBuilding(IssoBuilding *building)
{
    m_object->setCurrentBuilding(building);
}


IssoFloorPlan *IssoCore::currentFloor()
{
    return m_object->currentFloor();
}


void IssoCore::setCurrentFloor(IssoFloorPlan *floor)
{
    m_object->setCurrentFloor(floor);
}


IssoBuilding *IssoCore::findBuildingByName(const QString &name)
{
    return m_object->findBuildingByName(name);
}


IssoBuilding *IssoCore::findBuildingByNumber(int num)
{
    return m_object->findBuildingByNumber(num);
}


IssoModule *IssoCore::findModuleByDisplayName(const QString &displayName)
{
    return m_object->findModuleByDisplayName(displayName);
}


IssoModule *IssoCore::findModuleById(int uniqueId)
{
    return m_object->findModuleById(uniqueId);
}


IssoExchangeManager *IssoCore::exchanger()
{
    return m_exchanger;
}


IssoModulesController *IssoCore::modulesController()
{
    return m_controller;
}


QList<IssoModule *> IssoCore::locatedModules() const
{
    return m_object->locatedModules();
}


QStringList IssoCore::unlocatedModuleNames() const
{
    return m_object->unlocatedModuleNames();
}


QStringList IssoCore::moduleNames() const
{
    return m_object->moduleNames();
}


QStringList IssoCore::buildingNames() const
{
    return m_object->buildingNames();
}


IssoModule *IssoCore::selectedModule()
{
    return m_object->selectedModule();
}


IssoFloorPlan *IssoCore::findFloor(int buildNum, int floorNum)
{
    return m_object->findFloor(buildNum, floorNum);
}


bool IssoCore::changeModuleLocation(IssoModule *module, int buildNum, int floorNum)
{
    bool res = m_object->changeModuleLocation(module, buildNum, floorNum);
//    // обновить конфигурацию
//    if (res)
//        updateConfig();
    return res;
}


void IssoCore::loadConfig(QDataStream &stream)
{
    m_object->load(stream);
//    // обновить конфигурацию
//    updateConfig();
}


void IssoCore::saveConfig(QDataStream &stream)
{
    m_object->save(stream);
}


bool IssoCore::loadConfig(const QString& fileName)
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
        QDataStream in(ba);
        // загрузить конфигурацию из потока
        loadConfig(in);
        return true;
    }
    else
        return false;
}


void IssoCore::saveConfig(const QString& fileName)
{
    // создать файл
    QFile file(fileName);
    // открыть файл для записи
    if (file.open(QIODevice::WriteOnly))
    {
        // создать поток для записи
        QDataStream out(&file);
        // сохранить здания в поток
        saveConfig(out);
    }
    file.close();
}
