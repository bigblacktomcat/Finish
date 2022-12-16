#include "addguardedobjectdialog.h"
#include "issocommondata.h"
#include "ui_addguardedobjectdialog.h"


AddGuardedObjectDialog::AddGuardedObjectDialog(const QList<int> objectIds,
                                               QWidget *parent)
    : QDialog(parent),
      ui(new Ui::AddGuardedObjectDialog),
      m_objectIds(objectIds)
{
    ui->setupUi(this);
//    // убрать кнопку помощи
//    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
//    // надписи на стандартных кнопках
//    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("ОК"));
//    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Отмена"));
//    // установить валидатор ip-адреса
//    ui->leStartIp->setValidator(IssoCommonData::makeIpValidator(this));

    // инициализировать диалог
    init();
    // инициализировать последовательность перехода
    initTabOrder();
}


AddGuardedObjectDialog::~AddGuardedObjectDialog()
{
    delete ui;
}


int AddGuardedObjectDialog::objectId() const
{
    return ui->sbObjectId->value();
}

/**
 * @brief AddGuardedObjectDialog::objectCard
 * диалог по созданию нового охраняемого объекта
 * @return ссылка на новый охраняемый объект
 */
IssoObjectCard AddGuardedObjectDialog::objectCard() const
{
//    return IssoObjectCard(ui->sbObjectId->value(),
//                          ui->leCity->text(),
////                          QDate::currentDate(),
//                          ui->leAddress->text(),
//                          ui->leContactName->text(),
//                          ui->lePhone->text());

    return IssoObjectCard(ui->sbObjectId->value(),
                          ui->leCity->text(),
                          ui->leRegion->text(),
                          ui->leStreet->text(),
                          ui->leBuilding->text(),
                          ui->leContactName->text(),
                          ui->lePhone->text());
}


