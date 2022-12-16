#ifndef ISSOMSUTILS_H
#define ISSOMSUTILS_H

#include <QMap>
#include "issocommondata.h"


namespace IssoMsUtils
{
    enum IssoMsType
    {
        MS_TYPE_AUTO,
        MS_TYPE_1,
        MS_TYPE_2,
        MS_TYPE_3
    };

    QDataStream &operator<<(QDataStream &out, IssoMsType type);
    QDataStream &operator>>(QDataStream &in, IssoMsType& type);


    const QMap<IssoMsType, QString> msTypes =
    {
        {MS_TYPE_AUTO, QT_TRANSLATE_NOOP("IssoMsUtils", "Авто")},
        {MS_TYPE_1,    QT_TRANSLATE_NOOP("IssoMsUtils", "Тип - 1")},
        {MS_TYPE_2,    QT_TRANSLATE_NOOP("IssoMsUtils", "Тип - 2")},
        {MS_TYPE_3,    QT_TRANSLATE_NOOP("IssoMsUtils", "Тип - 3")}
    };

    QList<IssoDigitalSensorId> channelIds(IssoMsType msType);
    int channelCount(IssoMsType msType);
}

#endif // ISSOMSUTILS_H
