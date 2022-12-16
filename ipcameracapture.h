#ifndef IPCAMERACAPTURE_H
#define IPCAMERACAPTURE_H

#include <QObject>
#include <QImage>
#include <QFutureWatcher>
#include <QHostAddress>
#include <QtConcurrent/QtConcurrent>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>


//using namespace cv;


class IpCameraCapture : public QObject
{
        Q_OBJECT
    private:
        volatile bool m_running;
        QFutureWatcher<void>* mp_watcher;

    public:
        explicit IpCameraCapture(QObject *parent = nullptr);
        ~IpCameraCapture();

        void waitForStopped();

        bool isRunning() const;

    signals:
        void started(const QHostAddress& ip);
        void stopped(const QHostAddress& ip);
        void openStreamFailed(const QHostAddress& ip);
        void readStreamFailed(const QHostAddress& ip);
        void frameCaptured(const QImage& frame);

    private slots:
        void runCaptureLoop(const QHostAddress& ip, const QString &password);

    public slots:
        void start(const QHostAddress& ip, const QString &password);
        void stop();
};

#endif // IPCAMERACAPTURE_H
