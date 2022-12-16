/**
  @brief IssoCore - основной управляющий объект
  содержит в себе объекты:
    - содержит в себе определения сигналов и слотов
    - m_object охраняемый объект
    - m_exchanger менеджер обмена данными с модулями
    - m_controller контроллер состояний модулей
    - m_remoteLogger регистратор событий на удаленном сервере
    - m_cameraCapture захватчик видео
    - m_displayingCameraId ID отображаемой камеры
*/
#ifndef ISSOCORE_H
#define ISSOCORE_H

#include "issobuilding.h"
#include "issoexchangemanager.h"
#include "issolocallogger.h"
#include "issomodulescontroller.h"
#include "issoremotelogger.h"
#include "ipcameracapture.h"
#include "cameraviewframe.h"
#include "issoaudiocontroller.h"
#include "issoscenarioexecutor.h"

#include <QLabel>
#include <QObject>
#include <QSound>
#include <QMessageBox>
#include "issoobjectcard.h"
#include "issoobject.h"
#include "issovideoframeprocessor.h"

class IssoCore : public QObject
{
        Q_OBJECT

    private:
        const long RELAY_RESET_DELAY = 5;
        // охраняемый объект
        IssoObject* m_object = nullptr;
        // менеджер обмена данными с модулями
        IssoExchangeManager* m_exchanger = nullptr;
        // контроллер состояний модулей
        IssoModulesController* m_controller = nullptr;
        // регистратор событий на удаленном сервере
        IssoRemoteLogger* m_remoteLogger = nullptr;
        // захватчик видео
        IpCameraCapture* m_cameraCapture = nullptr;
        // ID отображаемой камеры
        int m_displayingCameraId = -1;

        // окно отображения видео с камеры
//        CameraViewFrame* m_cameraView = nullptr;
        // контроллер аудиозаписей
        IssoAudioController* m_audioController = nullptr;

        QFutureWatcher<void> m_watcher;

        IssoScenarioExecutor* m_scenarioExecutor;

        IssoVideoFrameProcessor m_videoProcessor;


        /// \brief  Инициализировать менеджер обмена данными с модулями
        void initExchangeManager();
        /// \brief  Уничтожить менеджер обмена данными с модулями
        void destroyExchangeManager();

        /// \brief  Инициализировать исполнитель сценариев
        void initScenarioExecutor();
        /// \brief  Уничтожить исполнитель сценариев
        void destroyScenarioExecutor();


        /// \brief  Инициализировать контроллер модулей, отвечающий за обработку
        ///         ответов (разбор и отображение состояний) модулей и
        ///         извещение об изменениях состояний модулей
        void initModulesController();
        /// \brief  Уничтожить контроллер модулей
        void destroyModulesController();
        /// \brief  Инициализировать удаленный регистратор событий
        void initRemoteLogger();
        /// \brief  Уничтожить удаленный регистратор событий
        void destroyRemoteLogger();
        /// \brief  Инициализировать захватчик видео
        void initCameraCapture();
        /// \brief  Уничтожить захватчик видео
        void destroyCameraCapture();
        /// \brief  Инициализировать аудио контроллер
        void initAudioController();
        /// \brief  Уничтожить аудио контроллер
        void destroyAudioController();

        void initVideoProcessor();
        void uninitVideoProcessor();

//        /// \brief  Обновить текущую конфигурацию
//        void updateConfig();

        void handleResetMultiSensorState(IssoModule *module, quint8 multiSensorId);
        /**
         * @brief IssoCore::handleResetMoveDetectorState сбросить состояние детектора движения
         * @param module
         * @param chainId
         * @param chainType
         */
        void handleResetMoveDetectorState(IssoModule *module, quint8 chainId, IssoChainType chainType);
        void handleResetAllMoveDetectorsStates();

        void resetAllMoveDetectorsStates(const QList<IssoModule*> modules);

    public:
        CameraViewFrame* m_cameraView = nullptr;

        explicit IssoCore(QObject *parent = nullptr);
        ~IssoCore();

        /// \brief  Подготовить к работе
//        void init();
        void init(IssoObject* object);
        /// \brief  Подготовить к завершению работы
        void uninit();

