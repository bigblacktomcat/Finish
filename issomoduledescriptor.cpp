#include "issomoduledescriptor.h"

#include <QDataStream>
#include <QDebug>
#include <QHostAddress>
#include <QtEndian>


IssoModuleDescriptor::IssoModuleDescriptor()
    : IssoModuleDescriptor(0, QByteArray(), 0, 0, 0, QByteArray(), false, 0)
{
}


IssoModuleDescriptor::IssoModuleDescriptor(quint32 ip,
                                           const QByteArray &mac,
                                           quint16 port,
                                           quint32 serverIp,
                                           quint16 serverPort,
                                           const QByteArray &phone,
                                           bool phoneEnabled,
                                           quint8 crc)
    : m_ip(ip),
      m_mac(mac),
      m_port(port),
      m_serverIp(serverIp),
      m_serverPort(serverPort),
      m_phone(phone),
      m_phoneEnabled(phoneEnabled),
      m_crc(crc)
{
    // нужно гарантировать фиксированный размер МАС и номера телефона
    m_mac.resize(MAC_SIZE);
    m_phone.resize(PHONE_SIZE);
}


IssoModuleDescriptor::IssoModuleDescriptor(const IssoModuleDescriptor &other)
{
    m_ip = other.ip();
    m_mac = other.mac();
    m_port = other.port();
    m_serverIp = other.serverIp();
    m_serverPort = other.serverPort();
    m_phone = other.phone();
    m_phoneEnabled = other.phoneEnabled();
    m_crc = other.crc();
}


IssoModuleDescriptor::~IssoModuleDescriptor()
{
}


quint32 IssoModuleDescriptor::ip() const
{
    return m_ip;
}


QByteArray IssoModuleDescriptor::mac() const
{
    return m_mac;
}


quint16 IssoModuleDescriptor::port() const
{
    return m_port;
}


quint32 IssoModuleDescriptor::serverIp() const
{
    return m_serverIp;
}


quint16 IssoModuleDescriptor::serverPort() const
{
    return m_serverPort;
}


QByteArray IssoModuleDescriptor::phone() const
{
    return m_phone;
}


bool IssoModuleDescriptor::phoneEnabled() const
{
    return m_phoneEnabled;
}


quint8 IssoModuleDescriptor::crc() const
{
    return m_crc;
}


QString IssoModuleDescriptor::ipString() const
{
    return QHostAddress(m_ip).toString();
}


QString IssoModuleDescriptor::serverIpString() const
{
    return QHostAddress(m_serverIp).toString();
}


QString IssoModuleDescriptor::phoneString() const
{
    return phoneBytesToString(m_phone);
}


QString IssoModuleDescriptor::macString() const
{
    return macBytesToString(m_mac);
}


QString IssoModuleDescriptor::macBytesToString(const QByteArray &bytes)
{
    return QString(bytes.toHex().toUpper());
}


QByteArray IssoModuleDescriptor::macStringToBytes(const QString &macStr)
{
    QByteArray mac;
    QStringList macDigits = macStr.split(":");
    foreach (QString digit, macDigits)
        mac.append(digit.toInt(nullptr, 16));
    return mac;
}


QString IssoModuleDescriptor::phoneBytesToString(const QByteArray &bytes)
{
    QString phoneStr = "";
    foreach (char c, bytes)
        phoneStr += QString::number(c);
    return phoneStr;
}


QByteArray IssoModuleDescriptor::phoneStringToBytes(const QString &phoneStr)
{
    QByteArray ba;
    foreach (QChar c, phoneStr.toUtf8())
        ba.append(QString(c).toInt());
    return ba;
}


