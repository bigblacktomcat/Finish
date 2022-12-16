#ifndef ISSOMODULECONFIG_H
#define ISSOMODULECONFIG_H

#include <QObject>
#include "issochainconfig.h"
#include "issocommondata.h"

/**
 * @brief The IssoChainConfigLink struct конфигурации шлейфов
 */
struct IssoChainConfigLink
{
    private:
        IssoParamId m_paramId;
        QString m_chainCfgName;
        bool m_enabled;

    public:
        IssoChainConfigLink(IssoParamId paramId = PARAM_UNDEFINED,
                            const QString& chainCfgName = "",
                            bool enabled = false);
        IssoParamId paramId() const;
        void setId(const IssoParamId &paramId);
        QString chainCfgName() const;
        void setChainCfgName(const QString &chainCfgName);
        bool enabled() const;

        bool isValid() const;
        //
        friend QDataStream &operator<<(QDataStream &out, const IssoChainConfigLink& link);
        friend QDataStream &operator>>(QDataStream &in, IssoChainConfigLink& link);
};

/**
 * @brief The IssoModuleConfig class конфигурация модулей
 * @param QString m_name Имя конфигурации модуля
 * @param QMap<IssoParamId, IssoAnalogChainConfig*> m_sensorConfigs карта соответствия id датчиков конфигурациям встроенных датчиков
 * @param QMap<IssoParamId, IssoChainConfigLink> m_chainConfigLinks карта соответствия id шлейфов ссылкам на конфигурации шлейфов
 * @param QMap<IssoParamId, QString> m_relayClassLinks карта соответствия id реле имени класса реле
 * @param QMap<IssoModuleState, QString> m_scenarioLinks карта соответствия состояния модуля имени сценария
 * @param QMap<int, QString> m_buttonClassLinks имена классов, назныченных кнопкам панели управления модулем
 */

class IssoModuleConfig
{
    private:
        // Имя конфигурации модуля
        QString m_name;
        // карта соответствия id датчиков конфигурациям встроенных датчиков
        QMap<IssoParamId, IssoAnalogChainConfig*> m_sensorConfigs;
        // карта соответствия id шлейфов ссылкам на конфигурации шлейфов
        QMap<IssoParamId, IssoChainConfigLink> m_chainConfigLinks;

        // карта соответствия id реле имени класса реле
        QMap<IssoParamId, QString> m_relayClassLinks;
        // карта соответствия состояния модуля имени сценария
        QMap<IssoModuleState, QString> m_scenarioLinks;
        //
        // имена классов, назныченных кнопкам панели управления модулем
        QMap<int, QString> m_buttonClassLinks;

    public:
        IssoModuleConfig(QString name = "");
        virtual ~IssoModuleConfig();

        void saveToStream(QDataStream& stream);
        void loadFromStream(QDataStream& stream);
        //
        QString name() const;
        void setName(const QString &name);
        //
        // конфигурации встроенных датчиков
        //
        QMap<IssoParamId, IssoAnalogChainConfig*> sensorConfigs() const;
        void setSensorConfigs(const QMap<IssoParamId, IssoAnalogChainConfig*> &sensorConfigs);
        //
        void insertSensorConfig(IssoParamId id, IssoAnalogChainConfig* sensorConfig);
        void removeSensorConfig(IssoParamId id);
        void removeAllSensorConfigs();
        IssoAnalogChainConfig* sensorConfig(IssoParamId id) const;
        bool sensorEnabled(IssoParamId id) const;
        //
        // ссылки на конфигурации шлейфов
        //
        QMap<IssoParamId, IssoChainConfigLink> chainConfigLinks() const;
        void setChainConfigLinks(const QMap<IssoParamId, IssoChainConfigLink> &chainConfigLinks);
        //
        void insertChainConfigLink(IssoParamId id, const QString& chainConfigName, bool enabled);
        void removeChainConfigLink(IssoParamId id);
        IssoChainConfigLink chainConfigLink(IssoParamId id) const;
        IssoChainConfigLink chainConfigLink(const QString& chainConfigName) const;
        QString chainConfigName(IssoParamId id) const;
        bool chainEnabled(IssoParamId id) const;
        //
        bool containsChainConfigLink(const QString& chainConfigName);
        void removeChainConfigLinksByName(const QString& chainConfigName);
        //
        // карта соответствия состояния модуля имени сценария
        //
        QMap<IssoModuleState, QString> scenarioLinks() const;
        void setScenarioLinks(const QMap<IssoModuleState, QString> &scenarioLinks);
        //
        // добавить привязку состояния модуля к имени сценария
        void insertScenarioLink(IssoModuleState moduleState, const QString& scenarioName);
        // удалить привязку состояния модуля к имени сценария
        void removeScenarioLink(IssoModuleState moduleState);
        // получить имя сценария по связанному с ним состоянию модуля
        QString scenarioNameByModuleState(IssoModuleState moduleState) const;
        // удалить все привязки к заданному имени сценария
        void removeScenarioName(const QString& scenarioName);
        // обновить все привязки к заданному имени сценария
        void updateScenarioName(const QString& oldName, const QString& newName);
        //
        // карта соответствия id реле имени имени класса
        //
        QMap<IssoParamId, QString> relayClassLinks() const;
        void setRelayClassLinks(const QMap<IssoParamId, QString> &relayClassLinks);
        //
        // добавить привязку id реле к имени класса
        void insertRelayClassLink(IssoParamId paramId, const QString& relayClassName);
        // удалить привязку id реле к имени класса
        void removeRelayClassLink(IssoParamId paramId);
        // получить имя класса по связанному с ним id реле
        QString relayClassName(IssoParamId paramId) const;
        //
        // карта соответствия номера кнопка панели управления имени класса
        //
        QMap<int, QString> buttonClassLinks() const;
        void setButtonClassLinks(const QMap<int, QString> &buttonClassLinks);
        // добавить назначение класса кнопке панели управления
        void insertButtonClassLink(int buttonNum, const QString& buttonClassName);
        // удалить назначение класса кнопке панели управления
        void removeButtonClassLink(int buttonNum);
        // получить имя класса по связанному с ним номеру кнопки
        QString buttonClassLink(int buttonNum);
        //
        //
        // удалить все привязки (из всех коллекций) к заданному имени класса реле
        void removeRelayClassName(const QString& relayClassName);
        // обновить все привязки (из всех коллекций) к заданному имени класса реле
        void updateRelayClassName(const QString& oldName, const QString& newName);
        //
        //
        // получить список Id реле, связанных с классом
        // (имя класса -> список Id реле)
        QList<IssoParamId> getRelayIdsByClassName(const QString& className);
        // получить список Id реле, активируемых кнопкой
        // (номер кнопки -> имя класса -> список Id реле)
        QList<IssoParamId> getRelayIdsByButtonNumber(int buttonNum); 
        /**
         * @brief getButtonNumbersByRelayId получить список номеров кнопок, связанных с реле
         * @param relayId
         * @return (Id реле -> имя класса -> список номеров кнопок)
         */
        QList<int> getButtonNumbersByRelayId(IssoParamId relayId);

        bool paramEnabled(IssoParamId id) const;

        friend QDataStream &operator<<(QDataStream &out, IssoModuleConfig* config);
        friend QDataStream &operator>>(QDataStream &in, IssoModuleConfig* config);

        virtual void toString(QTextStream& stream);
};

#endif // ISSOMODULECONFIG_H
