#ifndef ISSOEXCHANGEMANAGER_H
#define ISSOEXCHANGEMANAGER_H


#include <QObject>
#include <QQueue>
#include <QThread>
#include <QMutex>
#include <QtConcurrent/QtConcurrent>
#include "issoalarmreceiver.h"
#include "issobuilding.h"
#include "issomodule.h"
#include "issomoduledescriptor.h"
#include "issorequestsender.h"
#include "issoparambuffer.h"
#include "issoobject.h"
#include "issoscenarioqueue.h"
#include "issoregisters.h"


class IssoExchangeManager : public QObject
{
        Q_OBJECT
    private:
        static constexpr int MODULE_ID_BROADCAST = -1;
        static constexpr quint8 MULTISENSOR_ID_BROADCAST = 0;

        QHostAddress m_ip;
        quint16 m_bindPort;
        QList<IssoModule*> m_modules;
        IssoObject* m_object;

        IssoRequestSender* m_sender;
        IssoAlarmReceiver* m_alarmReceiver;

        QMutex* m_userLock;

        QMutex m_activationLock;
        volatile bool m_activationRunning = false;

        QMutex* m_packNumLock;
        quint16 m_packNum = 0;
        volatile bool m_running = false;
        QFutureWatcher<void>* m_watcher = nullptr;

        QQueue<IssoModuleRequest> m_userRequests;
        QQueue<IssoModuleRequest> m_activationRequests;

        IssoScenarioQueue m_scenarioRequests;

//        QMap<IssoParamId, IssoParamId> m_analogThresholdIds;

        // добавить запрос пользователя в очередь синхронно
        /**
         * @brief IssoExchangeManager::addToUserQueue добавить запрос в очередь пользовательских запросов
         * @param request запрос
         */
        void addToUserQueue(const IssoModuleRequest &request);
        // добавить список запросов пользователя в очередь синхронно
        void addToUserQueue(const QList<IssoModuleRequest> &requests);
        // добавить запрос пользователя в очередь асинхронно
        /**
         * @brief IssoExchangeManager::addToUserQueueAsync Ставим асинхронный запрос в очередь
         * @param request запрос
         */
        void addToUserQueueAsync(const IssoModuleRequest& request);
        // добавить список запросов пользователя в очередь асинхронно
        void addToUserQueueAsync(const QList<IssoModuleRequest> &requests);

        // добавить список запросов активации мультидатчиков в очередь асинхронно
        void addToActivationQueue(const QList<IssoModuleRequest> &requests);
        // очистить очередь запросов активации мультидатчиков асинхронно
        void clearActivationQueue();


        QSharedPointer<IssoModuleReply> sendRequest(IssoRequestSender &sender,
                                                    const IssoModuleRequest &request,
                                                    bool& ok);
        /**
         * @brief IssoExchangeManager::findModule найти модуль по Id
         * @param moduleId Id модуля
         * @return указатель на модуль с Id
         */
        IssoModule* findModule(const int moduleId);
        /**
         * @brief IssoExchangeManager::findModuleAddressById найти IP и порт модуля по его Id
         * @param moduleId
         * @param addr
         * @param port
         * @return false если модуль не найден
         */
        bool findModuleAddressById(const int moduleId, QHostAddress& addr, quint16& port);
//        IssoMultiSensorConfig* findMultiSensorCfg(int moduleId, quint8 multiSensorId);

        quint16 nextPackNum();
        IssoModuleRequest makeRelayRequest(int moduleId, IssoParamId relayId, bool state, bool& ok);
        void requestStateAsync(int moduleId, IssoCmdId cmdId);


//        IssoArrayParam* makeAnalogThresholdsParam(IssoParamId analogParamId,
//                                                 IssoAnalogChainConfig *analogCfg);
//        void initThresholdParamIds();
//        void setupSensorsThresholds(const QHostAddress& ip, quint16 port,
//                                    IssoModuleConfig* moduleCfg);
//        void setupAnalogChainsThresholds(const QHostAddress& ip, quint16 port,
//                                         IssoModuleConfig* moduleCfg);


        void setupMultiSensorPollingAddrs(IssoModule* module);
        IssoModuleRequest makeMultiSensorPollingAddrsRequest(const QHostAddress &ip, quint16 port,
                                                             const QByteArray& multiSensorIds);
        QByteArray moduleMultiSensorAddrs(IssoModule* module);


        void showActivationProgressDialog(int stepCount);

        // DEBUG
        /**
         * @brief IssoExchangeManager::testHandleActivationRequests проверить наличие запросов на активацию МД и обработать их
         * @param sender
         * @return
         */

