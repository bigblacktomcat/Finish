#ifndef CAMERAVIEWFRAME_H
#define CAMERAVIEWFRAME_H

#include <QFrame>
#include <QHostAddress>

namespace Ui {
    class CameraViewFrame;
}

class CameraViewFrame : public QFrame
{
        Q_OBJECT

    private:
        Ui::CameraViewFrame *ui;

    protected:
        virtual void closeEvent(QCloseEvent *event);
        virtual void showEvent(QShowEvent *event);

    public:
        explicit CameraViewFrame(QWidget *parent = 0);
        ~CameraViewFrame();

    signals:
        void closed();

    public slots:
        void displayFrame(const QImage &frame);
        void show(const QString &title);
};

#endif // CAMERAVIEWFRAME_H