        /**
         * @brief guardedObject Получить охраняемый объект
         * @return ссылка на  охраняемый объект
         */
        IssoObject* guardedObject() const;
        /**
         * @brief setGuardedObject Задать охраняемый объек
         * @param guardedObject ссылка на  охраняемый объект
         */
        void setGuardedObject(IssoObject *guardedObject); 
        /**
         * @brief destroyGuardedObject Уничтожить охраяняемый объект
         */
        void destroyGuardedObject();
        /**
         * @brief loadConfig Загрузить здания из потока
         * @param stream поток
         */
        void loadConfig(QDataStream& stream);
        /**
         * @brief saveConfig Сохранить здания в поток
         * @param stream поток
         */
         void saveConfig(QDataStream& stream);
         /**
         * @brief loadConfig Загрузить здания из файла
         * @param fileName файл
         * @return успех
         */
        bool loadConfig(const QString &fileName);
        /**
         * @brief saveConfig Сохранить здания в файл
         * @param fileName файл
         */
        void saveConfig(const QString &fileName);

        /// \brief  Найти здание по имени
        IssoBuilding* findBuildingByName(const QString& name);
        /// \brief  Найти здание по номеру
        IssoBuilding* findBuildingByNumber(int num);
        /// \brief  Найти модуль по отображаемому имени
        IssoModule* findModuleByDisplayName(const QString& displayName);
        /// \brief  Найти модуль по id
        IssoModule* findModuleById(int uniqueId);
        /// \brief  Найти этаж по номеру здания и этажа
        IssoFloorPlan* findFloor(int buildNum, int floorNum);
        /// \brief  Изменить месторасположение модуля
        bool changeModuleLocation(IssoModule *module, int buildNum, int floorNum);
        /// \brief  Получить список всех модулей системы
        ///         (включая нераспределенные по зданиям)
        QList<IssoModule*> modules() const;

        void addModule(IssoModule* module);
        QString addModule(int displayId, const QHostAddress& ip, quint16 port, int uniqueId);
        void addModules(const QList<IssoModule*>& modules);
        void removeModule(IssoModule* module);
        bool removeModule(const QString& moduleName);
        bool addFloorToBuilding(const QString& buildName, int floorNum,
                                const QPixmap& background);
        bool removeFloorFromBuilding(const QString& buildName,
                                     const QString& floorName);
        QString addBuilding(const QString& name, int number);
        void removeBuilding(const QString& name);        

        IssoModuleState objectState() const;

        /// \brief  Сформировать список модулей, распределенных по зданиям
        QList<IssoModule*> locatedModules() const;
        QStringList unlocatedModuleNames() const;
        QStringList moduleNames() const;
        QStringList buildingNames() const;
        /// \brief  Получить выбранный модуль
        IssoModule* selectedModule();
        /// \brief  Задать стартовый IP-адрес
        void setCurrentIp(const QHostAddress& ip);
        /// \brief  Задать стартовый MAC-адрес
        void setCurrentMac(const QByteArray& mac);
        /// \brief  Добавить здание в охраняемый объект
        void addBuilding(int buildNum, int lowerFloorNum, int floorCount,
                         const QMap<QString, QString> &imageMap,
                         const QMap<int, int> &moduleCountMap,
                         const QSet<IssoParamId> &checkableParams);
        /// \brief  Получить карточку охраняемого объекта
        IssoObjectCard objectCard() const;
        /// \brief  Задать карточку охраняемого объекта
        void setObjectCard(const IssoObjectCard &objectCard);
        //
        void sortModules();
        //
        QList<IssoModule*> filteredModules(int buildNum, int floorNum);
        QList<IssoModule*> filteredModules(const QString& buildName,
                                           const QString& floorName);
        //
        QList<int> availableBuildNumbers();
        QList<int> availableFloorNumbers();
        //
        QList<IssoBuilding *>& buildings();
        //
        IssoBuilding* currentBuilding();
        void setCurrentBuilding(IssoBuilding* building);
        //
        IssoFloorPlan* currentFloor();
        void setCurrentFloor(IssoFloorPlan* floor);
        //
        void sendMessageToRemoteLog(const QString& msg);
        void clearRemoteLog();
        void sendObjectCfgToRemoteLog(const QString& cfg);
//        void sendPhotoToServer(int id, const QString &base64Data);

        IssoExchangeManager* exchanger();
        IssoModulesController* modulesController();

