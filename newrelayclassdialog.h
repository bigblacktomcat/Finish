/**
\brief Описание диалогового класса NewRelayClassDialog
Для нового класса реле задаём имя, описание, задержку активации
\date  2020-08-18
**/
#ifndef NEWRELAYCLASSDIALOG_H
#define NEWRELAYCLASSDIALOG_H

#include <QDialog>
#include <QPushButton>

namespace Ui {
    class NewRelayClassDialog;
}

class NewRelayClassDialog : public QDialog
{
        Q_OBJECT

    public:
        explicit NewRelayClassDialog(QWidget *parent = 0);
        ~NewRelayClassDialog();

        /// \brief  Имя класса реле
        QString relayClassName() const;
        /// \brief  Описание класса реле
        QString description() const;
        /// \brief  Задержка активации
        int delay() const;


    private:
        Ui::NewRelayClassDialog *ui;
};

#endif // NEWRELAYCLASSDIALOG_H
