#ifndef ISSOMODULEREPLY_H
#define ISSOMODULEREPLY_H

#include <QObject>
#include "issomodulepacket.h"
#include "issocommondata.h"
#include "issomodulecommand.h"
#include "issomodulerequest.h"


/*!
 * \brief   Пакет ответа от контроллера, содержащий вложенную команду
 *          взаимодействия с модулем МАСО.
 */

class IssoModuleReply : public IssoModulePacket
{
    private:
        static const int MIN_SIZE = 15;
        static const quint8 STATUS_OK = 0xAA;

        quint8 m_status;

    public:
        static const QString STX;               // преамбула ответа (начало пакета)

        /// \brief  Конструктор по умолчанию
        IssoModuleReply();
        /// \brief  Конструктор ответа
        /// \param      addr    IP-адрес модуля-отправителя
        /// \param      port    порт модуля-отправителя
        /// \param      number  номер пакета
        /// \param      data    данные вложенной ответной команды
        /// \param      crc     контрольная сумма
        IssoModuleReply(QHostAddress addr, quint16 port, quint16 number,
                        const QByteArray& data, quint8 status, quint16 crc);
        /// \brief  Конструктор копирования
        IssoModuleReply(const IssoModuleReply& other);

        virtual ~IssoModuleReply();

        /// \brief  Получить статус ответа
        quint8 status() const;
        /// \brief  Определить корректность ответа
        bool isCorrect() const;
        /// \brief      Сформировать и вернуть массив данных ответного пакета
        /// \details    Переопределенный метод.
        virtual QByteArray bytes() const;

        /// \brief  Сформировать ответный пакет из массива данных
        static IssoModuleReply* fromBytes(QHostAddress addr, quint16 port,
                                          const QByteArray& bytes);
};

#endif // ISSOMODULEREPLY_H
