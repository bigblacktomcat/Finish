/**
\brief Описание диалогового класса NewCameraDialog
Для новой камеры задаём идентификатор, адрес порт, пароль, номер камеры, номер этажа
\date  2020-08-18
**/

#ifndef NEWCAMERADIALOG_H
#define NEWCAMERADIALOG_H

#include "issobuilding.h"

#include <QDialog>
#include <QHostAddress>
#include <QPushButton>

namespace Ui {
    class NewCameraDialog;
}

class NewCameraDialog : public QDialog
{
        Q_OBJECT
    private:
        Ui::NewCameraDialog *ui;

        void init(const QString& title);

    public:
        explicit NewCameraDialog(const QList<int>& availableIds,
                                 const QList<IssoBuilding *> &buildings,
                                 QWidget *parent = 0);
        ~NewCameraDialog();

        /// \brief  Идентификатор камеры
        int id() const;
        /// \brief  IP-адрес камеры
        QHostAddress ip() const;
        /// \brief  Порт камеры
        quint16 port() const;
        /// \brief  Пароль к камере
        QString password() const;
        /// \brief  Номер здания
        int buildNumber() const;
        /// \brief  Номер этажа
        int floorNumber() const;
};

#endif // NEWCAMERADIALOG_H
