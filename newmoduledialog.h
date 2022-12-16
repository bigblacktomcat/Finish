/**
\brief Описание диалогового класса NewModuleDialog
Для нового модуля задаём идентификатор, IP-адрес, порт
\date  2020-08-18
**/
#ifndef NEWMODULEDIALOG_H
#define NEWMODULEDIALOG_H

#include <QDialog>
#include <QHostAddress>
#include <QPushButton>


namespace Ui {
    class NewModuleDialog;
}


class NewModuleDialog : public QDialog
{
        Q_OBJECT
    public:
        explicit NewModuleDialog(QWidget *parent = 0);
        ~NewModuleDialog();

        /// \brief  Уникальный идентификатор модуля
        int uniqueId() const;
        void setUniqueId(int id);
        /// \brief  Отображаемый идентификатор модуля
        int displayId() const;
        void setDisplayId(int id);
        /// \brief  IP-адрес модуля
        QHostAddress ip() const;
        void setIp(const QHostAddress& ip);
        /// \brief  Порт модуля
        quint16 port() const;
        void setPort(quint16 port);

    private:
        Ui::NewModuleDialog *ui;

        void init();
        void initTabOrder();
};

#endif // NEWMODULEDIALOG_H