        bool displayCamera(int cameraId);
        bool displayCameraByAlarm(QDateTime dateTime, const QString& moduleName, int multiSensorId, int cameraId);
//        void startVideo(const QHostAddress& ip, quint16 port, const QString &password);
//        void stopVideo();
        bool isDisplayingVideo() const;
        int displayingCameraId() const;

        void sendDelayedReset(const QString& moduleName, long secs);

        void cancelBackgroundTask();


    signals:
        void requestSent(const IssoModuleRequest&);
        void replyReceived(const IssoModuleReply&, bool);
        void replyTimedOut(const IssoModuleRequest&);
        void replyDataReceived(QHostAddress,quint16,QByteArray);
        void alarmDataReceived(QHostAddress,quint16,QByteArray);

        void descriptorRead(const QString&, const IssoModuleDescriptor& descr);
        void descriptorWritten(const QString& moduleName,
                               const IssoModuleDescriptor& descr);
        void descriptorFailed(const QString& moduleName, IssoCmdId cmdId);

        void remoteLogCompleted(const QString&);
        void remoteLogFailed(const QString&);

        void openVideoStreamFailed(const QHostAddress &ip);
        void readVideoStreamFailed(const QHostAddress &ip);


        void sensorStatesChanged(const QString& moduleName);
        void relayStateChanged(const QString& moduleName,
                               IssoParamId paramId, bool pressed);
        void moduleStateChanged(const QString& moduleName, IssoModuleState state);
        void multiSensorRawDataReceived(/*int moduleId,*/ /*int chainId,*/
                                        const QString& moduleName,
                                        quint8 msId, IssoState msState,
                                        const QMap<IssoDigitalSensorId, IssoState>& sensorStates,
                                        const QMap<IssoDigitalSensorId, quint16>& sensorValues);
        void scenarioStateChanged(IssoScenarioInfo scenarioInfo, bool active);

        void progressCurrent(int count);
        void progressFinished();

        void multiSensorActivationRequested(int moduleId, const QList<quint8>& msIds);
        void multiSensorActivated(const QHostAddress& moduleIp, quint8 msId);

        void multiSensorRegActionCompleted(IssoMsRegAction action,
                                             const QHostAddress& moduleIp,
                                             quint8 msId, bool success);
        void cameraSnapshotReady(const QDateTime& dateTime, const QString& moduleName,
                                 int multiSensorId, int cameraId, int cameraIp,
                                 const QImage& frame);

    private slots:
        void processCameraCaptureStarted(const QHostAddress& ip);
        void processCameraCaptureStopped(const QHostAddress& ip);

        void processRelayStateChanged(const QString& moduleName,
                                      IssoParamId paramId, bool pressed);
//        void processCameraSnapshotReady(const QDateTime& dateTime, const QString& moduleName,
//                                        int multiSensorId, int cameraId, int cameraIp,
//                                        const QImage &frame);
        void clearDisplayCameraId();

    public slots:
        void playVoiceRecord(const QHostAddress& ip, quint16 port,
                             const QByteArray& voiceRecord);
        void playModuleAudioRecord(const QHostAddress& ip, quint16 port,
                                   quint8 recordNum);
        void playPcAudioRecord(const QHostAddress& ip, quint16 port,
                               const QString& fileName);

        void turnModuleRelayAsync(int moduleId, const IssoParamId &relayId, bool turnOn);
        void turnBroadcastRelayAsync(const IssoParamId &relayId, bool turnOn);

        void readDescriptor(const int moduleId);
        void writeDescriptor(const int moduleId,
                             const IssoModuleDescriptor& desc);
        void requestModuleInfo(int moduleId);
        void updateModuleConfig(int moduleId);

        void setScenarioActive(IssoScenarioInfo scenarioInfo, bool active);

        void handleSensorAction(quint8 sensorId, int moduleId, quint8 chainId,
                                IssoChainType chainType, IssoSensorAction action);


        void handleMultiSensorRegAction(IssoMsRegAction action,
                                          int moduleId, quint8 msId);


        void updateModuleSettings(int moduleId);

        void startVideo(const QHostAddress& ip, quint16 port, const QString &password);
        void stopVideo();

        void requestMultiSensorRawInfo(/*IssoModule *module*/int moduleId, quint8 multiSensorId);

        void processSnapshot(const QImage& frame);

};

#endif // ISSOCORE_H
