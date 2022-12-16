#include "issoremotelogger.h"

#include <QFile>
#include <QMessageBox>
#include <QTextCodec>
#include <QUrlQuery>

using namespace IssoServerConst;


IssoRemoteLogger::IssoRemoteLogger(const QString& url, QObject *parent) :
    QObject(parent)/*,
    m_url(url)*/
{
    setUrl(url);
    mp_NetManager = new QNetworkAccessManager(this);
    // задать обработчик ответа
    connect(mp_NetManager,  &QNetworkAccessManager::finished,
            this,           &IssoRemoteLogger::processReply);
}


IssoRemoteLogger::~IssoRemoteLogger()
{
    qDebug() << "~IssoRemoteLogger";
}


void IssoRemoteLogger::setUrl(const QString &url)
{
//    m_url = url;
//    m_url = QString("%1/logger.php").arg(url);
    m_url = QString(SERVER_URL).arg(url);
}


void IssoRemoteLogger::sendPostRequest(const QUrlQuery &params)
{
    QUrl url(m_url);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    mp_NetManager->post(request, params.query(QUrl::FullyEncoded).toUtf8());
}


void IssoRemoteLogger::addMessage(const QString& msg)
{
    // сформировать строку запроса (параметры)
    QUrlQuery params;
    params.addQueryItem(QUERY_KEY_METHOD, QUERY_VALUE_ADD_MSG);
    params.addQueryItem(QUERY_KEY_DATA, msg);
    // отправить
    sendPostRequest(params);
//    QUrl url(m_url);
//    QNetworkRequest request(url);
//    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
//    mp_NetManager->post(request, params.query(QUrl::FullyEncoded).toUtf8());
}


void IssoRemoteLogger::clearLog()
{
    // сформировать строку запроса (параметры)
    QUrlQuery params;
    params.addQueryItem(QUERY_KEY_METHOD, QUERY_VALUE_CLEAR_LOG);
    // отправить
    sendPostRequest(params);
//    mp_NetManager->post(QNetworkRequest(QUrl(m_url)),
//                        params.query(QUrl::FullyDecoded).toUtf8());
}


void IssoRemoteLogger::addObject(const QString &config)
{
    // сформировать строку запроса (параметры)
    QUrlQuery params;
    params.addQueryItem(QUERY_KEY_METHOD, QUERY_VALUE_UPDATE_OBJ);
    params.addQueryItem(QUERY_KEY_DATA, config);
    // отправить
    sendPostRequest(params);
//    QUrl url(m_url);
//    QNetworkRequest request(url);
//    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
//    mp_NetManager->post(request, params.query(QUrl::FullyEncoded).toUtf8());
}


//void IssoRemoteLogger::sendEventPhoto(int id, const QString &base64Data)
//{
//    // сформировать строку запроса (параметры)
//    QUrlQuery params;
//    params.addQueryItem("method", "saveCamShot");
//    params.addQueryItem("uid", QString::number(id));
//    params.addQueryItem("blob", base64Data);
//    // отправить
//    mp_NetManager->post(QNetworkRequest(QUrl(m_url)),
//                        params.query(QUrl::FullyDecoded).toUtf8());
////    // отладка
////    QFile file("base64Data.txt");
////    if (file.open(QIODevice::ReadWrite))
////    {
////        QTextStream stream(&file);
////        stream << base64Data;
////    }
//}


void IssoRemoteLogger::getLogUpdates(IssoAppType sourceApp, int lastEventId)
{
    QString cmd = "";
    // сформировать команду
    switch(sourceApp)
    {
        // ГСЦ
        case IssoAppType::GSC:
        {
            // если -1, запросить все новые события,
            // иначе - событие с заданным ID
            if (lastEventId == -1)
                cmd = REQUEST_GET_ALL_EVENTS.arg(m_url);
            else
                cmd = REQUEST_GET_LAST_EVENT.arg(m_url).arg(lastEventId);
            break;
        }
        // СОО
        case IssoAppType::SOO:
        {
            //
            break;
        }
        default:
            break;
    }
    // если команда не пуста, отправить запрос
    if (!cmd.isEmpty())
    {
        // отправить
        mp_NetManager->get(QNetworkRequest(QUrl(cmd)));
    }
}


void IssoRemoteLogger::processReply(QNetworkReply* pReply)
{
    // проверить наличие ошибок
    bool failed = (pReply->error() != QNetworkReply::NoError);
    // инициировать соответствующее событие
    if (failed)
    {
        // инициировать событие ошибки запроса
        emit requestFailed(pReply->errorString());
    }
    else
    {
        // вычитать данные ответа
        QByteArray content = pReply->readAll();
        // преобразовать данные в UTF-8
        QString replyMsg = QString::fromUtf8(content);
//        QTextCodec* codec = QTextCodec::codecForName("utf8");
//        QString replyMsg = codec->toUnicode(content.data());
        // инициировать событие получения ответа на запрос
        emit requestCompleted(replyMsg);

//        qDebug() << replyMsg << endl;
    }
    // удалить ответ
    pReply->deleteLater();
}