        bool testHandleActivationRequests(IssoRequestSender& sender);


        // работа с регистрами:
        //
        // одиночные input-регистры
        void readMsInputRegisterSingle(int moduleId, quint8 multiSensorId, quint8 regAddress);
        // одиночные holding-регистры
        void readMsHoldRegisterSingle(int moduleId, quint8 multiSensorId, quint8 regAddress);
        void writeMsHoldRegisterSingle(int moduleId, quint8 multiSensorId,
                                       quint8 regAddress, quint16 regValue);
        // блоки holding-регистров
        void readMsHoldRegisterBlock(int moduleId, quint8 multiSensorId,
                                     quint8 regAddress, quint8 regCount);
        void writeMsHoldRegisterBlock(int moduleId, quint8 multiSensorId,
                                      quint8 regAddress, const QByteArray& regArray,
                                      quint16 romValue = IssoRomCmdValue::NOTHING);


    public:
        IssoExchangeManager(const QHostAddress& ip, quint16 bindPort,
                            quint16 alarmPort, QObject* parent = nullptr);
        ~IssoExchangeManager();

        void startExchanging(bool polling);
        void stopExchanging();

        void requestAllMVSG(int moduleId); // new CHA 2020-09-17

        void turnRelay(int moduleId, IssoParamId relayId, bool state);
        void turnRelayAsync(int moduleId, IssoParamId relayId, bool state);
        void turnRelayBroadcast(IssoParamId relayId, bool state);
        void turnRelayBroadcastAsync(IssoParamId relayId, bool state);

        // список пар [moduleId : relayId]
        void turnRelaysByScenario(IssoScenarioInfo scenarioInfo,
                                  const QList<QPair<int, IssoParamId>>& moduleToRelayPairs,
                                  bool state);
        void addTerminalRequest(IssoScenarioInfo scenarioInfo);
        void stopScenario(const QString& scenarioName);


        void resetMsState(int moduleId, quint8 multiSensorId = MULTISENSOR_ID_BROADCAST);
        void writeMsThresholds(int moduleId, quint8 multiSensorId);
        void readMsThresholds(int moduleId, quint8 multiSensorId);

        void shortBeep(int moduleId, quint8 multiSensorId);
        void startCalibration(int moduleId, quint8 multiSensorId);


        void requestMsRawInfo(int moduleId, quint8 multiSensorId);
        void checkMsValid(int moduleId, const QList<quint8> &msIds);


//        void setModules(const QList<IssoModule *> &modules);

        IssoObject *object() const;
        void setObject(IssoObject *object);

        /**
         * @brief IssoExchangeManager::handleActivationRequests обработать запросы на активацию мультидатчиков
         * @param sender
         * @return
         */
        bool handleActivationRequests(IssoRequestSender& sender);
        /**
         * @brief IssoExchangeManager::handleUserRequests обработать очередь запросов пользователя
         * @param sender
         * @return
         */
        bool handleUserRequests(IssoRequestSender& sender);
        /**
         * @brief IssoExchangeManager::handleScenarioRequests обработать запросы по сценарию
         * @param sender
         * @return
         */
        bool handleScenarioRequests(IssoRequestSender& sender);

        void sleepIfNeed(QSharedPointer<IssoModuleReply> replyPtr);

    signals:
        void requestSent(const IssoModuleRequest&);
        void replyReceived(const IssoModuleReply&, bool);
        void replyDataReceived(const QHostAddress&, quint16, const QByteArray&);
        void replyTimedOut(const IssoModuleRequest&);
        void alarmReceived(const IssoModuleReply&);
        void alarmDataReceived(const QHostAddress&, quint16, const QByteArray&);

        void scenarioStopped(IssoScenarioInfo info);

        void multiSensorActivated(const QHostAddress& moduleIp, quint8 msId);

        void progressCurrent(int count);
        void progressInfo(const QString& info);
        void progressFinished();

    private slots:
        // режим админа
        void runExchangeLoop();
        // режим оператора
        void runExchangeLoopWithPolling();
        void processExchangeLoopFinished();

    public slots:
        void readDescriptor(const int moduleId);
        void writeDescriptor(const int moduleId, const IssoModuleDescriptor& desc);
        void requestModuleInfo(int moduleId);
        void updateModuleConfig(int moduleId);

        void startMsActivation(int moduleId, const QList<quint8>& msIds);
        void stopMsActivation();

};

#endif // ISSOEXCHANGEMANAGER_H
