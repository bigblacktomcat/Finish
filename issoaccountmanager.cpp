#include "issoaccountmanager.h"
#include "issoconfigutils.h"
#include "issologindialog.h"

#include <QMessageBox>


IssoAccount::IssoAccount() : IssoAccount("", "", false)
{
}


IssoAccount::IssoAccount(const QString &userName,
                         const QString &password, bool superUser)
    : m_userName(userName),
      m_password(password),
      m_superUser(superUser)
{
}


QString IssoAccount::userName() const
{
    return m_userName;
}


QString IssoAccount::password() const
{
    return m_password;
}


bool IssoAccount::isSuperUser() const
{
    return m_superUser;
}


bool IssoAccount::isValid() const
{
    return (!m_userName.isEmpty());
}




IssoAccountManager::IssoAccountManager(IssoDataBase *db, QObject *parent)
    : QObject(parent),
      m_db(db)
{
    // загрузить имена пользователей
    load();
}


bool IssoAccountManager::load()
{
    // ВРЕМЕННО
    // user
    m_accounts.insert("user", IssoAccount("user", "user", false));
    // admin
    m_accounts.insert("admin", IssoAccount("admin", "admin", true));
//    m_accounts.insert("admin", IssoAccount("admin", "bccjflvbybcnhfnjh", true));
    return true;
}


bool IssoAccountManager::save()
{
    return false;
}


bool IssoAccountManager::authenticate(IssoAppType appType,
                                      IssoAccount &account,
                                      IssoGuiMode &guiMode,
                                      int &selectedObjId)
{
    // успех аутентификации
    bool success = false;
    //
    // возвращаемые параметры:
    // режим отображения
    guiMode = IssoGuiMode::STANDART_MONITOR;
    // создать диалог аутентификации
    IssoLoginDialog dlg(appType, userNames(), IssoConst::LOGIN_TRY_COUNT, m_db);
    //
    // выводить диалог, пока не будут введены корректные данные,
    // либо не вернется отмена действия (кнопка Отмена или превышение числа попыток)
    while (dlg.tryCount() > 0)
    {
        // если отмена аутентификации, выход
        if (dlg.exec() == QDialog::Rejected)
            break;

        // найти аккаунт
        IssoAccount foundAccount = findAccount(dlg.userName());
        // если аккаунт не найден, повторить попытку
        if (!foundAccount.isValid())
            continue;
        // если пароль верный
        if (foundAccount.password() == dlg.password())
        {
            // установить успех
            success = true;
            // аккаунт
            account = foundAccount;
            // режим отображения
            guiMode = dlg.guiMode();
            // id выбранного объекта
            selectedObjId = dlg.objectId();
            // выйти из цикла
            break;
        }
    }
    // если неуспех
    if (!success)
    {
        // если превышено число попыток ввода, сообщить
        if (dlg.tryCount() <= 0)
        {
            QMessageBox::warning(nullptr, "Внимание",
                                 "Превышено число попыток аутентификации");
        }
    }
    return success;
}

//bool IssoAccountManager::authenticate(IssoAppType appType,
//                                      IssoAccount &account,
//                                      IssoGuiMode &guiMode,
//                                      QString& selectedCfgName)
//{
//    // успех аутентификации
//    bool success = false;
//    //
//    // возвращаемые параметры:
//    // режим отображения
//    guiMode = IssoGuiMode::STANDART_MONITOR;
//    //
//    // получить список конфигураций объектов
//    QStringList cfgNames = IssoConfigUtils::getObjectNames(m_db);
//    //
//    // создать диалог аутентификации
//    IssoLoginDialog dlg(appType, userNames(), IssoConst::LOGIN_TRY_COUNT, cfgNames);
//    // задать обработчик кнопки конфигурирования
//    connect(&dlg, &IssoLoginDialog::objectConfigsSetupRequested,
//            this, &IssoAccountManager::showObjectConfigSetupDialog);
//    //
//    // выводить диалог, пока не будут введены корректные данные,
//    // либо не вернется отмена действия (кнопка Отмена или превышение числа попыток)
//    while (dlg.tryCount() > 0)
//    {
//        // если отмена аутентификации, выход
//        if (dlg.exec() == QDialog::Rejected)
//            break;

//        // найти аккаунт
//        IssoAccount foundAccount = findAccount(dlg.userName());
//        // если аккаунт не найден, повторить попытку
//        if (!foundAccount.isValid())
//            continue;
//        // если пароль верный
//        if (foundAccount.password() == dlg.password())
//        {
//            // установить успех
//            success = true;
//            // аккаунт
//            account = foundAccount;
//            // режим отображения
//            guiMode = dlg.guiMode();
//            // имя выбранного конфига (объекта)
//            selectedCfgName = dlg.configName();
//            // выйти из цикла
//            break;
//        }
//    }
//    // если неуспех
//    if (!success)
//    {
//        // если превышено число попыток ввода, сообщить
//        if (dlg.tryCount() <= 0)
//        {
//            QMessageBox::warning(nullptr, "Внимание",
//                                 "Превышено число попыток аутентификации");
//        }
//    }
//    return success;
//}


IssoAccount IssoAccountManager::findAccount(const QString &userName)
{
    return m_accounts.value(userName);
}


QStringList IssoAccountManager::userNames() const
{
    QStringList names;
    foreach (IssoAccount account, m_accounts)
        names << account.userName();
    return names;
}


//void IssoAccountManager::showObjectConfigSetupDialog()
//{
//    // вызвать диалог настройки данных объектов
//    ObjectConfigSetupDialog dlg(m_db, qApp->activeWindow());
//    dlg.exec();
//}
