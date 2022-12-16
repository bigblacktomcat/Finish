#ifndef NEWSENSORTYPEDIALOG_H
#define NEWSENSORTYPEDIALOG_H

#include <QDialog>

namespace Ui {
    class NewSensorTypeDialog;
}

class NewSensorTypeDialog : public QDialog
{
        Q_OBJECT

    public:
        explicit NewSensorTypeDialog(QWidget *parent = 0);
        ~NewSensorTypeDialog();

        QString sensorTypeName() const;
        float minValue() const;
        float maxValue() const;

    private:
        Ui::NewSensorTypeDialog *ui;
};

#endif // NEWSENSORTYPEDIALOG_H
