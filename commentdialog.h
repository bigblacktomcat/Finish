/**
комментарии к событиям
*/

#ifndef COMMENTDIALOG_H
#define COMMENTDIALOG_H

#include <QDialog>

namespace Ui {
    class CommentDialog;
}

class CommentDialog : public QDialog
{
        Q_OBJECT

    public:
        explicit CommentDialog(const QString& date, const QString &time,
                               const QString& moduleName,
                               const QString& state,
                               const QString& sensorName,
                               const QString& comment,
                               QWidget *parent = 0);
        ~CommentDialog();

        QString comment() const;

    private:
        Ui::CommentDialog *ui;
};

#endif // COMMENTDIALOG_H
