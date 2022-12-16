/**
\brief Реализация класса IssoRequestSender.
Прием/передача информационных пакетов
\date  2020-08-19
**/
#include "issorequestsender.h"
#include <QTime>
#include <QDataStream>
#include <QtEndian>


IssoRequestSender::IssoRequestSender(const QHostAddress &ip, quint16 port, QObject *parent)
    : QObject(parent)
{
    // открыть сокет
    mp_sock = new QUdpSocket(this);
    // привязать к порту для получения ответа
//    mp_sock->bind(port, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
    mp_sock->bind(ip /*QHostAddress("192.168.1.10")*/, port, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
    // подготовить к работе
    init();
}


IssoRequestSender::~IssoRequestSender()
{
    qDebug() << "~IssoRequestSender";
    //
    // подготовить к завершению работы
    uninit();
}


IssoModuleReply* IssoRequestSender::sendRequestRepeatedly(
                                            const IssoModuleRequest &request,
                                            bool &ok, quint8 repeatCount)
{
    IssoModuleReply* reply = nullptr;
    // повторять отправку до получения корректного ответа,
    // либо пока не будет исчерпан лимит попыток
    while(repeatCount--)
    {
        // если ответ корректен, закончить отправку
        reply = sendRequest(request, ok);
        if (ok)
            break;
    }
    return reply;
}


IssoModuleReply* IssoRequestSender::sendRequest(const IssoModuleRequest &request, bool &ok)
{
//    qDebug() << "Send to socket: " << request.command().bytes().toHex(' ').toUpper() << endl;

//    qDebug() << QString("Request outgoing: %1:%2")
//                        .arg(mp_sock->localAddress().toString())
//                        .arg(mp_sock->localPort()) << endl;

    ok = false;
    // если датаграмма не отправилась, вернуть пустой указатель
    if (mp_sock->writeDatagram(request.bytes(), request.address(), request.port()) == -1)
        return nullptr;
    //
    // известить об успешной отправке запроса
    emit requestSent(request);
    // если запрос широковещательный, то не ждем ответа
    if (request.address().isEqual(QHostAddress::Broadcast,
                                  QHostAddress::ConvertV4MappedToIPv4))
        return nullptr;

    // считать ответный пакет
    IssoModuleReply* reply = receiveReply();
    // корректность ответа
    ok = (reply && (reply->number() == request.number())); //&& (reply->isCorrect()));
//    // отладка
//    if (reply)
//    {
//        qDebug() << QString("reply num = %1,   request num = %2,    reply correct = %3")
//                    .arg(reply->number()).arg(request.number()).arg(reply->isCorrect());
//    }
    // вернуть ответ
    return reply;
}


IssoModuleReply *IssoRequestSender::receiveReply()
{
    QByteArray datagram;    // входящая датаграмма
    QHostAddress addr;      // ip-адрес источника
    quint16 port;           // порт источника
    // считать датаграмму
    if (readSocket(datagram, &addr, &port))
    {
//        qDebug() << "Read from socket: data size = " << datagram.size()
//                 << "   Data: " << datagram.toHex(' ').toUpper() << endl;

        // известить о получении данных
        emit dataReceived(addr, port, datagram);

        // если удачно, вернуть ответный пакет
        return IssoModuleReply::fromBytes(addr, port, datagram);
    }
    else
        // вернуть пустой указатель
        return nullptr;
}


void IssoRequestSender::closeSocket()
{
    if (mp_sock->isOpen())
        mp_sock->close();
}


bool IssoRequestSender::waitForPendingData(int timeout)
{
    QTime time;
    time.start();
    // ждем, когда будут доступны данные для чтения
    while(!mp_sock->hasPendingDatagrams())
    {
        // если вышел таймаут ожидания данных, выход
        if (time.elapsed() >= timeout)
            return false;
    }
    return true;
}


void IssoRequestSender::init()
{
}


void IssoRequestSender::uninit()
{
    closeSocket();
}


bool IssoRequestSender::readSocket(QByteArray &buffer,
                                  QHostAddress* addr, quint16* port)
{
    // дождаться поступления данных
    if (!waitForPendingData(IssoConst::RECV_DATA_TIMEOUT))
        return false;

    // задать размер буфера
    buffer.resize(mp_sock->pendingDatagramSize());
    // считать датаграмму в буфер
    qint64 recvSize = mp_sock->readDatagram(buffer.data(), buffer.size(),
                                            addr, port);
    return (recvSize != -1);
}

