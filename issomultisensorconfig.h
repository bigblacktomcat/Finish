#ifndef ISSOMULTISENSORCONFIG_H
#define ISSOMULTISENSORCONFIG_H

#include <QMap>
#include "issodigitalsensorconfig.h"
#include "issomsutils.h"

using namespace IssoMsUtils;


class IssoMultiSensorConfig
{
    private:
        // порядковый номер
        quint8 m_id;
        // коллекция конфигураций встроенных датчиков из состава мультидатчика
        QMap<IssoDigitalSensorId, IssoDigitalSensorConfig*> m_digitalSensorConfigs;
//        QMap<IssoDigitalSensorId, IssoDigitalSensorConfig> m_digitalSensorConfigs;
        //
        // расшренные настройки Дым(о):
        // чувствительность
        quint16 m_smokeOptSens = 0;
        // яркость
        quint16 m_smokeOptBright = 0;
        //
        // ID камеры
        int m_cameraId = -1;
        // описание месторасположения
        QString m_locationDesc = "";

        // тип мультидатчика
        IssoMsType m_msType = MS_TYPE_AUTO;


        void initDigitalSensorCfgs();
        void uninitDigitalSensorCfgs();

    public:
        IssoMultiSensorConfig(quint8 id = 0);
        ~IssoMultiSensorConfig();

        QMap<IssoDigitalSensorId, IssoDigitalSensorConfig*> digitalSensorConfigs() const;
//        QMap<IssoDigitalSensorId, IssoDigitalSensorConfig> digitalSensorConfigs() const;
//        void setDigitalSensorConfigs(const QMap<IssoDigitalSensorId, IssoDigitalSensorConfig> &digitalSensorConfigs);

//        void insertDigitalSensorConfig(const IssoDigitalSensorConfig& cfg);
//        void removeDigitalSensorConfig(IssoDigitalSensorId cfgId);

        IssoDigitalSensorConfig* digitalSensorConfig(IssoDigitalSensorId id);
//        IssoDigitalSensorConfig digitalSensorConfig(IssoDigitalSensorId id);

        quint16 smokeOptSens() const;
        void setSmokeOptSens(const quint16 &smokeOptSens);
        quint16 smokeOptBright() const;
        void setSmokeOptBright(const quint16 &smokeOptBright);

        quint8 id() const;
        QString displayName() const;

        int cameraId() const;
        void setCameraId(int cameraId);

        QString locationDesc() const;
        void setLocationDesc(const QString &locationDesc);

        void toString(QTextStream& stream);



        quint16 sensorMask();
        void setSensorMask(quint16 mask);

        QByteArray getThresholdData();
        void setThresholdData(const QByteArray& bytes);

        IssoMsType getMsType() const;
        void setMsType(const IssoMsType &msType);

        int channelCount();


        friend QDataStream &operator<<(QDataStream &out, const IssoMultiSensorConfig& config);
        friend QDataStream &operator>>(QDataStream &in, IssoMultiSensorConfig& config);
};

#endif // ISSOMULTISENSORCONFIG_H
