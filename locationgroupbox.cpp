#include "locationgroupbox.h"
#include "ui_locationgroupbox.h"


LocationGroupBox::LocationGroupBox(QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::LocationGroupBox)
{
    ui->setupUi(this);
    initEventHandlers();
}

LocationGroupBox::~LocationGroupBox()
{
    delete ui;
}


QList<IssoBuilding *> LocationGroupBox::buildings() const
{
    return m_buildings;
}


void LocationGroupBox::setBuildings(const QList<IssoBuilding *> &buildings)
{
    m_buildings = buildings;
    // обновить выпадающий список зданий
    ui->cbBuilding->clear();
    // добавить возможность обнуления здания
    ui->cbBuilding->addItem("", QVariant(0));
    // добавить все здания системы
    foreach (IssoBuilding* build, m_buildings)
        ui->cbBuilding->addItem(build->displayName(), QVariant(build->number()));
}


int LocationGroupBox::buildNumber() const
{
    return (ui->cbBuilding->currentIndex() == -1 ?
                0 : ui->cbBuilding->currentData().toInt());
}


void LocationGroupBox::setBuildNumber(int buildNum)
{
    // найти индекс здания в списке по номеру
    int index = ui->cbBuilding->findData(QVariant(buildNum));
    // выбрать здание
    ui->cbBuilding->setCurrentIndex(index);

    processBuildingChanged(index);
}


int LocationGroupBox::floorNumber() const
{
    return (ui->cbFloor->currentIndex() == -1 ?
                0 : ui->cbFloor->currentData().toInt());
}


void LocationGroupBox::setFloorNumber(int floorNum)
{
    // найти индекс этажа в списке по номеру
    int index = ui->cbFloor->findData(QVariant(floorNum));
    // выбрать этаж
    ui->cbFloor->setCurrentIndex(index);
}


void LocationGroupBox::setLocation(int buildNum, int floorNum)
{
    setBuildNumber(buildNum);
    setFloorNumber(floorNum);
}


IssoBuilding *LocationGroupBox::findBuildingByName(const QString &name)
{
    foreach (IssoBuilding* build, m_buildings)
    {
        if (build->displayName() == name)
            return build;
    }
    return nullptr;
}


void LocationGroupBox::initEventHandlers()
{
    connect(ui->cbBuilding, SIGNAL(currentIndexChanged(int)),
            this,           SLOT(processBuildingChanged(int)));
}


void LocationGroupBox::processBuildingChanged(int index)
{
    // очистить список этажей
    ui->cbFloor->clear();
    // найти выбранное здание по имени
    IssoBuilding* build = findBuildingByName(ui->cbBuilding->itemText(index));
    if (!build)
        return;
    // обновить выпадающий список этажей здания
    foreach (IssoFloorPlan* floor, build->floors())
        ui->cbFloor->addItem(floor->displayName(), QVariant(floor->number()));
}