void AddGuardedObjectDialog::init()
{
    // убрать кнопку помощи
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    // надписи на стандартных кнопках
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("ОК"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Отмена"));
    // установить валидатор ip-адреса
    ui->leStartIp->setValidator(IssoCommonData::makeIpValidator(this));
    //
    ui->scaBuildContainer->widget()->layout()->setMargin(4);
    ui->scaBuildContainer->widget()->layout()->setSpacing(20);
    // задать обработчик изменения количества этажей
    connect(ui->sbBuildCount, SIGNAL(valueChanged(int)),
            this,             SLOT(processBuildCountChanged(int)));
    // вызвать обработчик с текщим количеством
    processBuildCountChanged(buildCount());
    // задать размеры
    setBaseSize(500, 600);
    setMinimumWidth(500);
    setMinimumHeight(600);

//    // задать последовательность перехода по Tab
//    setTabOrder(ui->leCity,         ui->leAddress);
//    setTabOrder(ui->leAddress,      ui->leContactName);
//    setTabOrder(ui->leContactName,  ui->lePhone);
//    setTabOrder(ui->lePhone,        ui->sbBuildCount);
//    setTabOrder(ui->sbBuildCount,   ui->leStartIp);
//    setTabOrder(ui->leStartIp,      ui->leStartMac);
//    setTabOrder(ui->leStartMac,     ui->wgtContainer);

    // заполнить список IP-адресов сетевых карт
    fillServerIpCombo();
}


void AddGuardedObjectDialog::initTabOrder()
{
    QList<QWidget*> widgets
    {
        ui->sbObjectId,
        ui->leCity,
        ui->leRegion,
        ui->leStreet,
        ui->leBuilding,
//        ui->leAddress,
        ui->leContactName,
        ui->lePhone,
        ui->sbBuildCount,
        ui->leStartIp,
        ui->leStartMac,
        ui->cbServerIp,
        ui->wgtContainer
    };
    // задать последовательность перехода по Tab
    for (int i = 0; i < widgets.size() - 1; ++i)
        setTabOrder(widgets[i], widgets[i + 1]);
}


void AddGuardedObjectDialog::addBuildWidget()
{
    // номер добавяемого виждета (соответствует номеру здания)
    int buildNum = m_buildWidgets.size() + 1;
    // создать виджет
    BuildInitWidget* widget = new BuildInitWidget(buildNum, this);
    // добавить виджет в отображение
    m_buildWidgets.insert(buildNum, widget);
    // добавить виджет в диалог
    ui->scaBuildContainer->widget()->layout()->addWidget(widget);
}


void AddGuardedObjectDialog::addBuildWidgets(int count)
{
    for (int i = 0; i < count; ++i)
        addBuildWidget();
}


void AddGuardedObjectDialog::removeBuildWidget()
{
    // номер удаляемого виждета (соответствует номеру здания)
    int buildNum = m_buildWidgets.size();
    // проверить наличие номера в отображении
    if (!m_buildWidgets.contains(buildNum))
        return;
    // удалить виджет из отображения
    BuildInitWidget* widget = m_buildWidgets.take(buildNum);
    // удалить виджет из диалога
    ui->scaBuildContainer->widget()->layout()->removeWidget(widget);
    // уничтожить виджет
    delete widget;
}


void AddGuardedObjectDialog::removeBuildWidgets(int count)
{
    for (int i = 0; i < count; ++i)
        removeBuildWidget();
}


void AddGuardedObjectDialog::fillServerIpCombo()
{
    ui->cbServerIp->clear();
    // IP сервера по умолчанию
    ui->cbServerIp->addItem(tr("По умолчанию (%2)").arg(IssoConst::DEFAULT_SERVER_IP.toString()),
                            QVariant(IssoConst::DEFAULT_SERVER_IP.toIPv4Address()));
    // заполнить список IP найденных интерфейсов
    foreach (auto ip, IssoCommonData::localIps())
    {
        ui->cbServerIp->addItem(ip.toString(), QVariant(ip.toIPv4Address()));
    }

//    ui->cbServerIp->clear();
//    foreach (auto ip, IssoCommonData::localIps())
//    {
//        ui->cbServerIp->addItem(ip.toString());
//    }
}


int AddGuardedObjectDialog::buildCount() const
{
    ui->sbBuildCount->interpretText();
    return ui->sbBuildCount->value();
}


QHostAddress AddGuardedObjectDialog::startIp() const
{
    return QHostAddress(ui->leStartIp->text());
}


QByteArray AddGuardedObjectDialog::startMac() const
{
    return IssoModuleDescriptor::macStringToBytes(ui->leStartMac->text());
}


QHostAddress AddGuardedObjectDialog::serverIp() const
{
    return QHostAddress(ui->cbServerIp->currentData().toUInt());
//    return QHostAddress(ui->cbServerIp->currentText());
}


BuildInitWidget *AddGuardedObjectDialog::findWidgetByNumber(int num)
{
    return m_buildWidgets.value(num, nullptr);
}


QList<BuildInitWidget *> AddGuardedObjectDialog::incompleteWidgets()
{
    QList<BuildInitWidget*> list;
    foreach (BuildInitWidget* wgt, m_buildWidgets.values())
    {
        if (!wgt->isOk())
            list << wgt;
    }
    return list;
}


void AddGuardedObjectDialog::processBuildCountChanged(int count)
{
    int delta = count - m_buildWidgets.size();
    if (delta > 0)
        addBuildWidgets(abs(delta));
    else if (delta < 0)
        removeBuildWidgets(abs(delta));
}


void AddGuardedObjectDialog::accept()
{
    // убедиться, что объект с заданным ID отсутствует
    if (m_objectIds.contains(objectId()))
    {
        // сформировать строку сообщения
        QString str = tr("Объект с ID = %1 уже существует!"
                         "\nВведите другой ID объекта.")
                                            .arg(objectId());
        // вывести сообщение
        QMessageBox::warning(this, tr("Внимание"), str);
        return;
    }


    // получить список незаполненных виджетов
    QList<BuildInitWidget*> widgets = incompleteWidgets();
    // если список пуст
    if (widgets.isEmpty())
    {
        // принять
        QDialog::accept();
    }
    else
    {
        // сформировать строку с незаполненными виджетами
        QString str = tr("Не завершена настройка здания %1")
                        .arg(widgets.first()->buildNumber(), 2, 10, QLatin1Char('0'));
        // вывести сообщение
        QMessageBox::warning(this, tr("Внимание"), str);
    }
}
