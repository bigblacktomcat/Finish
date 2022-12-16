#ifndef ISSODIGITALSENSORCONFIG_H
#define ISSODIGITALSENSORCONFIG_H

#include <QMap>
#include "issocommondata.h"


class IssoDigitalSensorConfig
{
    private:
//        static const int DEFAULT_NORMAL_VALUE = 20;
        static const int DEFAULT_WARNING_VALUE = 60;
        static const int DEFAULT_ALARM_VALUE = 80;

        // ID цифрового датчика
        IssoDigitalSensorId m_id;
        // карта соответствия состояния цифрового датчика и порогового значения
        QMap<IssoState, int> m_stateValues;
        // условие сработки тревоги
        IssoStateCondition m_condition;
        // активирован / деактивирован
        bool m_enabled;

        void setDefaultStateValues();

    public:
        IssoDigitalSensorConfig(IssoDigitalSensorId id = DIGITALSENSOR_UNDEFINED);
        IssoDigitalSensorConfig(const IssoDigitalSensorConfig& other);

        IssoDigitalSensorId id() const;
        void setId(const IssoDigitalSensorId &id);

        quint16 stateValue(IssoState state);
        void setStateValue(IssoState state, int value);

        QMap<IssoState, int> stateValues() const;
        void setStateValues(const QMap<IssoState, int> &stateValues);
        void setStateValues(int warningValue, int alarmValue);

        bool enabled() const;
        void setEnabled(bool enabled);

        IssoStateCondition condition() const;
        void setCondition(const IssoStateCondition &condition);

        void toString(QTextStream& stream);

//        QDataStream &operator<<(QDataStream &out);
//        QDataStream &operator>>(QDataStream &in);

        friend QDataStream &operator<<(QDataStream &out, const IssoDigitalSensorConfig& config);
        friend QDataStream &operator>>(QDataStream &in, IssoDigitalSensorConfig& config);

};


#endif // ISSODIGITALSENSORCONFIG_H
