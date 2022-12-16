#ifndef ISSOSENSORCHAIN_H
#define ISSOSENSORCHAIN_H

#include "issosensor.h"
#include <QObject>
#include <QList>


class IssoSensorChain: public QObject
{
        Q_OBJECT
    private:
        int m_id;
        int m_moduleUniqueId;
        int m_moduleDisplayId;
        IssoChainType m_chainType;
//        bool m_visible;
        QList<IssoSensor*> m_sensors;

    public:
        explicit IssoSensorChain(QObject* parent = nullptr);
        IssoSensorChain(int id, int moduleUniqueId, int moduleDisplayId,
                        IssoChainType chainType, int sensorCount,
                        QObject* parent = nullptr);
        ~IssoSensorChain();

        /// \brief  Загрузить данные шлейфа из потока
        void loadFromStream(QDataStream& stream);
        /// \brief  Сохранить данные шлейфа в поток
        void saveToStream(QDataStream &stream);

        void add(IssoSensor* sensor);
        void addSeveral(int sensorCount);
        void remove(IssoSensor* sensor);
        void removeById(int sensorId);
        void removeSeveral(int sensorCount);
        void removeAll();

        int id() const;
        void setId(int id);
        int moduleUniqueId() const;
        void setModuleUniqueId(int moduleUniqueId);
        int moduleDisplayId() const;
        void setModuleDisplayId(int moduleDisplayId);
        IssoChainType type() const;
        void setType(const IssoChainType &type);

        QList<IssoSensor *> sensors() const;
        void setSensors(QList<IssoSensor *> sensors);

        IssoSensor* sensor(quint8 id);
        QString sensorLocation(quint8 id);
        int sensorCount();
        void setSensorCount(int count);
        QList<int> sensorIds();
        int alarmCount();

//        bool visible() const;

        void displayAllSensors(QGraphicsScene* scene);
        void displayAlarmSensors(QGraphicsScene* scene);
        void hideAllSensors();

        void setInitPosition(QPointF pos);

        void setStateToAllSensors(IssoState state, const QString& hint = "");
        void setStateToSingleSensor(int multiSensorId, IssoState state, const QString& hint = "");
        void clearState();

        bool inNormalState();

        void setMovable(bool movable);
};

#endif // ISSOSENSORCHAIN_H
