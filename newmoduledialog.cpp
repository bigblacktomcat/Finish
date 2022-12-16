/**
\brief Реализация диалогового класса NewModuleDialog
Для нового модуля задаём идентификатор, IP-адрес, порт
\date  2020-08-18
**/
#include "newmoduledialog.h"
#include "ui_newmoduledialog.h"
#include "issocommondata.h"


NewModuleDialog::NewModuleDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewModuleDialog)
{
    ui->setupUi(this);
    init();
    initTabOrder();
}


NewModuleDialog::~NewModuleDialog()
{
    delete ui;
}


int NewModuleDialog::uniqueId() const
{
    return ui->sbUniqueId->value();
}


void NewModuleDialog::setUniqueId(int id)
{
    ui->sbUniqueId->setValue(id);
}


int NewModuleDialog::displayId() const
{
//    return ui->sbDisplayId->text().toInt();
    return ui->sbDisplayId->value();
}


void NewModuleDialog::setDisplayId(int id)
{
    ui->sbDisplayId->setValue(id);
}


QHostAddress NewModuleDialog::ip() const
{
    return QHostAddress(ui->leIp->text());
}


void NewModuleDialog::setIp(const QHostAddress &ip)
{
    ui->leIp->setText(ip.toString());
}


quint16 NewModuleDialog::port() const
{
    return ui->sbPort->value();
}


void NewModuleDialog::setPort(quint16 port)
{
    ui->sbPort->setValue(port);
}


void NewModuleDialog::init()
{
    // убрать кнопку помощи
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    // надписи на стандартных кнопках
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("ОК"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Отмена"));
    // установить валидатор ip-адреса
    ui->leIp->setValidator(IssoCommonData::makeIpValidator(this));
}


void NewModuleDialog::initTabOrder()
{
    QList<QWidget*> widgets
    {
        ui->sbUniqueId,
        ui->sbDisplayId,
        ui->leIp,
        ui->sbPort
    };
    // задать последовательность перехода по Tab
    for (int i = 0; i < widgets.size() - 1; ++i)
        setTabOrder(widgets[i], widgets[i + 1]);
}
