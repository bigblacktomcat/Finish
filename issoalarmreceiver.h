#ifndef ISSOALARMRECEIVER_H
#define ISSOALARMRECEIVER_H

#include <QObject>
#include <QUdpSocket>
#include "issomodulereply.h"

/**
 * @brief The IssoAlarmReceiver class получение сообщений об ошибках
 */
class IssoAlarmReceiver : public QObject
{
        Q_OBJECT
    private:
        QUdpSocket* mp_sock;

    public:
        explicit IssoAlarmReceiver(quint16 port, QObject *parent = nullptr);
        ~IssoAlarmReceiver();

        void startReceiving();
        void stopReceiving();
        void closeSocket();

    signals:
        void alarmReceived(IssoModuleReply);
        void dataReceived(const QHostAddress&, quint16, const QByteArray&);

    private slots:
        void processReadyRead();
};

#endif // ISSOALARMRECEIVER_H
