#include "issomodulereply.h"
#include <QDataStream>
#include <QtEndian>


const QString IssoModuleReply::STX = "ISSA";


quint8 IssoModuleReply::status() const
{
    return m_status;
}

IssoModuleReply::IssoModuleReply()
    : IssoModulePacket(),
      m_status(0)
{
}


IssoModuleReply::IssoModuleReply(QHostAddress addr, quint16 port, quint16 number,
                                 const QByteArray &data, quint8 status, quint16 crc)
    : IssoModulePacket(addr, port, number, IssoModuleCommand(data)),
      m_status(status)
{
    // CRC16
    m_crc = crc;
}


IssoModuleReply::IssoModuleReply(const IssoModuleReply &other)
    : IssoModulePacket(other),
      m_status(other.m_status)
{
    this->m_crc = other.crc();
}


IssoModuleReply::~IssoModuleReply()
{
//    qDebug() << "~IssoModuleReply";
}


bool IssoModuleReply::isCorrect() const
{
    return (m_status == STATUS_OK);
}


QByteArray IssoModuleReply::bytes() const
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
    // статус
    //
    stream.writeRawData((char*)&m_status, sizeof(m_status));
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


IssoModuleReply *IssoModuleReply::fromBytes(QHostAddress addr, quint16 port,
                                            const QByteArray &bytes)
{
    QString stx;            // преамбула
    quint16 number;         // номер пакета
    quint16 packSize;       // длина пакета
    QByteArray cmdData;     // ответная команда
    quint8 status;          // статус ответа
    quint16 crc;            // контрольная сумма
    //
    // проверить длину пакета
    if (bytes.size() < IssoModuleReply::MIN_SIZE)
        return nullptr;
    //
    // сделать изменяемую копию массива
    QByteArray ba = QByteArray(bytes);
    // поток данных
    QDataStream stream(&ba, QIODevice::ReadOnly);
    // перейти в начало массива
    stream.device()->reset();
    //
    // разобрать пакет:
    //
    // преамбула
    //
    // создать массив размера STX
    QByteArray stxArr(IssoModuleReply::STX.length(), '0');
    // считать из потока в массив
    stream.readRawData(stxArr.data(), stxArr.length());
    // сформировать строку из массива
    stx = QString(stxArr);
    //
    // номер пакета
    //
    // считать из потока
    stream.readRawData((char*)&number, sizeof(number));
    // конвертировать из BigEndian в ByteOrder платформы
    number = qFromBigEndian(number);
    //
    // длина пакета
    //
    // считать из потока
    stream.readRawData((char*)&packSize, sizeof(packSize));
    // конвертировать из BigEndian в ByteOrder платформы
    packSize = qFromBigEndian(packSize);

    // проверить заголовок и длину пакета
    if ((stx != IssoModuleReply::STX) || (bytes.size() < packSize))
        return nullptr;
    //
    // ответная команда
    //
    // рассчитать позицию буфера данных
    int pos = stx.length() + sizeof(number) + sizeof(packSize);
    //
    // обрезать буфер до значения packSize
//    cmdData = bytes.left(packSize);
    cmdData = bytes.left(packSize + pos - sizeof(m_status));
    // обрезать в начале буфера поля Преамбула, Номер пакета, длина пакета,
    // в конце - поля Статус и CRC
//    cmdData = cmdData.mid(pos).chopped(sizeof(m_status) + sizeof(crc));
    cmdData = cmdData.mid(pos);
    //
    // сместить позицию в потоке
//    stream.skipRawData(cmdData.length());
    stream.skipRawData(pos + cmdData.length());
    //
    // статус
    //
    stream.readRawData((char*)&status, sizeof(status));
    //
    // CRC16
    //
    // считать из потока
    stream.readRawData((char*)&crc, sizeof(crc));
    // конвертировать из BigEndian в ByteOrder платформы
    crc = qFromBigEndian(crc);

//    qDebug() << stx << endl
//             << number << endl
//             << packSize << endl
//             << cmdData.toHex(' ').toUpper() << endl
//             << crc << endl;

    // создать ответ из полученных данных
    return new IssoModuleReply(addr, port, number, cmdData, status, crc);
}

