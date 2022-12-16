#ifndef ISSOMODULEPACKET_H
#define ISSOMODULEPACKET_H

#include <QHostAddress>
#include <QObject>
#include "issomodulecommand.h"


/*!
 * \brief   Базовый класс пакета данных, содержащего вложенную команду
 *          взаимодействия с модулем МАСО.
 * \details класс содержит IP-адрес, порт, номер пакета, команду, CRC
 */
class IssoModulePacket
{
    protected:
        QHostAddress m_address;         // ip-адрес получателя
        quint16 m_port;                 // порт получателя
        quint16 m_number;               // номер пакета

//        IssoModuleCommand* m_command = nullptr;    // вложенная команда
        IssoModuleCommand m_command;    // вложенная команда

        quint16 m_crc = 0;              // CRC16

    public:
        IssoModulePacket();
        IssoModulePacket(QHostAddress addr, quint16 port, quint16 number,
                         IssoModuleCommand command);
        IssoModulePacket(const IssoModulePacket& other);
        virtual ~IssoModulePacket();


        const QHostAddress address() const;
        quint16 port() const;
        /// \brief  Получить номер пакета
        quint16 number() const;
        /// \brief  Получить указатель на вложенную команду
        IssoModuleCommand command() const;
        /// \brief  Получить контрольную сумму
        quint16 crc() const;

        /// \brief  Сформировать и вернуть массив данных пакета
        virtual QByteArray bytes() const = 0;
};

#endif // ISSOMODULEPACKET_H
