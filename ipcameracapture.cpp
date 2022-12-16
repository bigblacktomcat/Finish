#include "ipcameracapture.h"

using namespace cv;

IpCameraCapture::IpCameraCapture(QObject *parent)
    : QObject(parent),
      m_running(false)
{
    // обработчики завершения цикла обмена командами
    mp_watcher = new QFutureWatcher<void>();
}


IpCameraCapture::~IpCameraCapture()
{
    // остановить захват
    stop();
    // дождаться завершения
    waitForStopped();
    // уничтожить наблюдателя
    delete mp_watcher;
    //
    qDebug() << "~IpCameraCapture" << endl;
}


void IpCameraCapture::start(const QHostAddress& ip, const QString& password)
{
    // если захват видео уже запущен, выход
    if (m_running)
        return;
    //
    m_running = true;
    // запустить цикл захвата кадров в отдельном потоке
    QFuture<void> loopFuture =
            QtConcurrent::run(this, &IpCameraCapture::runCaptureLoop, ip, password);
    // установить наблюдателя за потоком
    mp_watcher->setFuture(loopFuture);
}


void IpCameraCapture::stop()
{
    m_running = false;
}


void IpCameraCapture::waitForStopped()
{
    mp_watcher->waitForFinished();
}


bool IpCameraCapture::isRunning() const
{
    return m_running;
}


void IpCameraCapture::runCaptureLoop(const QHostAddress& ip,
                                     const QString& password)
{
    //
    qDebug() << tr("Попытка открыть видеопоток") << endl;
    //
    // подготовить url
    QString url = QString("rtsp://%1/user=admin_password=%2_channel=1_stream=0.sdp?real_stream")
                            .arg(ip.toString())
                            .arg(password);
//    QString url = "rtsp://192.168.1.200:554/video.pro1";

    // открыть видео поток для захвата
    VideoCapture cap(url.toStdString());
    if (!cap.isOpened())
    {
        //
        qDebug() << tr("Ошибка открытия видеопотока") << endl;
        //
        emit openStreamFailed(ip);

        m_running = false;
        return;
    }
    // известить о запуске захвата
    emit started(ip);

    // счетчик ошибок чтения кадра
    int failCount = 0;
    // начать захват
    while(m_running)
    {
        Mat cvImage;
        // захватить кадр
        if (!cap.read(cvImage))
        {
            // увеличить счетчик ошибок
            failCount++;
            //
            qDebug() << tr("Не удалось получить видеокадр: %1").arg(failCount) << endl;
            //
            waitKey(10);

            // если достигнут лимит ошибок
            if (failCount >= 10)
            {
                // известить об ошибке чтения потока
                emit readStreamFailed(ip);
                // сбросить признак выполнения
                m_running = false;
                // прервать цикл
                break;
            }
            continue;
        }
        // обнулить счетчик ошибок чтения
        failCount = 0;
        // преобразовать Mat -> QImage
//        QImage img(cvImage.data, cvImage.cols, cvImage.rows,
//                   cvImage.step1(), QImage::Format_RGB888);
        QImage img = QImage(cvImage.data, cvImage.cols, cvImage.rows,
                            cvImage.step1(), QImage::Format_RGB888).rgbSwapped();
        // копировать изображение
        QImage frame = img.copy(img.rect());
        // инициировать событие
        emit frameCaptured(frame);
    }
    // известить об остановке захвата
    emit stopped(ip);
}

