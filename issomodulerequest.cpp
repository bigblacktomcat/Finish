#include <QDataStream>
#include <QDebug>
#include <QtEndian>
#include "issomodulerequest.h"


const QString IssoModuleRequest::STX = "ISSR";


IssoModuleRequest::IssoModuleRequest()
    : IssoModulePacket()
{
    m_crc = 0;
}


IssoModuleRequest::IssoModuleRequest(QHostAddress addr, quint16 port,
                                     quint16 number, IssoCmdId cmdId)
    : IssoModulePacket(addr, port, number, IssoModuleCommand(cmdId))
{
    m_crc = calcCrc16();
}


IssoModuleRequest::IssoModuleRequest(QHostAddress addr, quint16 port,
                                     quint16 number, IssoCmdId cmdId,
                                     const IssoParamBuffer& params)
    : IssoModulePacket(addr, port, number, IssoModuleCommand(cmdId, params))
{
    m_crc = calcCrc16();
}


IssoModuleRequest::IssoModuleRequest(const IssoModuleRequest &other)
    : IssoModulePacket(other)
{
    this->m_crc = other.crc();
}


IssoModuleRequest::~IssoModuleRequest()
{
//    qDebug() << "~IssoModuleRequest";
}


quint16 IssoModuleRequest::calcCrc16()
{
    return (quint16)0;
}


QByteArray IssoModuleRequest::bytes() const
{
    QByteArray ba;
    QDataStream stream(&ba, QIODevice::ReadWrite);
    //
    // преамбула
    //
    stream.writeRawData(STX.toLatin1().data(), STX.length());
    //
    // номер команды
    //
    // конвертировать в BigEndian
    quint16 num = qToBigEndian(m_number);
    // записать в поток
    stream.writeRawData((char*)&num, sizeof(num));
    //
    // длина пакета
    //
    quint16 packLength;
    // получить данные команды
    QByteArray commandData = m_command.bytes();
//    QByteArray commandData = m_command->bytes();
    // рассчитать длину пакета
    packLength = STX.length()
                    + sizeof(m_number)
                    + sizeof(packLength)
                    + commandData.length()
                    + sizeof(m_crc);
    // конвертировать в BigEndian
    packLength = qToBigEndian(packLength);
    // записать в поток
    stream.writeRawData((char*)&packLength, sizeof(packLength));
    //
    // данные команды
    //
    stream.writeRawData(commandData.data(), commandData.length());
    //
    // crc
    //
    quint16 crc = qToBigEndian(m_crc);
    // записать в поток
    stream.writeRawData((char*)&crc, sizeof(crc));
    //
    // вернуть заполненный массив
    return ba;
}
