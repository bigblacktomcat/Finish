#include "issocheckableselectionpanel.h"
#include "ui_issocheckableselectionpanel.h"


IssoCheckableSelectionPanel::IssoCheckableSelectionPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::IssoCheckableSelectionPanel)
{
    ui->setupUi(this);
    init();
}


IssoCheckableSelectionPanel::~IssoCheckableSelectionPanel()
{
    delete ui;
}


QString IssoCheckableSelectionPanel::label() const
{
    return ui->chkLabel->text();
}


void IssoCheckableSelectionPanel::setLabel(const QString &label)
{
    ui->chkLabel->setText(label);
}


void IssoCheckableSelectionPanel::setItemNames(const QStringList &itemNames)
{
    ui->cbItemName->clear();
    ui->cbItemName->addItems(itemNames);
}


QString IssoCheckableSelectionPanel::selectedItemName() const
{
    return ui->cbItemName->currentText();
}


void IssoCheckableSelectionPanel::setSelectedItemName(const QString &itemName)
{
    int index = ui->cbItemName->findText(itemName);
    ui->cbItemName->setCurrentIndex(index);
}


bool IssoCheckableSelectionPanel::checked() const
{
    return ui->chkLabel->isChecked();
}


void IssoCheckableSelectionPanel::setChecked(bool checked)
{
    ui->chkLabel->setChecked(checked);
}


void IssoCheckableSelectionPanel::init()
{
    connect(ui->chkLabel, &QCheckBox::toggled,
            [this](bool checked)
            {
                ui->cbItemName->setEnabled(checked);
            });
}

