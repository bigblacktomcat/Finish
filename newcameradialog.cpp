/**
\brief Реализация диалогового класса NewCameraDialog
Для новой камеры задаём идентификатор, адрес порт, пароль, номер камеры, номер этажа
\date  2020-08-18
**/
#include "newcameradialog.h"
#include "ui_newcameradialog.h"


NewCameraDialog::NewCameraDialog(const QList<int> &availableIds,
                                 const QList<IssoBuilding *> &buildings,
                                 QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewCameraDialog)
{
    ui->setupUi(this);
    // настроить диалог
    init(tr("Добавить камеру"));
    // заполнить список доступных ID камер
    foreach (int id, availableIds)
    {
        // добавить описание и номер этажа
        ui->cbId->addItem(QString::number(id), QVariant::fromValue(id));
    }
    // передать в компонент расположения камеры список зданий
    ui->gbLocation->setBuildings(buildings);
}


NewCameraDialog::~NewCameraDialog()
{
    delete ui;
}


void NewCameraDialog::init(const QString &title)
{
    // установить заголовок
    setWindowTitle(title);
    // убрать кнопку помощи
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    // надписи на стандартных кнопках
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("ОК"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Отмена"));
    // установить валидатор ip-адреса
    QRegExpValidator* ipValidator = IssoCommonData::makeIpValidator(this);
    ui->leIp->setValidator(ipValidator);
}


int NewCameraDialog::id() const
{
    if (ui->cbId->count() == 0)
        return -1;
    else
        return ui->cbId->currentData().toInt();
}


QHostAddress NewCameraDialog::ip() const
{
    return QHostAddress(ui->leIp->text());
}


quint16 NewCameraDialog::port() const
{
    return ui->sbPort->value();
}


QString NewCameraDialog::password() const
{
    return ui->lePassword->text();
}


int NewCameraDialog::buildNumber() const
{
    return ui->gbLocation->buildNumber();
}


int NewCameraDialog::floorNumber() const
{
    return ui->gbLocation->floorNumber();
}
