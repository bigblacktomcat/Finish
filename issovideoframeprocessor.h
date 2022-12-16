#ifndef ISSOVIDEOFRAMEPROCESSOR_H
#define ISSOVIDEOFRAMEPROCESSOR_H

#include <QFutureWatcher>
#include <QMutex>
#include <QObject>
#include <QQueue>
#include <QImage>
#include <QDateTime>


class IssoVideoFrameProcessor : public QObject
{
        Q_OBJECT
    private:
        QDateTime m_dateTime;
        QString m_moduleName;
        int m_multiSensorId;
        int m_cameraId;
        int m_cameraIp;

        QQueue<QImage> m_queue;
        QMutex m_queueLock;
        volatile bool m_running = false;
        QFutureWatcher<void>* m_watcher = nullptr;


        void initSourceInfo(QDateTime dateTime = QDateTime(), const QString& moduleName = "",
                            int multiSensorId = 0, int cameraId = 0, int cameraIp = 0);
        void runThread();
    public:
        explicit IssoVideoFrameProcessor(QObject *parent = nullptr);
        ~IssoVideoFrameProcessor();

        bool start(QDateTime dateTime, const QString& moduleName,
                   int multiSensorId, int cameraId, int cameraIp);
        void stop();
        bool isRunning();
        void enqueueFrame(const QImage& frame);

    signals:
        void cameraSnapshotReady(const QDateTime& dateTime, const QString& moduleName,
                                 int multiSensorId, int cameraId, int cameraIp,
                                 const QImage& frame);

};

#endif // ISSOVIDEOFRAMEPROCESSOR_H
