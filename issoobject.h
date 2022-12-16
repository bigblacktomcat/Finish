#ifndef ISSOOBJECT_H
#define ISSOOBJECT_H

#include "issobuilding.h"
#include "issocamera.h"
#include "issomodule.h"
#include "issomoduleconfig.h"
#include "issoobjectcard.h"
#include "issosensortype.h"
#include "issoscenario.h"

#include <QObject>
#include <QFile>

class IssoObject : public QObject
{
        Q_OBJECT
    private:
//        int m_id;
        //
        QList<IssoModule*> m_modules;           // список всех модулей системы
        QList<IssoCamera*> m_cameras;           // список всех камер системы
        //
        IssoObjectCard m_objectCard;            // карточка охраняемого объекта
        QList<IssoBuilding*> m_buildings;       // список зданий
        IssoBuilding* mp_building = nullptr;    // выбранное здание
        IssoFloorPlan* mp_floor = nullptr;      // выбранный план этажа
        //
        int m_currentUniqueId;
        QHostAddress m_currentIp;
        QByteArray m_currentMac;

        QHostAddress m_serverIp;
        //
        // карта конфигураций модулей
        QMap<QString, IssoModuleConfig*> m_moduleConfigs;
        // карта конфигураций шлейфов
        QMap<QString, IssoChainConfig*> m_chainConfigs;

        // список классов реле
        QMap<QString, IssoRelayClass> m_relayClasses;
        // список сценариев
        QMap<QString, IssoScenario*> m_scenarios;
        //
        // имена сценариев, назныченных кнопкам панели управления системой
        QMap<int, QString> m_buttonScenarioLinks;


        int m_totalModules;
        int m_totalMs;
        int m_totalChannels;

        //
        /// \brief  Уничтожить здания
        void destroyBuildings();
        /// \brief  Уничтожить модули
        void destroyModules();
        /// \brief  Уничтожить камеры
        void destroyCameras();
        //
        int nextUniqueId();
        QHostAddress nextIp();
        QByteArray nextMac();

    public:
        IssoObject(QObject *parent = nullptr);
//        explicit IssoObject(int id, QObject *parent = nullptr);
        ~IssoObject();

        /// \brief  Загрузить конфигурацию из потока
        void load(QDataStream& stream);
        /// \brief  Сохранить конфигурацию в поток
        void save(QDataStream& stream);
        /// \brief  Загрузить конфигурацию из файла
        bool load(const QString &fileName);
        /// \brief  Сохранить конфигурацию в файл
        QByteArray save(const QString &fileName);

        void load(const QByteArray &configData);