IssoModuleDescriptor *IssoModuleDescriptor::fromBytes(const QByteArray& bytes)
{
    // переменные
    quint32 ip;
    QByteArray mac = QByteArray(6, '0');
    quint16 port;
    quint32 serverIp;
    quint16 serverPort;
    QByteArray phone = QByteArray(12, '0');
    quint8 phoneEnabledVal;
    quint8 crc;
//    // ожидаемый размер данных
//    int desiredSize = sizeof(ip) + mac.length() + sizeof(port)
//            + sizeof(serverIp) + sizeof(serverPort) + phone.length()
//            + sizeof(phoneEnabledVal) + sizeof(crc);
    int desiredSize = LENGTH;
    // проверить размер данных
    if (bytes.length() < desiredSize)
        return nullptr;

//    // DEBUG
//    qDebug() << bytes.toHex(' ').toUpper();

    // создать поток для чтения
    QByteArray ba = QByteArray(bytes);
    QDataStream stream(&ba, QIODevice::ReadOnly);
    // считать данные из потока:
    // ip-адрес
    stream.readRawData((char*)&ip, sizeof(ip));
    ip = qFromBigEndian(ip);

//    qDebug() << "Read IP: " << QHostAddress(ip).toString() << endl;

    //
    // MAC-адрес
    stream.readRawData(mac.data(), mac.length());
    // порт
    stream.readRawData((char*)&port, sizeof(port));
//    port = qFromBigEndian(port);                              // ВРЕМЕННО!!!
    // ip-адрес сервера
    stream.readRawData((char*)&serverIp, sizeof(serverIp));
    serverIp = qFromBigEndian(serverIp);
    //
    // порт сервера
    stream.readRawData((char*)&serverPort, sizeof(serverPort));
//    serverPort = qFromBigEndian(serverPort);                  // ВРЕМЕННО!!!
    // номер телефона
    stream.readRawData(phone.data(), phone.length());
    // разрешение использовать номер телефона
    stream.readRawData((char*)&phoneEnabledVal, sizeof(phoneEnabledVal));
    bool phoneEnabled = (phoneEnabledVal != 0);
    // CRC
    stream.readRawData((char*)&crc, sizeof(crc));
    //
    // создать и вернуть объект
    return new IssoModuleDescriptor(ip, mac, port,
                                    serverIp, serverPort,
                                    phone, phoneEnabled, crc);
}


QByteArray IssoModuleDescriptor::bytes() const
{
    // переменные
    quint32 ip;
    quint16 port;
    quint32 serverIp;
    quint16 serverPort;
    quint8 phoneEnabledVal;
    // создать поток для записи
    QByteArray ba;
    QDataStream stream(&ba, QIODevice::WriteOnly);
    // записать в данные поток:
    // ip-адрес
    ip = qToBigEndian(m_ip);
    stream.writeRawData((char*)&ip, sizeof(ip));
//    stream.writeRawData((char*)&m_ip, sizeof(ip));

//    qDebug() << "Write IP: " << QHostAddress(ip).toString() << endl;

    //
    // MAC-адрес
    stream.writeRawData(m_mac.data(), m_mac.length());
    // порт
//    port = qToBigEndian(m_port);                          // ВРЕМЕННО!!!
    port = m_port;
    stream.writeRawData((char*)&port, sizeof(port));
    // ip-адрес сервера
    serverIp = qToBigEndian(m_serverIp);
    stream.writeRawData((char*)&serverIp, sizeof(serverIp));
//    stream.writeRawData((char*)&m_serverIp, sizeof(serverIp));
    //
    // порт сервера
//    serverPort = qToBigEndian(m_serverPort);              // ВРЕМЕННО!!!
    serverPort = m_serverPort;
    stream.writeRawData((char*)&serverPort, sizeof(serverPort));
    // номер телефона
    stream.writeRawData(m_phone.data(), m_phone.length());
    // разрешение использовать номер телефона
    phoneEnabledVal = (m_phoneEnabled ? 1 : 0);
    stream.writeRawData((char*)&phoneEnabledVal, sizeof(phoneEnabledVal));
    // CRC
    stream.writeRawData((char*)&m_crc, sizeof(m_crc));
    // вернуть заполненный массив
    return ba;
}

