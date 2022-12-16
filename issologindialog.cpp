#include "issoconfigutils.h"
#include "issologindialog.h"
#include "objectconfigsetupdialog.h"
#include "ui_issologindialog.h"
#include <QDate>
#include <QDate>
#include <QPushButton>


IssoLoginDialog::IssoLoginDialog(IssoAppType appType,
                                 const QStringList &userNames,
                                 int tryCount,
                                 IssoDataBase* db,
                                 QWidget *parent)
    : QDialog(parent),
      ui(new Ui::IssoLoginDialog),
      m_tryCount(tryCount),
      m_db(db)
{
    ui->setupUi(this);
    // задать тип приложения
    setAppType(appType);
    //
    // проинициализировать диалог
    init();
    // задать обработчики
    initEventHandlers();
    //
    // заполнить список имен пользователей
    ui->cbUserName->addItems(userNames);
    // заполнить список имен объектов
    updateObjectNames();
}


IssoLoginDialog::~IssoLoginDialog()
{
    delete ui;
}


QString IssoLoginDialog::userName() const
{
    return ui->cbUserName->currentText();
}


QString IssoLoginDialog::password() const
{
    return ui->lePassword->text();
}


IssoGuiMode IssoLoginDialog::guiMode() const
{
    return ui->cbGuiMode->currentData().value<IssoGuiMode>();
}


int IssoLoginDialog::objectId() const
{
    return (ui->cbConfig->currentData().isValid()  ?
                        ui->cbConfig->currentData().toInt() : -1);
}


QString IssoLoginDialog::configName() const
{
    return (ui->cbConfig->currentIndex() == 0 ? "" : ui->cbConfig->currentText());
}


//bool IssoLoginDialog::savedConfig() const
//{
//    return (ui->cbConig->currentIndex() == 0);
//}


int IssoLoginDialog::tryCount() const
{
    return m_tryCount;
}


IssoAppType IssoLoginDialog::appType() const
{
    return m_appType;
}


void IssoLoginDialog::setAppType(const IssoAppType &appType)
{
    // установить тип приложения
    m_appType = appType;
    //
    // задать имя приложения
//    ui->lblAppType->setText(IssoCommonData::getAppTypeName(appType));
    ui->gbSoftwareInfo->setTitle(IssoCommonData::getAppTypeName(appType));
    // если не СНК, скрыть выбор конфигурации
    if (appType != IssoAppType::SNK)
    {
        ui->lblConfig->setVisible(false);
        ui->cbConfig->setVisible(false);
        ui->btnSetupConfigs->setVisible(false);
    }
}


void IssoLoginDialog::setupObjectConfigs()
{
    // вызвать диалог настройки данных объектов
    ObjectConfigSetupDialog dlg(m_db, qApp->activeWindow());
    dlg.exec();
    // обновить список имен объектов
    updateObjectNames();
}


void IssoLoginDialog::init()
{
    // заполнить список режимов отображения
    initGuiModes();
    // убрать кнопку помощи
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    // надписи на стандартных кнопках
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("ОК"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Отмена"));
    // дата релиза
    QLocale usLocale(QLocale::English, QLocale::UnitedStates);
//    qDebug() << usLocale.toDate(QString(__DATE__).simplified(), "MMM d yyyy") << endl;
    QDate buildDate = usLocale.toDate(QString(__DATE__).simplified(), "MMM d yyyy");
    ui->lblReleaseDate->setText(tr("Дата релиза: %1").arg(buildDate.toString("dd.MM.yyyy")));
    // версия релиза
    ui->lblReleaseVersion->setText(tr("Версия ПО: %1").arg(IssoConst::BUILD_VERSION));
}


void IssoLoginDialog::initGuiModes()
{
    ui->cbGuiMode->addItem(tr("Стандартный монитор"),
                           QVariant::fromValue(IssoGuiMode::STANDART_MONITOR));
    ui->cbGuiMode->addItem(tr("Сенсорный экран"),
                           QVariant::fromValue(IssoGuiMode::TOUCH_SCREEN));
    ui->cbGuiMode->addItem(tr("Планшет"),
                           QVariant::fromValue(IssoGuiMode::TABLET));
}


void IssoLoginDialog::initEventHandlers()
{
    connect(ui->btnSetupConfigs, &QPushButton::clicked,
            this, &IssoLoginDialog::setupObjectConfigs);
    //            this, &IssoLoginDialog::objectConfigsSetupRequested);
}


void IssoLoginDialog::updateObjectNames()
{
    // очистить список имен объектов
    ui->cbConfig->clear();
    // получить список имен объектов
    QMap<int, QString> cfgNames = IssoConfigUtils::getObjectNames(m_db);
    //
    // пункт создания новой конфигурации
    ui->cbConfig->addItem(tr("Новая"), -1);
    // конфигурации объектов из БД
    foreach (int id, cfgNames.keys())
    {
        // добавить имя объекта и привязать ID
        ui->cbConfig->addItem(cfgNames.value(id), id);
    }
    // выбрать последний элемент списка
    ui->cbConfig->setCurrentIndex(ui->cbConfig->count() - 1);
}


void IssoLoginDialog::setVisible(bool visible)
{
    if (visible)
    {
        if (m_tryCount <= 0)
        {
            setResult(QDialog::Rejected);
            visible = false;
        }

        // отобразить число оставшихся попыток
        ui->lblTryCount->setText(QString::number(m_tryCount));
        // декрементировать число оставшихся попыток
        m_tryCount--;
        // фокус на ввод пароля
        ui->lePassword->setFocus();
    }
    // вызвать метод предка
    QDialog::setVisible(visible);
}
