#ifndef ISSOLOGINDIALOG_H
#define ISSOLOGINDIALOG_H

#include <QDialog>
#include "issocommondata.h"
#include "issodatabase.h"


namespace Ui {
    class IssoLoginDialog;
}


class IssoLoginDialog : public QDialog
{
        Q_OBJECT
    private:
        Ui::IssoLoginDialog *ui;
        IssoAppType m_appType;
        int m_tryCount;
        IssoDataBase* m_db;

        void init();
        void initGuiModes();
        void initEventHandlers();
        void updateObjectNames();

    protected:
        virtual void setVisible(bool visible) Q_DECL_OVERRIDE;


    public:
        explicit IssoLoginDialog(IssoAppType appType,
                                 const QStringList& userNames,
                                 int tryCount,
//                                 const QStringList &cfgNames,
//                                 const QMap<int, QString>& cfgNames,
                                 IssoDataBase* db,
                                 QWidget *parent = 0);
        ~IssoLoginDialog();

        QString userName() const;
        QString password() const;
        IssoGuiMode guiMode() const;
//        bool savedConfig() const;
        int objectId() const;
        QString configName() const;
        int tryCount() const;

        IssoAppType appType() const;
        void setAppType(const IssoAppType &appType);

//    signals:
//        void objectConfigsSetupRequested();

    private slots:
        void setupObjectConfigs();
};

#endif // ISSOLOGINDIALOG_H
