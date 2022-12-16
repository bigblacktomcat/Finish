#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include "issocore.h"
#include "issodatabase.h"
#include "issoaccountmanager.h"
#include "issosoundplayer.h"
#include "testingwidget.h"



namespace Ui {
    class MainForm;
}

class MainWindow : public QMainWindow
{
        Q_OBJECT
    private:
        Ui::MainForm* ui;

        const int PING_INTERVAL = 2500;

        IssoCore m_core;
        IssoDataBase* m_dataBase;
        IssoAppType m_appType;
        IssoAccount m_account;
        QTimer m_pingTimer;
        IssoSoundPlayer m_soundPlayer;
        //
        QTextEdit* m_debugInfo;
        //
        TestingWidget* m_testWidget;


        /// \brief  Подготовить к работе
        void init();
        /// \brief  Подготовить к завершению работы
        void uninit();

        void initEventHandlers();
        void uninitEventHandlers();

        void setDataBase(IssoDataBase *db);

        void setSuperUserMode(bool superUser);

        void registerMetaTypes();

        QList<IssoEventData> getModuleDBEvents(IssoModule* module,
                                                 const QDateTime& dateTime);

        void logChangedSensorStates(const QDateTime &dateTime, IssoModule *module);

        void sendEventToServer(const QDateTime &dateTime, IssoModule* module,
                               const QList<IssoEventData> &events = QList<IssoEventData>());
        void sendPhotoToServer(const QDateTime &dateTime, const QString &moduleName,
                               const QString &sensorName, int cameraId, int cameraIp,
                               const QString &base64Data);
        void writeEventToDB(const QDateTime &dateTime, IssoModule* module,
                            const QList<IssoEventData> &events = QList<IssoEventData>());

//        /// \brief  Добавить запись в журнал событий
//        /// \param  moduleName  имя модуля - источника события
//        /// \param  eventMsg    текст события
//        /// \param  alarmed     тревожность записи (влияет на цвет текста)
//        void sendEventToServer(const QDateTime &dateTime,
//                               const QString& moduleName,
//                               const QString& sensorName,
//                               const QString &sensorState,
//                               quint32 extValue, const QString &location);

        void runScenarios(IssoModule* module);

        void handleChangedMsParam(const QDateTime& dateTime, IssoModule* module,
                                  QSharedPointer<IssoParam> param);

        QString findMsLocation(const QString& moduleCfgName, int msId);

        // DEBUG
        void setTestEventHandlers();
        void moduleCfgTest();
        void objectCfgTest();
        //
        void initDebugInfo();
        void addDebugMsg(const QString& msg);

        void initTestingWidget();

    protected:
        virtual void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;
        virtual void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;

    public:
        MainWindow(IssoAppType appType, IssoAccount account, IssoGuiMode guiMode,
                   IssoObject *guardedObject, IssoDataBase *db,
                   QWidget *parent = nullptr);
        virtual ~MainWindow();

        int objectId();

        IssoObject* guardedObject();
        void setGuardedObject(IssoObject* guardedObject);

        IssoAppType getAppType() const;
        void setAppType(const IssoAppType &appType);

        void updateEventTableView();
        void updateModuleParams(const QString& moduleName,
                                const IssoParamBuffer& paramBuffer);

        void setObjectOnline();
        void setObjectOffline();
        void setModuleOnline(const QString& moduleName);
        void setModuleOffline(const QString& moduleName);

        void setEventFilter(const QDate &date,
                            const QTime &timeFrom, const QTime &timeTo,
                            const QString &eventType);
        void setEventFilterEnabled(bool enabled);

    private slots:
        //
        //  События контроллера модулей:
        //
        /// \brief  Обработчик события отправки запроса
        void processRequestSent(const IssoModuleRequest& request);
        void processReplyReceived(const IssoModuleReply& reply, bool ok);
        void processReplyTimedOut(const IssoModuleRequest& request);
        void processReplyDataReceived(const QHostAddress& ip, quint16 port, const QByteArray& data);
        void processAlarmDataReceived(const QHostAddress& ip, quint16 port, const QByteArray& data);

        /// \brief  Обработчик события изменения набора тревожных параметров (датчиков) модуля
        void processSensorStatesChanged(const QString& moduleName);
        void processModuleStateChanged(const QString& moduleName,
                                       IssoModuleState moduleState);
        void processMultiSensorRawDataReceived(const QString &moduleName,
                                            quint8 multiSensorId, IssoState multiSensorState,
                                            const QMap<IssoDigitalSensorId, IssoState>& sensorStates,
                                            const QMap<IssoDigitalSensorId, quint16>& sensorValues);

        /// \brief  Обработчик включения / выключения камеры из контекстного меню
        void processCameraTurned(int cameraId, bool turnOn);
        /// \brief  Отправить конфиг объекта на сервер
        void sendObjectConfigToServer();
        //
        //
        /// \brief  Обработчик изменения текущей вкладки (в режиме Админа)
        void processTabChanged(int index);
        //
        //  События удаленного сервера
        //
        /// \brief  Обработчик успешной регистрации события на удаленном сервере
        void processRemoteLogCompleted(const QString& replyMsg);
        /// \brief  Обработчик ошибки регистрации события на удаленном сервере
        void processRemoteLogFailed(const QString &errorMsg);
        //
        //  События видеопотока
        //
        /// \brief  Обработчик ошибки открытия видеопотока
        void processOpenVideoStreamFailed(const QHostAddress& ip);
        /// \brief  Обработчик ошибки чтения видеопотока
        void processReadVideoStreamFailed(const QHostAddress& ip);
        //
        //  Работа с БД
        //
        /// \brief  Записать комментарий к тревожному событию
        void writeComment(int eventId, const QString& comment);

        void processSelectEventsRequested(QDate date, QTime timeFrom, QTime timeTo,
                                          const QString eventType, int objectId, bool filtered);
        void sendPingToServer();

        void processCameraSnapshotReady(const QDateTime& dateTime, const QString& moduleName,
                                        int multiSensorId, int cameraId, int cameraIp,
                                        const QImage &frame);

    signals:
        void windowClosed();

};

#endif // MAINWINDOW_H
