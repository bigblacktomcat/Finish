#include "issocommondata.h"
#include "issovideoframeprocessor.h"
#include <QtConcurrent/QtConcurrent>


IssoVideoFrameProcessor::IssoVideoFrameProcessor(QObject *parent)
    : QObject(parent)
{
    initSourceInfo();
    m_watcher = new QFutureWatcher<void>();
}


IssoVideoFrameProcessor::~IssoVideoFrameProcessor()
{
    if (m_watcher)
    {
        // если задача запущена, ожидать завершения
        if (m_watcher->isRunning())
            m_watcher->future().waitForFinished();
        // уничтожить наблюдателя и обнулить указатель
        delete m_watcher;
        m_watcher = nullptr;
    }
}


void IssoVideoFrameProcessor::initSourceInfo(QDateTime dateTime, const QString &moduleName,
                                             int multiSensorId, int cameraId, int cameraIp)
{
    m_dateTime = dateTime;
    m_moduleName = moduleName;
    m_multiSensorId = multiSensorId;
    m_cameraId = cameraId;
    m_cameraIp = cameraIp;
}


void IssoVideoFrameProcessor::runThread()
{
    m_running = true;
    // пока поток выполняется или очередь не пуста
    while (m_running || !m_queue.isEmpty())
    {
        // если очередь пуста
        if (m_queue.isEmpty())
        {
            QThread::currentThread()->msleep(200);
            continue;
        }
        // заблокировать очередь
        m_queueLock.lock();
        // получить видеокадр
        auto frame = m_queue.dequeue();
        // разблокировать очередь
        m_queueLock.unlock();
        //
        // инициировать событие готовности данных видеокадра
        emit cameraSnapshotReady(m_dateTime, m_moduleName, m_multiSensorId,
                                 m_cameraId, m_cameraIp, frame);
    }
    // обнулить инфо об источнике
    initSourceInfo();
}


bool IssoVideoFrameProcessor::start(QDateTime dateTime, const QString &moduleName,
                                    int multiSensorId, int cameraId, int cameraIp)
{
    // если поток выполняется, запретить запуск нового потока
    if (m_watcher->isRunning())
        return false;
    //
    // проинициализировать инфо об источнике
    initSourceInfo(dateTime, moduleName, multiSensorId, cameraId, cameraIp);
    //
    // запустить цикл обмена командами в отдельном потоке
    QFuture<void> loopFuture = QtConcurrent::run(this, &IssoVideoFrameProcessor::runThread);
    // установить наблюдателя за потоком
    m_watcher->setFuture(loopFuture);
    //
    return true;
}


void IssoVideoFrameProcessor::stop()
{
    // завершить опрос
    m_running = false;
}


bool IssoVideoFrameProcessor::isRunning()
{
    return m_running;
}


void IssoVideoFrameProcessor::enqueueFrame(const QImage &frame)
{
    if (!m_running)
        return;
    // заблокировать
    QMutexLocker locker(&m_queueLock);
    // добавить видеокадр в очередь
    m_queue.enqueue(frame);
}
