#include "selectmoduledialog.h"
#include "ui_selectmoduledialog.h"
#include <QPushButton>


SelectModuleDialog::SelectModuleDialog(const QStringList &moduleNames, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelectModuleDialog)
{
    ui->setupUi(this);
    // убрать кнопку помощи
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    // надписи на стандартных кнопках
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("ОК"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Отмена"));

    // отсортировать имена модулей
    QStringList names = moduleNames;
//    qSort(names.begin(), names.end());
    std::sort(names.begin(), names.end());
    // заполнить список имен модулей, не распределенных по этажам
    ui->listUnlocatedModules->clear();
    ui->listUnlocatedModules->addItems(names);
    // выбрать первый элемент
    if (ui->listUnlocatedModules->count() > 0)
        ui->listUnlocatedModules->item(0)->setSelected(true);
}


SelectModuleDialog::~SelectModuleDialog()
{
    delete ui;
}


QString SelectModuleDialog::selectedModuleName()
{
    if (ui->listUnlocatedModules->selectedItems().isEmpty())
        return "";
    else
        return ui->listUnlocatedModules->selectedItems().first()->text();
}
