#include "descriptorsetupdialog.h"
#include "ui_descriptorsetupdialog.h"

#include <QMessageBox>


DescriptorSetupDialog::DescriptorSetupDialog(QList<IssoModule *> modules, QWidget *parent)
    : QDialog(parent),
      ui(new Ui::DescriptorSetupDialog),
      m_modules(modules)
{
    ui->setupUi(this);
    init();
}


void DescriptorSetupDialog::init()
{
    // убрать кнопку помощи
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    // задать валидаторы ip-адреса
    QRegExpValidator* ipValidator = IssoCommonData::makeIpValidator(this);
    ui->leDescrIp->setValidator(ipValidator);
    ui->leDescrServerIp->setValidator(ipValidator);
    // установить обработчики событий виджетов
    initWidgetEventHandlers();
    // заполнить список модулей
    ui->cbModules->addItems(this->moduleNames());
    // надписи на стандартных кнопках
    ui->buttonBox->button(QDialogButtonBox::Close)->setText(tr("Закрыть"));
}


IssoModule *DescriptorSetupDialog::findModuleByDisplayName(const QString &displayName)
{
    foreach (IssoModule* module, m_modules)
    {
        if (module->displayName() == displayName)
            return module;
    }
    return nullptr;
}


QStringList DescriptorSetupDialog::moduleNames() const
{
    QStringList list;
    foreach (IssoModule* module, m_modules)
        list << module->displayName();
    return list;
}


void DescriptorSetupDialog::initWidgetEventHandlers()
{
    connect(ui->cbModules, SIGNAL(currentIndexChanged(QString)),
            this,          SLOT(processCbModulesCurrentIndexChanged(QString)));
    connect(ui->btnReadDescriptor, SIGNAL(clicked(bool)),
            this,                  SLOT(processBtnReadDescriptorClicked()));
    connect(ui->btnWriteDescriptor, SIGNAL(clicked(bool)),
            this,                   SLOT(processBtnWriteDescriptorClicked()));
}


void DescriptorSetupDialog::setDescriptorToControls(const IssoModuleDescriptor &descr)
{
    // ip
    ui->leDescrIp->setText(descr.ipString());
    // порт
    ui->sbDescrPort->setValue(descr.port());
    // МАС
    ui->leMac->setText(descr.macString());
    // ip сервера
    ui->leDescrServerIp->setText(descr.serverIpString());
    // порт сервера
    ui->sbDescrServerPort->setValue(descr.serverPort());
    // телефон
    ui->lePhone->setText(descr.phoneString());
    // разрешение использования телефона
    ui->gbPhone->setChecked(descr.phoneEnabled());
}


IssoModuleDescriptor DescriptorSetupDialog::getDescriptorFromControls()
{
    // получить данные:
    // ip
    quint32 ip = QHostAddress(ui->leDescrIp->text()).toIPv4Address();
    // порт
    quint16 port = ui->sbDescrPort->value();
    // МАС
    QByteArray mac = IssoModuleDescriptor::macStringToBytes(ui->leMac->text());
    // ip сервера
    quint32 serverIp = QHostAddress(ui->leDescrServerIp->text()).toIPv4Address();
    // порт сервера
    quint16 serverPort = ui->sbDescrServerPort->value();
    // телефон
    QString phoneStr = ui->lePhone->text().remove('(').remove(')').remove('-');
    QByteArray phone = IssoModuleDescriptor::phoneStringToBytes(phoneStr);
    // разрешение использования телефона
    bool phoneEnabled = ui->gbPhone->isChecked();
    //
    // вернуть новый дескриптор
    return IssoModuleDescriptor(ip, mac, port, serverIp, serverPort, phone, phoneEnabled, 0);
}


DescriptorSetupDialog::~DescriptorSetupDialog()
{
    delete ui;
}


void DescriptorSetupDialog::setCurrentModule(const QString &displayName)
{
    int index = ui->cbModules->findText(displayName, Qt::MatchExactly);
    if (index != -1)
        ui->cbModules->setCurrentIndex(index);
}


void DescriptorSetupDialog::processCbModulesCurrentIndexChanged(const QString &moduleName)
{
    IssoModule* module = findModuleByDisplayName(moduleName);
    if (!module)
        return;
    // получить дескриптор выбранного модуля
    IssoModuleDescriptor descr = module->getDescriptor();
    // заполнить поля
    setDescriptorToControls(descr);
}


void DescriptorSetupDialog::processBtnReadDescriptorClicked()
{
    IssoModule* module = findModuleByDisplayName(ui->cbModules->currentText());
    if (module)
        emit descriptorRequested(module->uniqueId());
}


void DescriptorSetupDialog::processBtnWriteDescriptorClicked()
{
    // найти модуль
    IssoModule* module = findModuleByDisplayName(ui->cbModules->currentText());
    if (!module)
        return;
    // сформировать дескриптор из данных
    IssoModuleDescriptor descr = getDescriptorFromControls();
    // инициировать отправку запроса изменения дескриптора
    emit descriptorChanged(module->uniqueId(), descr);

//    qDebug() << ip << endl
//             << port << endl
//             << QString(mac.toHex(' ')) << endl
//             << serverIp << endl
//             << serverPort << endl
//             << QString(phone.toHex(' ')) << endl
//             << endl << phoneEnabled;
}


void DescriptorSetupDialog::processDescriptorRead(const QString &moduleDisplayName,
                                                  const IssoModuleDescriptor &descr)
{
    // найти модуль
    IssoModule* module = findModuleByDisplayName(moduleDisplayName);
    if (!module)
    {
        // отобразить сообщение об ошибке
        QMessageBox::warning(this, tr("Внимание"),
                             tr("Считаны настройки модуля, не найденного в конфигурации!"));
        return;
    }
    // отобразить настройки дескриптора на форме
    setDescriptorToControls(descr);
    //
    // сообщить об успешном считывании дескриптора
    QString s = tr("Настройки модуля <%1> успешно считаны").arg(moduleDisplayName);
    QMessageBox::information(this, tr("Информация"), s);
}


void DescriptorSetupDialog::processDescriptorWritten(const QString &moduleDisplayName,
                                                     const IssoModuleDescriptor &descr)
{
    // найти модуль
    IssoModule* module = findModuleByDisplayName(moduleDisplayName);
    if (!module)
    {
        // отобразить сообщение об ошибке
        QMessageBox::warning(this, tr("Внимание"),
                             tr("Записаны настройки модуля, не найденного в конфигурации!"));
        return;
    }
    // сохранить установленный дескриптор в модуле
    module->setDescriptor(descr);
    // отобразить настройки дескриптора на форме
    setDescriptorToControls(descr);
    //
    // сообщить об успешном считывании дескриптора
    QString s = tr("Настройки модуля <%1> успешно записаны").arg(moduleDisplayName);
    QMessageBox::information(this, tr("Информация"), s);
}


void DescriptorSetupDialog::processDescriptorFailed(const QString &moduleDisplayName,
                                                    IssoCmdId cmdId)
{
    // отобразить сообщение об ошибке операции с дескриптором
    QString s = QString("Настройки модуля <%1> не %2")
                        .arg(moduleDisplayName)
                        .arg(cmdId == CMD_DESC_GET ? tr("считаны") : tr("записаны"));
    QMessageBox::critical(this, tr("Ошибка"), s);
}
