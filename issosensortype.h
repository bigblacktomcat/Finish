#ifndef ISSOSENSORTYPE_H
#define ISSOSENSORTYPE_H

#include <QString>
#include <QVariant>


class IssoSensorType
{
    private:
        QString m_name;
        QVariant m_minValue;
        QVariant m_maxValue;
        bool m_fixed;

    public:
        IssoSensorType(QString name = "", QVariant minValue = 0, QVariant maxValue = 0, bool fixed = true);

        /// \brief  Загрузить данные типа датчика из потока
        void loadFromStream(QDataStream& stream);
        /// \brief  Сохранить данные типа датчика в поток
        void saveToStream(QDataStream &stream);

        bool checkBounds(QVariant value);

        QString name() const;
        QVariant minValue() const;
        QVariant maxValue() const;
        bool fixed() const;
};

#endif // ISSOSENSORTYPE_H
