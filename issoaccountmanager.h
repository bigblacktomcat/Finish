#ifndef ISSOACCOUNTMANAGER_H
#define ISSOACCOUNTMANAGER_H

#include <QObject>
#include <QList>
#include "issocommondata.h"
#include "issodatabase.h"


class IssoAccount
{
    private:
        QString m_userName;
        QString m_password;
        bool m_superUser;
    public:
        IssoAccount();
        IssoAccount(const QString& userName, const QString& password, bool isSuperUser);
        QString userName() const;
        QString password() const;
        bool isSuperUser() const;
        bool isValid() const;
};


class IssoAccountManager : public QObject
{
//        Q_OBJECT
    private:
        QMap<QString, IssoAccount> m_accounts;
        IssoDataBase* m_db;

    public:
        explicit IssoAccountManager(IssoDataBase* db, QObject *parent = nullptr);

        bool load();
        bool save();
        bool authenticate(IssoAppType appType, IssoAccount& account,
                          IssoGuiMode& guiMode, /*const QStringList &cfgNames,*/
                          /*QString &selectedCfgName*/
                          int& selectedObjId);

        IssoAccount findAccount(const QString& userName);
        QStringList userNames() const;

    private slots:
//        void showObjectConfigSetupDialog();
};

#endif // ISSOACCOUNTMANAGER_H
