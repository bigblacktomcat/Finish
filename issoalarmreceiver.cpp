#include "issoalarmreceiver.h"


IssoAlarmReceiver::IssoAlarmReceiver(quint16 port, QObject *parent) : QObject(parent)
{
    mp_sock = new QUdpSocket(this);
    mp_sock->bind(port, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
}


void IssoAlarmReceiver::startReceiving()
{
    connect(mp_sock, SIGNAL(readyRead()),
            this,    SLOT(processReadyRead()));
}


void IssoAlarmReceiver::stopReceiving()
{
    disconnect(mp_sock,    0, this, 0);
}


IssoAlarmReceiver::~IssoAlarmReceiver()
{
    stopReceiving();
    closeSocket();
    //
    qDebug() << "~IssoAlarmReceiver";
}


void IssoAlarmReceiver::closeSocket()
{
    if (mp_sock->isOpen())
        mp_sock->close();
}


void IssoAlarmReceiver::processReadyRead()
{
    QByteArray buffer;      // входящая датаграмма
    QHostAddress addr;      // ip-адрес источника
    quint16 port;           // порт источника
    // задать размер буфера
    buffer.resize(mp_sock->pendingDatagramSize());
    // считать датаграмму в буфер
    qint64 recvSize = mp_sock->readDatagram(buffer.data(), buffer.size(),
                                            &addr, &port);
    if (recvSize == -1)
        return;

//    qDebug() << "Alarm received: data size = " << buffer.size()
//             << "   Data: " << buffer.toHex(' ').toUpper() << endl;

    // известить о получении тревожных данных
    emit dataReceived(addr, port, buffer);

    // сформировать ответны пакет из полученных данных
    IssoModuleReply* alarm = IssoModuleReply::fromBytes(addr, port, buffer);
    // если получен корректный пакет, инициировать событие
    if (alarm)
    {
        emit alarmReceived(*alarm);
        delete alarm;
    }
}

