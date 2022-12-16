#include "issosensortype.h"

#include <QDataStream>


QString IssoSensorType::name() const
{
    return m_name;
}


QVariant IssoSensorType::minValue() const
{
    return m_minValue;
}


QVariant IssoSensorType::maxValue() const
{
    return m_maxValue;
}


bool IssoSensorType::fixed() const
{
    return m_fixed;
}


IssoSensorType::IssoSensorType(QString name, QVariant minValue, QVariant maxValue, bool fixed)
    : m_name(name),
      m_minValue(minValue),
      m_maxValue(maxValue),
      m_fixed(fixed)
{
}


void IssoSensorType::loadFromStream(QDataStream &stream)
{
    // имя
    stream >> m_name;
    // минимальное значение
    stream >> m_minValue;
    // максимальное значение
    stream >> m_maxValue;
    // изменяемость датчика
    stream >> m_fixed;

//    // длина имени
//    int nameLen;
//    stream.readRawData((char*)&nameLen, sizeof(nameLen));
//    // имя:
//    //
//    // создать массив
//    QByteArray nameArr(nameLen, '0');
//    // считать из потока в массив
//    stream.readRawData(nameArr.data(), nameArr.length());
//    // сформировать строку из массива
//    m_name = QString(nameArr);
//    //
//    // минимальное значение
//    stream.readRawData((char*)&m_minValue, sizeof(m_minValue));
//    // максимальное значение
//    stream.readRawData((char*)&m_maxValue, sizeof(m_maxValue));
}


void IssoSensorType::saveToStream(QDataStream &stream)
{
    // имя
    stream << m_name;
    // минимальное значение
    stream << m_minValue;
    // максимальное значение
    stream << m_maxValue;
    // изменяемость датчика
    stream << m_fixed;

//    // длина имени
//    int nameLen = m_name.length();
//    stream.writeRawData((char*)&nameLen, sizeof(nameLen));
//    // имя
//    stream.writeRawData(m_name.toLatin1().data(), m_name.length());
//    // минимальное значение
//    stream.writeRawData((char*)&m_minValue, sizeof(m_minValue));
//    // максимальное значение
//    stream.writeRawData((char*)&m_maxValue, sizeof(m_maxValue));
}


bool IssoSensorType::checkBounds(QVariant value)
{
    return ((value >= m_minValue) && (value <= m_maxValue));
}
