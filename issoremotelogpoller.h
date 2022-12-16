/**
@brief IssoRemoteLogPoller   Класс отвечающий за удаленный опрос объектов
*/

#ifndef ISSOREMOTELOGPOLLER_H
#define ISSOREMOTELOGPOLLER_H

#include "issoremotelogger.h"
#include "issodatabase.h"
#include <QObject>
#include <QTimer>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>


class IssoRemoteLogPoller : public QObject
{
        Q_OBJECT
    private:
        IssoRemoteLogger m_remoteLogger;
        QTimer m_timer;
        IssoAppType m_appType;

        int m_lastEventId = -1;

        QSettings m_settings;

        void setEventHandlers();

        QList<IssoEventData> parseLogReply(const QString& reply);
        QList<IssoEventData> parseEventObject(const QJsonObject& eventObj/*QString msg*/);

        void parseCommonInfo(const QJsonObject &eventObj, IssoEventData& event);
        void parseModuleInfo(const QJsonObject &eventObj, IssoEventData& event);
        QList<IssoEventData> parseSensorsInfo(const QJsonObject &eventObj, const IssoEventData &event);
        void parsePhotoInfo(const QJsonObject &eventObj, IssoEventData& event);

    public:
        explicit IssoRemoteLogPoller(IssoAppType appType = IssoAppType::GSC,
                                     QObject *parent = nullptr);
        ~IssoRemoteLogPoller();

        void start(int msec);
        void stop();

        IssoAppType appType() const;
        void setAppType(const IssoAppType &appType);

    signals:
        void sensorEventsReceived(const QList<IssoEventData>&);
        void pingEventsReceived(const QList<IssoEventData>&);
        void photoEventsReceived(const QList<IssoEventData>&);
        void remoteRequestFailed(const QString& errorMsg);

    private slots:
        void processLogRequestCompleted(const QString& replyMsg);
        void processLogRequestFailed(const QString& errorMsg);
        void requestLogUpdates();
};

#endif // ISSOREMOTELOGPOLLER_H