        /// \brief  Найти здание по имени
        IssoBuilding* findBuildingByName(const QString& name);
        /// \brief  Найти здание по номеру
        IssoBuilding* findBuildingByNumber(int num);
        /// \brief  Найти модуль по отображаемому имени
        IssoModule* findModuleByDisplayName(const QString& displayName, bool locatedOnly = false);
        /// \brief  Найти модуль по id
        IssoModule* findModuleById(int uniqueId, bool locatedOnly = false);
        /// \brief  Найти модуль по IP-адресу
        IssoModule* findModuleByAddress(const QHostAddress& ip, bool locatedOnly = false);
        /// \brief  Найти этаж по номеру здания и этажа
        IssoFloorPlan* findFloor(int buildNum, int floorNum);
        /// \brief  Найти камеру по id
        IssoCamera* findCameraById(int id);
        /// \brief  Найти камеру по IP-адресу
        IssoCamera* findCameraByIp(const QHostAddress& ip);
        /// \brief  Изменить месторасположение модуля
        bool changeModuleLocation(IssoModule *module, int buildNum, int floorNum);
        /// \brief  Изменить месторасположение камеры
        bool changeCameraLocation(IssoCamera *camera, int buildNum, int floorNum);
        //
        /// \brief  Получить список всех модулей системы
        ///         (включая не распределенные по зданиям)
        QList<IssoModule*> modules() const;
        /// \brief  Добавить модуль в охраняемый объект
        void addModule(IssoModule* module);
        /// \brief  Добавить модуль в охраняемый объект
        QString addModule(int displayId, const QHostAddress& ip, quint16 port,
                          int uniqueId);
        /// \brief  Добавить список модулей в охраняемый объект
        void addModules(const QList<IssoModule*>& modules);
        /// \brief  Удалить модуль из объекта
        void removeModule(IssoModule* module);
        /// \brief  Удалить модуль из объекта
        bool removeModule(const QString& moduleName);
        //
        /// \brief  Добавить этаж в здание
        bool addFloorToBuilding(const QString& buildName, int floorNum,
                                const QPixmap& background);
        /// \brief  Удалить этаж из здания
        bool removeFloorFromBuilding(const QString& buildName, const QString& floorName);
        //
        /// \brief  Добавить здание в охраняемый объект
        QString addBuilding(const QString& name, int number);
        /// \brief  Добавить здание в охраняемый объект
        void addBuilding(int buildNum, int lowerFloorNum, int floorCount,
                         const QMap<QString, QString> &imageMap,
                         const QMap<int, int> &moduleCountMap,
                         const QSet<IssoParamId> &checkableParams);
        /// \brief  Удалить здание из охраняемого объект
        void removeBuilding(const QString& name);
        /// \brief  Добавить камеру в охраняемый объект
        void addCamera(IssoCamera* camera);
        /// \brief  Добавить камеру в охраняемый объект
        IssoCamera* addCamera(int id, const QHostAddress& ip, quint16 port, const QString &password,
                              int buildNum, int floorNum);
        /// \brief  Удалить камеру из охраняемого объекта
        void removeCamera(IssoCamera* camera);
        /// \brief  Удалить камеру из охраняемого объекта
        bool removeCamera(int id);
        //
        IssoModuleState state() const;
        void setGraphicItemsMovable(bool movable);
        /// \brief  Сформировать список модулей, распределенных по зданиям
        QList<IssoModule*> locatedModules() const;
        QStringList unlocatedModuleNames() const;
        QStringList moduleNames() const;
        QStringList buildingNames() const;
        /// \brief  Получить выбранный модуль
        IssoModule* selectedModule();
        //
        /// \brief  Задать стартовый IP-адрес
        void setCurrentIp(const QHostAddress& ip);
        /// \brief  Задать стартовый MAC-адрес
        void setCurrentMac(const QByteArray& mac);
        //
        /// \brief  Получить карточку охраняемого объекта
        IssoObjectCard objectCard() const;
        /// \brief  Задать карточку охраняемого объекта
        void setObjectCard(const IssoObjectCard &objectCard);
        //
        void sortModules();
        //
        QList<IssoModule*> filteredModules(int buildNum, int floorNum);
        QList<IssoModule*> filteredModules(const QString& buildName,
                                           const QString& floorName);
        //
        QList<int> availableBuildNumbers();
        QList<int> availableFloorNumbers();
        QList<int> availableCameraIds();
        //
        QList<IssoBuilding *>& buildings();
        //
        IssoBuilding* currentBuilding();
        void setCurrentBuilding(IssoBuilding* building);
        //
        IssoFloorPlan* currentFloor();
        void setCurrentFloor(IssoFloorPlan* floor);
        //
        int id() const;
        void setId(int id);
        //
        // отображаемое имя объекта
        QString displayName() const;
        //
        int currentUniqueId() const;
        QHostAddress currentIp() const;
        QByteArray currentMac() const;
        QHostAddress serverIp() const;

