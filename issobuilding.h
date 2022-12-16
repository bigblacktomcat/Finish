#ifndef ISSOBUILDING_H
#define ISSOBUILDING_H

#include <QObject>
#include "issofloorplan.h"


/*!
 * \brief   Класс, представляющий здание.
 * \details Содержит список всех этажей здания,
 *          каждый из которых включает список модулей МАСО,
 *          расположенных на данном этаже.
 */
class IssoBuilding : public QObject
{
        Q_OBJECT
    private:
        static int testId;
        static int testAddr;

        int m_number;                       // номер здания
        QString m_name;                     // имя здания
        QList<IssoFloorPlan*> m_floors;     // список этажей здания

        /// \brief  Подготовить к началу работы
        void init();
        /// \brief  Подготовить к завершению работы
        void uninit();

    public:
        IssoBuilding();
        explicit IssoBuilding(int number, QString name, QObject *parent = nullptr);
        ~IssoBuilding();

        /// \brief  Загрузить данные здания из потока
        void loadFromStream(QDataStream& stream);
        /// \brief  Сохранить данные здания в поток
        void saveToStream(QDataStream &stream);

        bool addFloor(int number, const QPixmap& background);
        bool addFloor(IssoFloorPlan *floor);
        bool removeFloor(IssoFloorPlan *floor);

        /// \brief  Получить список этажей здания
        QList<IssoFloorPlan *> floors() const;
        /// \brief  Получить количество этажей здания
        int floorCount() const;
        /// \brief  Получить имя здания
        QString name() const;
        /// \brief  Получить отображаемое имя здания
        QString displayName() const;
        /// \brief  Получить список имен всех этажей здания
        QStringList floorNames();
        /// \brief  Получить набор номеров этажей здания
        QSet<int> floorNumbers();
        /// \brief  Найти этаж по имени
        IssoFloorPlan* findFloorByName(const QString& name);
        /// \brief  Найти этаж по номеру
        IssoFloorPlan* findFloorByNumber(int number);
        /// \brief  Найти модуль по идентификатору
        IssoModule* findModuleById(int moduleId);
        /// \brief  Найти модуль по адресу
        IssoModule* findModuleByAddress(QHostAddress addr);
        /// \brief  Найти модуль по отображаемому имени
        IssoModule* findModuleByDisplayName(const QString& displayName);
        /// \brief  Сформировать список всех модулей здания
        QList<IssoModule *> modules();
        /// \brief  Получить номер здания
        int number() const;

        /// \brief  Получить набор состояний всех модулей здания
        QSet<IssoModuleState> moduleStates() const;

        bool activated() const;

    signals:

    public slots:
};

#endif // ISSOBUILDING_H
