#ifndef ISSOCHAINCONFIG_H
#define ISSOCHAINCONFIG_H

#include "issocommondata.h"
#include "issomultisensorconfig.h"


/*!
 * \brief   Родительский класс конфигурации шлейфа
 */
class IssoChainConfig
{
    protected:
        IssoChainType m_type;
        QString m_name;
        bool m_enabled;

    public:
        IssoChainConfig(IssoChainType type = SMOKE_CHAIN, const QString& name = "", bool enabled = true);
        virtual ~IssoChainConfig();

        virtual void saveToStream(QDataStream& stream);
        virtual bool loadFromStream(QDataStream& stream);

        QString name() const;
        void setName(const QString &name);

        bool enabled() const;
        void setEnabled(bool enabled);

        IssoChainType type() const;
        void setType(const IssoChainType &type);

        virtual void toString(QTextStream& stream);

        friend QDataStream &operator<<(QDataStream &out, IssoChainConfig* config);
        friend QDataStream &operator>>(QDataStream &in, IssoChainConfig* config);
};



/*!
 * \brief   Класс конфигурации аналогового шлейфа
 */
class IssoAnalogChainConfig : public IssoChainConfig
{
    private:
        int m_sensorCount;
        QMap<IssoState, IssoStateRange> m_stateRanges;

    public:
        IssoAnalogChainConfig(IssoChainType type = SMOKE_CHAIN, const QString& name = "",
                              bool enabled = true, int sensorCount = 1);
        virtual ~IssoAnalogChainConfig();

        virtual void saveToStream(QDataStream& stream);
        virtual bool loadFromStream(QDataStream& stream);

        int sensorCount() const;
        void setSensorCount(int sensorCount);

        QMap<IssoState, IssoStateRange> stateRanges() const;
        void setStateRanges(const QMap<IssoState, IssoStateRange> &stateRanges);

        void insertStateRange(IssoState state, const IssoStateRange& range);
        void removeStateRange(IssoState state);
        IssoStateRange stateRange(IssoState state);

        void initStates(const QSet<IssoState> states);

        virtual void toString(QTextStream& stream);
};



/*!
 * \brief   Класс конфигурации неадресного шлейфа типа Дым
 */
class IssoSmokeChainConfig : public IssoAnalogChainConfig
{
    public:
        IssoSmokeChainConfig(const QString& name = "", bool enabled = true, int sensorCount = 1);
        virtual ~IssoSmokeChainConfig();
};



/*!
 * \brief   Класс конфигурации неадресного шлейфа типа Температура
 */
class IssoTempChainConfig : public IssoAnalogChainConfig
{
    public:
        IssoTempChainConfig(const QString& name = "", bool enabled = true, int sensorCount = 1);
        virtual ~IssoTempChainConfig();
};



/*!
 * \brief   Класс конфигурации тревожной кнопки
 */
class IssoAlarmButtonConfig : public IssoAnalogChainConfig
{
    public:
        IssoAlarmButtonConfig(const QString& name = "", bool enabled = true, int sensorCount = 1);
        virtual ~IssoAlarmButtonConfig();
};



/*!
 * \brief   Класс конфигурации детектора движения
 */
class IssoMoveDetectorConfig : public IssoAnalogChainConfig
{
    public:
        IssoMoveDetectorConfig(const QString& name = "", bool enabled = true, int sensorCount = 1);
        virtual ~IssoMoveDetectorConfig();
};



/*!
 * \brief   Класс конфигурации адресного шлейфа
 */
class IssoAddressChainConfig : public IssoChainConfig
{
    private:
        // пары [имя конфига : конфиг мультидатчика]
        QMap<QString, IssoMultiSensorConfig*> m_multiSensorConfigs;

    public:
        static constexpr quint8 MAX_MULTISENSOR_ID = 247;

        IssoAddressChainConfig(const QString& name = "", bool enabled = true, int sensorCount = 1);
        virtual ~IssoAddressChainConfig();

        virtual void saveToStream(QDataStream& stream);
        virtual bool loadFromStream(QDataStream& stream);

        QMap<QString, IssoMultiSensorConfig*> multiSensorConfigs() const;
        void setMultiSensorConfigs(const QMap<QString, IssoMultiSensorConfig*> &configs);

        /// \brief  Получить отсортированный по ID список имен конфигов мультидатчиков
        QStringList multiSensorConfigNames() const;
        /// \brief  Получить количество конфигов мультидатчиков
        int multiSensorConfigCount() const;

        void addNewMultiSensorConfigs(int configCount);
        void insertMultiSensorConfig(IssoMultiSensorConfig* cfg);
        void removeMultiSensorConfig(const QString& cfgName);
        void removeAllMultiSensorConfigs();

        IssoMultiSensorConfig* multiSensorConfig(const QString& cfgName);
        IssoMultiSensorConfig* multiSensorConfig(int cfgId);

        int nextMultiSensorId() const;

        QByteArray multiSensorIdArray() const;
        QList<quint8> multiSensorIdList() const;

        virtual void toString(QTextStream& stream);
};


/*!
 * \brief   Фабрика классов конфигурации
 */
class IssoChainConfigFactory
{
    public:
        static IssoChainConfig* make(IssoChainType type,
                                     const QString& name,
                                     int sensorCount,
                                     bool enabled);
        static IssoChainConfig* make(IssoChainType type);
};


#endif // ISSOCHAINCONFIG_H
