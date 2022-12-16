#include "issoobjectcard.h"


IssoObjectCard::IssoObjectCard() : IssoObjectCard(0, "", "", "", "", "", "")
{
}


IssoObjectCard::IssoObjectCard(int id, const QString &city, const QString &region,
                               const QString &street, const QString &building,
                               const QString &contactName, const QString &phone)
    : m_id(id),
      m_city(city),
//      m_date(date),
//      m_address(address),
      m_region(region),
      m_street(street),
      m_building(building),
      //
      m_contactName(contactName),
      m_phone(phone)
{
}


int IssoObjectCard::id() const
{
    return m_id;
}


void IssoObjectCard::setId(int id)
{
    m_id = id;
}


QString IssoObjectCard::city() const
{
    return m_city;
}


void IssoObjectCard::setCity(const QString &city)
{
    m_city = city;
}


//QDate IssoObjectCard::date() const
//{
//    return m_date;
//}


//void IssoObjectCard::setDate(const QDate &date)
//{
//    m_date = date;
//}


QString IssoObjectCard::region() const
{
    return m_region;
}


void IssoObjectCard::setRegion(const QString &region)
{
    m_region = region;
}


QString IssoObjectCard::street() const
{
    return m_street;
}


void IssoObjectCard::setStreet(const QString &street)
{
    m_street = street;
}


QString IssoObjectCard::building() const
{
    return m_building;
}


void IssoObjectCard::setBuilding(const QString &building)
{
    m_building = building;
}


QString IssoObjectCard::address() const
{
//    return m_address;

    // [регион, город, улица, строение]
    return QString("%1, %2, %3, %4")
                        .arg(m_region)
                        .arg(m_city)
                        .arg(m_street)
                        .arg(m_building);
}


//void IssoObjectCard::setAddress(const QString &address)
//{
//    m_address = address;
//}


QString IssoObjectCard::contactName() const
{
    return m_contactName;
}


void IssoObjectCard::setContactName(const QString &contactName)
{
    m_contactName = contactName;
}


QString IssoObjectCard::phone() const
{
    return m_phone;
}


void IssoObjectCard::setPhone(const QString &phone)
{
    m_phone = phone;
}


QString IssoObjectCard::displayName() const
{
    return QString("%1-%2").arg(m_id, 4, 10, QLatin1Char('0'))
                           .arg(m_city);
}


void IssoObjectCard::loadFromStream(QDataStream &stream)
{
    stream >> m_id;

    stream >> m_city;

    stream >> m_region;
    stream >> m_street;
    stream >> m_building;
//    stream >> m_date;
//    stream >> m_name;
//    stream >> m_address;
    stream >> m_contactName;
    stream >> m_phone;
}


void IssoObjectCard::saveToStream(QDataStream &stream)
{
    stream << m_id;

    stream << m_city;

    stream << m_region;
    stream << m_street;
    stream << m_building;
//    stream << m_date;
//    stream << m_name;
//    stream << m_address;
    stream << m_contactName;
    stream << m_phone;
}
