#ifndef ISSOMODULEREQUEST_H
#define ISSOMODULEREQUEST_H

#include "issocommondata.h"
#include "issomodulecommand.h"
#include "issomodulepacket.h"
#include <QMetaType>


/*!
 * \brief   Пакет запроса контроллеру, содержащий вложенную команду
 *          взаимодействия с модулем МАСО.
 */
class IssoModuleRequest : public IssoModulePacket
{
    private:
        quint16 calcCrc16();

    public:
        static const QString STX;       // преамбула запроса (начало пакета)

        /// \brief  Конструктор по умолчанию
        IssoModuleRequest();
        /// \brief  Конструктор запроса без параметров
        /// \param      addr    IP-адрес целевого модуля
        /// \param      port    порт целевого модуля
        /// \param      number  номер пакета
        /// \param      cmdId   id команды запроса
        IssoModuleRequest(QHostAddress addr, quint16 port,
                          quint16 number, IssoCmdId cmdId);
        /// \brief  Конструктор запроса с набором параметров
        /// \param      addr    IP-адрес целевого модуля
        /// \param      port    порт целевого модуля
        /// \param      number  номер пакета
        /// \param      cmdId   id команды запроса
        /// \param      params  набор параметров команды
        IssoModuleRequest(QHostAddress addr, quint16 port, quint16 number,
                          IssoCmdId cmdId, const IssoParamBuffer &params);
        /// \brief  Конструктор копирования запроса
        IssoModuleRequest(const IssoModuleRequest& other);
        virtual ~IssoModuleRequest();

        /// \brief  Сформировать и вернуть массив данных пакета запроса
        /// \details    Переопределенный метод
        virtual QByteArray bytes() const;
};

#endif // ISSOMODULEREQUEST_H