        /// \brief  Получить список всех камер системы
        QList<IssoCamera *> cameras() const;
        //
        // конфигурации модулей
        //
        QMap<QString, IssoModuleConfig *> moduleConfigs() const;
        void setModuleConfigs(const QMap<QString, IssoModuleConfig *> &moduleConfigs);
        //
        void insertModuleConfig(IssoModuleConfig* moduleCfg);
        void removeModuleConfig(const QString& moduleCfgName);
        void removeAllModuleConfigs();
        IssoModuleConfig* moduleConfig(const QString& moduleCfgName);
        /// \brief  Получить список имен конифгураций модулей
        QStringList moduleConfigNames() const;
        //
        /// \brief  Сохранить конфигурации модулей
        void saveModuleConfigs(QDataStream& stream);
        /// \brief  Загрузить конфигурации модулей
        bool loadModuleConfigs(QDataStream& stream);
        //
        // конфигурации шлейфов
        //
        QMap<QString, IssoChainConfig *> chainConfigs() const;
        void setChainConfigs(const QMap<QString, IssoChainConfig *> &chainConfigs);
        //
        void insertChainConfig(IssoChainConfig* chainCfg);
        void removeChainConfig(const QString& chainCfgName);
        void removeAllChainConfigs();
        IssoChainConfig* chainConfig(const QString& chainCfgName);
        /// \brief  Получить список имен конифгураций шлейфов
        QStringList chainConfigNames();
        /// \brief  Получить список имен конифгураций шлейфов заданного типа
        QStringList chainConfigNames(IssoChainType chainType);
        //
        /// \brief  Сохранить конфигурации шлейфов
        void saveChainConfigs(QDataStream& stream);
        /// \brief  Загрузить конфигурации шлейфов
        bool loadChainConfigs(QDataStream& stream);
        //
        bool moduleParamEnabled(const QString& moduleCfgName,
                                IssoParamId paramId);
        IssoChainConfig* moduleChainConfig(const QString& moduleCfgName,
                                           IssoParamId paramId);
        IssoAddressChainConfig* moduleAddressChainConfig(const QString& moduleCfgName);
        IssoMultiSensorConfig* findMultiSensorCfgByModuleCfgName(const QString& moduleCfgName,
                                                                 int multiSensorId);
        IssoMultiSensorConfig* findMultiSensorCfgByModuleId(int moduleId,
                                                       int multiSensorId);
        IssoMultiSensorConfig* findMultiSensorCfgByModuleName(const QString& moduleName,
                                                       int multiSensorId);

        void applyModuleConfigToModule(const QString& moduleName,
                                       const QString& moduleCfgName);
        void applyModuleConfigChanges(const QString& moduleCfgName);
        void applyChainConfigChanges(const QString& chainCfgName);

        void setOnline();
        void setOffline();
        void setModuleOnline(const QString& moduleName);
        void setModuleOffline(const QString& moduleName);
        //
        // классы реле
        //
        QMap<QString, IssoRelayClass> relayClasses() const;
        void setRelayClasses(const QMap<QString, IssoRelayClass> &relayClasses);
        //
        void insertRelayClass(const IssoRelayClass& relayClass);
        void removeRelayClass(const QString& relayClassName);
        void replaceRelayClass(const QString& oldClassName,
                               const IssoRelayClass& newClass);
        IssoRelayClass relayClass(const QString& relayClassName);
        bool relayClassExists(const QString& relayClassName);
        QStringList relayClassNames() const;
        //
        // сценарии
        //
        QMap<QString, IssoScenario*> scenarios() const;
        void setScenarios(const QMap<QString, IssoScenario*> &scenarios);
        //
        void insertScenario(IssoScenario* scenario);
        void removeScenario(const QString& scenarioName);
        void updateScenario(const QString& oldScenarioName,
                            const QString& newScenarioName,
                            IssoScenarioScope newScope,
                            const QStringList& newClassNames);
        void destroyAllScenarios();
        IssoScenario* scenario(const QString& scenarioName);
        bool scenarioExists(const QString& scenarioName);
        QStringList scenarioNames() const;
        //
        /// \brief  Сохранить сценарии
        void saveScenarios(QDataStream& stream);
        /// \brief  Загрузить сценарии
        bool loadScenarios(QDataStream& stream);
        //
        // карта соответствия номера кнопка панели управления системой имени сценария
        //
        QMap<int, QString> buttonScenarioLinks() const;
        void setButtonScenarioLinks(const QMap<int, QString> &buttonScenarioLinks);
        // добавить назначение сценария кнопке панели управления
        void insertButtonScenarioLink(int buttonNum, const QString& buttonScenarioName);
        // удалить назначение сценария кнопке панели управления
        void removeButtonScenarioLink(int buttonNum);
        // получить имя сценария по связанному с ним номеру кнопки
        QString buttonScenarioLink(int buttonNum);
        void setServerIp(const QHostAddress &serverIp);
        //
        // сформировать статичеык сводные данные
        void calcTotalSummary();
        // сформировать тревожные сводные данные
        void calcAlarmSummary(int &alarmModules, int &alarmMs, int &alarmChannels) const;
        //
        int totalModules() const;
        int totalMs() const;
        int totalChannels() const;
        //
};

#endif // ISSOOBJECT_H
