#include "issoeditlistpanel.h"
#include "ui_issoeditlistpanel.h"


IssoEditListPanel::IssoEditListPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::IssoEditListPanel)
{
    ui->setupUi(this);
    init();
    setEventHandlers();
}


IssoEditListPanel::~IssoEditListPanel()
{
    delete ui;
}


QString IssoEditListPanel::title() const
{
    return ui->lblTitle->text();
}


void IssoEditListPanel::setTitle(const QString &title)
{
    ui->lblTitle->setText(title);
    ui->lblTitle->setVisible(!title.isEmpty());

}


QListWidget *IssoEditListPanel::list()
{
    return ui->listItems;
}


int IssoEditListPanel::itemCount() const
{
    return ui->listItems->count();
}


QListWidgetItem *IssoEditListPanel::item(int index)
{
    return ui->listItems->item(index);
}


QStringList IssoEditListPanel::itemNames()
{
    QStringList list;
    for (int i = 0; i < itemCount(); ++i)
    {
        list << item(i)->text();
    }
    return list;
}


QListWidgetItem* IssoEditListPanel::addItem(const QString &displayName, bool selected)
{
    // создать элемент списка
    QListWidgetItem* item = new QListWidgetItem(displayName);
    // добавить и, если необходимо, выделить
    addItem(item, selected);
    return item;
}


void IssoEditListPanel::addItem(QListWidgetItem *item, bool selected)
{
    // добавить элемент в список
    ui->listItems->addItem(item);
    // если необходимо, выбрать элемент
    if (selected)
        item->setSelected(true);
}


void IssoEditListPanel::addItems(const QStringList &displayNames)
{
    ui->listItems->addItems(displayNames);
    // если список не пуст, выделить первый элемент
    if (itemCount() > 0)
        setItemSelected(0, true);
}


void IssoEditListPanel::removeItem(const QString &displayName)
{
    // найти элемент списка по имени
    QListWidgetItem* item = findItem(displayName);
    // удалить из списка и уничтожить
    removeAndDeleteItem(item);
    //
    // если список не пуст, выделить первый элемент
    if (itemCount() > 0)
        setItemSelected(0, true);
}


void IssoEditListPanel::removeAndDeleteItem(QListWidgetItem *item)
{
    if (item)
    {
        // удалить элемент списка
        ui->listItems->removeItemWidget(item);
        // уничтожить элемент списка
        delete item;
    }
}


QListWidgetItem *IssoEditListPanel::findItem(const QString &displayName)
{
    // найти элементы визуального списка по имени
    QList<QListWidgetItem*> foundItems =
            ui->listItems->findItems(displayName, Qt::MatchExactly);
    // вернуть найденный элемент, либо пустой указатель
    return (foundItems.isEmpty() ? nullptr : foundItems.first());
}


bool IssoEditListPanel::hasSelectedItems() const
{
    return (!ui->listItems->selectedItems().isEmpty());
}


QListWidgetItem *IssoEditListPanel::selectedItem()
{
    // если список выделенных элементов не пуст
    if (!ui->listItems->selectedItems().isEmpty())
        // вернуть первый элемент
        return ui->listItems->selectedItems().first();
    else
        // иначе пустой указатель
        return nullptr;
}


QString IssoEditListPanel::selectedItemName()
{
    QListWidgetItem* item = selectedItem();
    return (item ? item->text() : "");
}


QStringList IssoEditListPanel::selectedItemNames()
{
    return QStringList { selectedItemName() };
}


void IssoEditListPanel::setItemSelected(int index, bool selected)
{
    // установить / снять выделение элемента списка по индексу
    if ((index >= 0) && (index < ui->listItems->count()))
        ui->listItems->item(index)->setSelected(selected);
}


void IssoEditListPanel::setButtonsVisible(bool visible)
{
    ui->wgtButtons->setVisible(visible);
}


void IssoEditListPanel::setEditBtnVisible(bool visible)
{
    ui->btnEdit->setVisible(visible);
}


void IssoEditListPanel::setListItemDelegate(QAbstractItemDelegate *delegate)
{
    ui->listItems->setItemDelegate(delegate);
}


void IssoEditListPanel::scrollListToItem(const QListWidgetItem *item)
{
    ui->listItems->scrollToItem(item);
}


void IssoEditListPanel::changeItemName(const QString &oldName, const QString &newName)
{
    QListWidgetItem* item = findItem(oldName);
    if (item)
        item->setText(newName);
}


void IssoEditListPanel::clearList()
{
    ui->listItems->clear();
}


void IssoEditListPanel::clearSelection()
{
    ui->listItems->clearSelection();
}


void IssoEditListPanel::setItems(const QStringList &displayNames)
{
    clearList();
    addItems(displayNames);
}


void IssoEditListPanel::init()
{
    // очистить заголовок
    setTitle("");
    // очистить список
    ui->listItems->clear();
}


void IssoEditListPanel::setEventHandlers()
{
    // сигналы кнопок
    connect(ui->btnAdd,     SIGNAL(clicked(bool)),
            this,           SIGNAL(addBtnClicked(bool)));
    connect(ui->btnRemove,  SIGNAL(clicked(bool)),
            this,           SIGNAL(removeBtnClicked(bool)));
    connect(ui->btnEdit,    SIGNAL(clicked(bool)),
            this,           SIGNAL(editBtnClicked(bool)));
    // сигналы списка
    connect(ui->listItems,  SIGNAL(itemSelectionChanged()),
            this,           SIGNAL(selectionChanged()));
    connect(ui->listItems,  SIGNAL(itemDoubleClicked(QListWidgetItem*)),
            this,           SIGNAL(itemDoubleClicked(QListWidgetItem*)));
}
