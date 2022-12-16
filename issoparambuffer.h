#ifndef ISSOPARAMBUFFER_H
#define ISSOPARAMBUFFER_H

#include "issoparam.h"
#include "issocommondata.h"
#include <QSharedPointer>


class IssoParamBuffer
{
    private:
    /// @variable m_params представляет из себя пару - тип параметра и
    /// параметр команды
        QMultiMap<IssoParamId, QSharedPointer<IssoParam>> m_params;

        /// \brief  Преобразовать входной буфер в карту параметров
        void parseBytes(const QByteArray& bytes);

    public:
        /// \brief  Конструктор по умолчанию
        IssoParamBuffer();
        /// \brief  Конструктор, формирующий карту параметров из массива данных
        explicit IssoParamBuffer(const QByteArray& buffer);
        /// \brief  Конструктор, принимающий готовую карту параметров
        explicit IssoParamBuffer(const QMultiMap<IssoParamId, QSharedPointer<IssoParam>>& params);
        IssoParamBuffer(const IssoParamBuffer& other);

        /// \brief  Получить карту параметров
        QMultiMap<IssoParamId, QSharedPointer<IssoParam> > params() const;
        /// \brief  Задать карту параметров
        void setParams(const QMultiMap<IssoParamId, QSharedPointer<IssoParam>> &params);
        /// \brief  Очистить карту параметров
        void clearParams();

        /// \brief  Добавить параметр, заместив существуюший
        QSharedPointer<IssoParam> insertParam(IssoParam *param);

        void insertParamPtr(QSharedPointer<IssoParam> paramPtr);

        /// \brief  Удалить все параметры, соответствующие ID
        void removeParam(IssoParamId id);

        int paramCount() const;
        bool isEmpty() const;
        /// \brief  Получить параметр по ID
        IssoParam *paramById(const IssoParamId& paramId);
        /// \brief  Получить список параметров по ID
        QList<QSharedPointer<IssoParam>> paramsById(IssoParamId paramId);
        IssoParam *paramByIndex(int index) const;

        QSharedPointer<IssoParam> paramPtrByIndex(int index) const;

        bool paramExists(const IssoParamId& paramId) const;

        /// \brief  Получить массив данных из карты параметров
        QByteArray bytes() const;

        IssoModuleState getResultState();

    signals:

    public slots:
};

#endif // ISSOPARAMBUFFER_H
