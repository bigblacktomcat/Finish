#include "issomsutils.h"


QList<IssoDigitalSensorId> IssoMsUtils::channelIds(IssoMsUtils::IssoMsType msType)
{
    QList<IssoDigitalSensorId> channels;
    switch(msType)
    {
        case MS_TYPE_AUTO:
        {
            channels << DIGITALSENSOR_SMOKE_O
                     << DIGITALSENSOR_TEMP_A
                     << DIGITALSENSOR_TEMP_D
                     << DIGITALSENSOR_SMOKE_E
                     << DIGITALSENSOR_FLAME_STD_DEV
                     << DIGITALSENSOR_CO
                     << DIGITALSENSOR_VOC;
            break;
        }
        case MS_TYPE_1:
        {
            channels << DIGITALSENSOR_SMOKE_O
                     << DIGITALSENSOR_TEMP_A;
            break;
        }
        case MS_TYPE_2:
        {
            channels << DIGITALSENSOR_SMOKE_O
                     << DIGITALSENSOR_TEMP_A
                     << DIGITALSENSOR_TEMP_D
                     << DIGITALSENSOR_SMOKE_E;
            break;
        }
        case MS_TYPE_3:
        {
            channels << DIGITALSENSOR_SMOKE_O
                     << DIGITALSENSOR_TEMP_A
                     << DIGITALSENSOR_TEMP_D
                     << DIGITALSENSOR_SMOKE_E
                     << DIGITALSENSOR_FLAME_STD_DEV;
            break;
        }
        default:
            break;
    }
    return channels;
}


int IssoMsUtils::channelCount(IssoMsUtils::IssoMsType msType)
{
    return channelIds(msType).size();
}


QDataStream &IssoMsUtils::operator<<(QDataStream &out, IssoMsUtils::IssoMsType type)
{
    out << (qint32&)type;
    return out;
}


QDataStream &IssoMsUtils::operator>>(QDataStream &in, IssoMsUtils::IssoMsType &type)
{
    in >> (qint32&)type;
    return in;
}
