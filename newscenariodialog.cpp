/**
\brief Реализация диалогового класса NewScenarioDialog
Для нового сценария задаём имя, область действия
\date  2020-08-18
**/
#include "newscenariodialog.h"
#include "ui_newscenariodialog.h"


NewScenarioDialog::NewScenarioDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewScenarioDialog)
{
    ui->setupUi(this);
    // убрать кнопку помощи
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    // надписи на стандартных кнопках
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("ОК"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Отмена"));
    // заполнить комбобокс "Область действия"
    ui->cbScenarioScope->addItems(IssoCommonData::scenarioScopeStrings());
}


NewScenarioDialog::~NewScenarioDialog()
{
    delete ui;
}


QString NewScenarioDialog::scenarioName() const
{
    return ui->leScenarioName->text();
}


IssoScenarioScope NewScenarioDialog::scenarioScope() const
{
    return IssoCommonData::scenarioScopeByString(ui->cbScenarioScope->currentText());
}
