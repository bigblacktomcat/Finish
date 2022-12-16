#ifndef ISSOMODULEDESCRIPTOR_H
#define ISSOMODULEDESCRIPTOR_H

#include <QObject>
/**
 * @brief The IssoModuleDescriptor class
 * содержит описание модуля IP, MAC, port, serverIP, serverPort, Phone и зачем-то контрольную сумму
 */

class IssoModuleDescriptor
{
    private:
        const int MAC_SIZE = 6;
        const int PHONE_SIZE = 12;

        quint32 m_ip;
        QByteArray m_mac;
        quint16 m_port;
        quint32 m_serverIp;
        quint16 m_serverPort;
        QByteArray m_phone;
        bool m_phoneEnabled;
        quint8 m_crc;

    public:
        static const int LENGTH = 32;

        IssoModuleDescriptor();
        IssoModuleDescriptor(quint32 ip, const QByteArray& mac, quint16 port,
                             quint32 serverIp, quint16 serverPort,
                             const QByteArray &phone, bool phoneEnabled, quint8 crc);
        IssoModuleDescriptor(const IssoModuleDescriptor& other);
        ~IssoModuleDescriptor();

        static IssoModuleDescriptor* fromBytes(const QByteArray &bytes);
        QByteArray bytes() const;

        quint32 ip() const;
        QByteArray mac() const;
        quint16 port() const;
        quint32 serverIp() const;
        quint16 serverPort() const;
        QByteArray phone() const;
        bool phoneEnabled() const;
        quint8 crc() const;

        QString ipString() const;
        QString serverIpString() const;
        QString phoneString() const;
        QString macString() const;

        static QString macBytesToString(const QByteArray& bytes);
        static QByteArray macStringToBytes(const QString& macStr);

        static QString phoneBytesToString(const QByteArray& bytes);
        static QByteArray phoneStringToBytes(const QString& phoneStr);
};

Q_DECLARE_METATYPE(IssoModuleDescriptor)

#endif // ISSOMODULEDESCRIPTOR_H
