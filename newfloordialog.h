/**
\brief Описание диалогового класса NewFloorDialog
Для нового этажа задаём идентификатор, графический план
\date  2020-08-18
**/
#ifndef NEWFLOORDIALOG_H
#define NEWFLOORDIALOG_H

#include <QDialog>

namespace Ui {
    class NewFloorDialog;
}

class NewFloorDialog : public QDialog
{
        Q_OBJECT
    private:
        QPixmap m_pixmap;
        bool m_ok = false;

    public:
        explicit NewFloorDialog(QList<int> availableNumbers, QWidget *parent = 0);
        explicit NewFloorDialog(int currentNumber, QWidget *parent = 0);
        ~NewFloorDialog();

        void init();
//        QString name() const;
        int number() const;
        QPixmap pixmap(bool& ok) const;

    private slots:
        void on_btnSelectFloorPath_clicked();

    private:
        Ui::NewFloorDialog *ui;
};

#endif // NEWFLOORDIALOG_H
