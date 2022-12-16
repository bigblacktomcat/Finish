#ifndef LOCATIONGROUPBOX_H
#define LOCATIONGROUPBOX_H

#include "issobuilding.h"

#include <QGroupBox>

namespace Ui {
    class LocationGroupBox;
}

class LocationGroupBox : public QGroupBox
{
        Q_OBJECT

    private:
        Ui::LocationGroupBox *ui;
        QList<IssoBuilding *> m_buildings;

        /// \brief  Задать обработчики
        void initEventHandlers();
        /// \brief  Найти здание по имени
        IssoBuilding* findBuildingByName(const QString& name);

    public:
        explicit LocationGroupBox(QWidget *parent = 0);
        ~LocationGroupBox();

        QList<IssoBuilding *> buildings() const;
        void setBuildings(const QList<IssoBuilding *> &buildings);

        int buildNumber() const;
        void setBuildNumber(int buildNum);
        int floorNumber() const;
        void setFloorNumber(int floorNum);

        void setLocation(int buildNum, int floorNum);

    private slots:
        /// \brief  Обработчик выбора здания
        void processBuildingChanged(int index);
};

#endif // LOCATIONGROUPBOX_H
