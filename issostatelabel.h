#ifndef ISSOSTATELABEL_H
#define ISSOSTATELABEL_H

#include <QLabel>
#include <QTimer>
#include "issocommondata.h"


class IssoStateLabel : public QLabel
{
        Q_OBJECT
    private:
        IssoState m_state;
        QTimer m_blinkTimer;

        QPalette makePalette(QColor textColor, QColor fillColor);

        void startBlinking(const QString& blinkStyle1,
                           const QString& blinkStyle2,
                           int interval = 300);
        void stopBlinking();

    public:
        explicit IssoStateLabel(QWidget *parent = nullptr);

        void setState(IssoState state, const QString &title = "");
    signals:

    public slots:
};

#endif // ISSOSTATELABEL_H
