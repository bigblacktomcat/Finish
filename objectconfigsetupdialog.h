#ifndef OBJECTCONFIGSETUPDIALOG_H
#define OBJECTCONFIGSETUPDIALOG_H

#include "issodatabase.h"
#include "issoobjectcard.h"

#include <QDialog>

namespace Ui {
    class ObjectConfigSetupDialog;
}


class ObjectConfigSetupDialog : public QDialog
{
        Q_OBJECT

    public:
        explicit ObjectConfigSetupDialog(IssoDataBase* db,
                                         QWidget *parent = 0);
        ~ObjectConfigSetupDialog();

    private:
        Ui::ObjectConfigSetupDialog *ui;

        IssoDataBase* m_db;
        // [objectID : objectCard]
        QMap<int, IssoObjectCard> m_objCards;

        /// \brief  Проинициализировать диалог
        void init();
        /// \brief  Установить обработчики событий
        void initEventHandlers();
        /// \brief  Загрузить список карточек объектов из БД
        void loadObjCards();

    private slots:
        void processAddCfgBtnClicked();
        void processRemoveCfgBtnClicked();
        void processSaveSettingsBtnClicked();
        void processListConfigsSelectionChanged();

};

#endif // OBJECTCONFIGSETUPDIALOG_H
