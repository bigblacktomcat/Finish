#ifndef ISSOMODULECOMMAND_H
#define ISSOMODULECOMMAND_H

#include <QByteArray>
#include <QMap>
#include <QVariant>
#include <QDataStream>
#include <QtEndian>
#include <QDebug>
#include "issocommondata.h"
#include "issoparam.h"
#include "issoparambuffer.h"
#include "issomoduledescriptor.h"


/*!
 * \brief   Класс, представляющий команду, вложенную в запрос / ответ,
 *          для взаимодействия с модулем МАСО. Класс формирует пару - номер команды
 *          и буфер с параметрами
 */
class IssoModuleCommand
{
    private:
        IssoCmdId m_id;                         // id команды
        IssoParamBuffer m_paramBuffer;

    public:
        IssoModuleCommand();
        /// \brief  Cоздать запрос без параметров
        IssoModuleCommand(IssoCmdId cmdId);
        /// \brief  Cоздать запрос с набором параметров
        IssoModuleCommand(IssoCmdId cmdId, IssoParamBuffer params);
        /// \brief  Cоздать команду из массива данных
        /// \param      data    массив данных (id команды и параметры)
        IssoModuleCommand(const QByteArray& data);
        /// \brief  Конструктор копирования
        IssoModuleCommand(const IssoModuleCommand& other);


        /// \brief  Получить id команды
        IssoCmdId id() const;
        /// \brief  Получить буфер параметров
        IssoParamBuffer paramBuffer() const;
        /// \brief  Получить массив данных команды
        const QByteArray bytes() const;
        /// \brief  Получить количество параметров команды
        int paramCount() const;
        /// \brief  Получить параметр по его id
        IssoParam* paramById(const IssoParamId& paramId);
        /// \brief  Получить параметр по индексу
        QSharedPointer<IssoParam> paramPtrByIndex(int index);
        /// \brief  Определить наличие параметра команды
        bool paramExists(const IssoParamId& paramId) const;
};


#endif // ISSOMODULECOMMAND_H
