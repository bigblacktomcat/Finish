#ifndef ADDGUARDEDOBJECTDIALOG_H
#define ADDGUARDEDOBJECTDIALOG_H

#include <QDialog>
#include <QHostAddress>
#include <QCloseEvent>
#include <QMessageBox>
#include "buildinitwidget.h"
#include "issomoduledescriptor.h"
#include "issoobjectcard.h"


namespace Ui {
    class AddGuardedObjectDialog;
}

class AddGuardedObjectDialog : public QDialog
{
        Q_OBJECT

    private:
        Ui::AddGuardedObjectDialog *ui;
        QMap<int, BuildInitWidget*> m_buildWidgets;
        QList<int> m_objectIds;

        void init();
        void initTabOrder();
        void addBuildWidget();
        void addBuildWidgets(int count);
        void removeBuildWidget();
        void removeBuildWidgets(int count);
        void fillServerIpCombo();

    public:
        explicit AddGuardedObjectDialog(const QList<int> objectIds, QWidget *parent = 0);
        ~AddGuardedObjectDialog();

        // ID охраняемого объекта
        int objectId() const;
        // карточка охраняемого объекта
        IssoObjectCard objectCard() const;
        // характеристики охраняемого объекта
        int buildCount() const;
        QHostAddress startIp() const;
        QByteArray startMac() const;
        QHostAddress serverIp() const;
        BuildInitWidget* findWidgetByNumber(int num);
        QList<BuildInitWidget*> incompleteWidgets();

    private slots:
        void processBuildCountChanged(int count);
    public slots:
        virtual void accept() override;

};

#endif // ADDGUARDEDOBJECTDIALOG_H
