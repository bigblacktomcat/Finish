#include "issoconfigutils.h"
#include "objectconfigsetupdialog.h"
#include "ui_objectconfigsetupdialog.h"

#include <QFileDialog>
#include <QMessageBox>


ObjectConfigSetupDialog::ObjectConfigSetupDialog(IssoDataBase *db, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ObjectConfigSetupDialog),
    m_db(db)
{
    ui->setupUi(this);
    // проинициализировать диалог
    init();
    // задать обработчики
    initEventHandlers();
    // загрузить карточки объектов
    loadObjCards();
}


ObjectConfigSetupDialog::~ObjectConfigSetupDialog()
{
    delete ui;
}


void ObjectConfigSetupDialog::init()
{
    // убрать кнопку помощи
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    // надписи на стандартных кнопках
    ui->buttonBox->button(QDialogButtonBox::Close)->setText(tr("Закрыть"));
    // задать заголовок
    ui->wgtObjects->setTitle(tr("Объекты"));
    // скрыть кнопку редактирования
    ui->wgtObjects->setEditBtnVisible(false);
}


void ObjectConfigSetupDialog::initEventHandlers()
{
    connect(ui->wgtObjects, SIGNAL(addBtnClicked(bool)),
            this,           SLOT(processAddCfgBtnClicked()));
    connect(ui->wgtObjects, SIGNAL(removeBtnClicked(bool)),
            this,           SLOT(processRemoveCfgBtnClicked()));
    connect(ui->wgtObjects, SIGNAL(selectionChanged()),
            this,           SLOT(processListConfigsSelectionChanged()));
    connect(ui->btnSaveSettings, SIGNAL(clicked(bool)),
            this,                SLOT(processSaveSettingsBtnClicked()));
}


void ObjectConfigSetupDialog::loadObjCards()
{
    // загрузить список карточек объектов
    m_objCards = m_db->readObjectCards();
    //
    // заполнить визуальный список имен объектов:
    //
    // очистить список имен объектов
    ui->wgtObjects->clearList();
    // заполнить список имен объектов
    foreach (auto card, m_objCards)
    {
        // добавить имя объекта в визуальный список
        // и привязать ID объекта
        ui->wgtObjects->addItem(card.displayName())
                        ->setData(Qt::UserRole, QVariant(card.id()));
    }
}


void ObjectConfigSetupDialog::processAddCfgBtnClicked()
{
    QString fileName =
            QFileDialog::getOpenFileName(this,
                                         tr("Загрузить конфигурацию объекта"),
                                         QDir::currentPath(),
                                         tr("Конфигурация объекта (*.issocfg)"));
    if(fileName.isEmpty())
        return;

    QFile file(fileName);
    // открыть файл для чтения
    if (!file.open(QIODevice::ReadOnly))
        return;

    // прочитать данные конфигурации объекта
    QByteArray configData = file.readAll();
    //
    // загрузить объект из массива
    IssoObject obj;
    obj.load(configData);
    // если объект с таким ID уже содержится в БД,
    // известить и выйти
    if (IssoConfigUtils::objectExists(m_db, obj.id()))
    {
        QMessageBox::warning(this, tr("Внимание"),
                             tr("Невозможно добавить объект с ID = %1, "
                                "т.к. объект с таким ID уже существует.")
                                                            .arg(obj.id()));
        return;
    }
    //
    // добавить конфигурацию в БД
    if (IssoConfigUtils::insertConfig(m_db, configData))
    {
        // обновить список карточек объектов
        loadObjCards();

//        // добавить имя объекта в визуальный список
//        // и привязать ID объекта
//        ui->wgtObjects->addItem(obj.displayName(), true)
//                        ->setData(Qt::UserRole, QVariant(obj.id()));

    }
}


void ObjectConfigSetupDialog::processRemoveCfgBtnClicked()
{
    // если не выбран объект, выход
    if (!ui->wgtObjects->hasSelectedItems())
        return;

    // получить выбранный элемент
    auto selectedItem = ui->wgtObjects->selectedItem();
    // получить имя объекта
    QString name = selectedItem->text();
    // получить id
    int id = selectedItem->data(Qt::UserRole).toInt();
    // запросить подтверждение у оператора
    if (!IssoCommonData::showQuestionDialog(this, tr("Подтверждение удаления"),
                               tr("Вы действительно хотите удалить "
                                  "конфигурацию объекта <%1>?").arg(name)))
        return;
    // удалить конфигурацию из БД
    if (IssoConfigUtils::deleteConfig(m_db, id))
    {
        // обновить список карточек объектов
        loadObjCards();
//        // удалить имя объекта из визуального списка
//        ui->wgtObjects->removeItem(name);
    }
}


void ObjectConfigSetupDialog::processSaveSettingsBtnClicked()
{
    IssoObjectCard card;
    // заполнить карточку объекта:
    // id
    card.setId(ui->sbObjectId->value());
    // город
    card.setCity(ui->leCity->text());
    // регион
    card.setRegion(ui->leRegion->text());
    // улица
    card.setStreet(ui->leStreet->text());
    // строение
    card.setBuilding(ui->leBuilding->text());
    // контакт
    card.setContactName(ui->leContactName->text());
    // телефон
    card.setPhone(ui->lePhone->text());
    //
    // обновить карточку объекта в БД
    if (m_db->updateObjectCard(card))
    {
        // обновить карточку объекта в локальной коллекции
        m_objCards.insert(card.id(), card);
        // вывести сообщение
        QMessageBox::information(this, tr("Сохранение изменений"),
                             tr("Сохранение данных карточки объекта %1 успешно выполнено.")
                                                            .arg(card.displayName()));
    }
    else
    {
        // вывести сообщение
        QMessageBox::warning(this, tr("Сохранение изменений"),
                             tr("Не удалось сохранить данные карточки объекта %1.")
                                                            .arg(card.displayName()));
    }
}


void ObjectConfigSetupDialog::processListConfigsSelectionChanged()
{
    int id = -1;
    // если объект выбран
    if (ui->wgtObjects->hasSelectedItems())
    {
        // получить ID объекта
        id = ui->wgtObjects->selectedItem()->data(Qt::UserRole).toInt();
    }
    // получить объект по ID
    IssoObjectCard card = m_objCards.value(id);
    // заполнить поля диалога:
    // id
    ui->sbObjectId->setValue(card.id());
    // город
    ui->leCity->setText(card.city());
    // регион
    ui->leRegion->setText(card.region());
    // улица
    ui->leStreet->setText(card.street());
    // строение
    ui->leBuilding->setText(card.building());
    // контакт
    ui->leContactName->setText(card.contactName());
    // телефон
    ui->lePhone->setText(card.phone());
}
