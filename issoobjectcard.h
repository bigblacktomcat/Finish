#ifndef ISSOOBJECTCARD_H
#define ISSOOBJECTCARD_H

#include <QDataStream>
#include <QDate>
#include <QString>


class IssoObjectCard
{
    private:
//        QString m_name;
        int m_id;

        QString m_city;
//        QDate m_date;
//        QString m_address;
        //
        QString m_region;
        QString m_street;
        QString m_building;
        //
        QString m_contactName;
        QString m_phone;
    public:
        IssoObjectCard();
        IssoObjectCard(int id, /*const QDate &date, const QString& address,*/
                       const QString& city,  const QString& region,
                       const QString& street, const QString& building,
                       const QString& contactName, const QString& phone);

        int id() const;
        void setId(int id);

        QString city() const;
        void setCity(const QString &city);

//        QDate date() const;
//        void setDate(const QDate &date);

        QString region() const;
        void setRegion(const QString &region);

        QString street() const;
        void setStreet(const QString &street);

        QString building() const;
        void setBuilding(const QString &building);


        QString contactName() const;
        void setContactName(const QString &contactName);

        QString phone() const;
        void setPhone(const QString &phone);
        //
        // вернуть полный адрес:
        // [регион, город, улица, строение]
        QString address() const;
//        void setAddress(const QString &address);
        //
        // формат имени объекта ID-ГОРОД
        QString displayName() const;
        //
        /// \brief  Загрузить из потока
        void loadFromStream(QDataStream& stream);
        /// \brief  Сохранить в поток
        void saveToStream(QDataStream& stream);
};

#endif // ISSOOBJECTCARD_H
