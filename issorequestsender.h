/**
\brief IssoRequestSender
Реализация приема/передачи информационных пакетов по UDP
\date  2020-08-19
**/
#ifndef ISSOREQUESTSENDER_H
#define ISSOREQUESTSENDER_H

#include <QObject>
#include <QUdpSocket>
#include <QThread>
#include "issomodulerequest.h"
#include "issomodulereply.h"

/**
 * The IssoRequestSender class
 * реализует посылку / чтение UDP пакетов по заданным адресам
 */
class IssoRequestSender : public QObject
{
        Q_OBJECT
    private:
        QUdpSocket* mp_sock;

        void init();
        void uninit();

        bool readSocket(QByteArray& buffer, QHostAddress *addr, quint16 *port);
        bool waitForPendingData(int timeout);

    public:
        IssoRequestSender(const QHostAddress& ip, quint16 port, QObject* parent = nullptr);
        ~IssoRequestSender();

        IssoModuleReply *sendRequestRepeatedly(const IssoModuleRequest& request,
                                   bool &ok, quint8 repeatCount);
        /**
         * @brief sendRequest послать запрос
         * @param request
         * @param ok
         * @return
         */
        IssoModuleReply *sendRequest(const IssoModuleRequest& request, bool &ok);

        /**
         * @brief receiveReply получить ответ
         * @return
         */
        IssoModuleReply* receiveReply();

        void closeSocket();

    signals:
        void requestSent(const IssoModuleRequest& request);
        void dataReceived(const QHostAddress& ip, quint16 port, const QByteArray& data);

    public slots:

};

#endif // ISSOREQUESTSENDER_H
