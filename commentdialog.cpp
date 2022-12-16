#include "commentdialog.h"
#include "ui_commentdialog.h"
#include <QPushButton>

CommentDialog::CommentDialog(const QString &date, const QString &time,
                             const QString &moduleName, const QString &state,
                             const QString &sensorName, const QString &comment,
                             QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CommentDialog)
{
    ui->setupUi(this);
    // убрать кнопку помощи
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    // надписи на стандартных кнопках
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("ОК"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Отмена"));
    // заполнить форму
    ui->lblDateTime->setText(QString("%1   %2").arg(date).arg(time));
    ui->lblModuleName->setText(moduleName);
    ui->lblState->setText(state);
    ui->lblSensorName->setText(sensorName);
    ui->txteComment->setText(comment);
}


CommentDialog::~CommentDialog()
{
    delete ui;
}


QString CommentDialog::comment() const
{
    return ui->txteComment->toPlainText();
}
