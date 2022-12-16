#ifndef ISSOREMOTELOGGER_H
#define ISSOREMOTELOGGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include "issocommondata.h"


namespace IssoServerConst
{
    const QString SERVER_URL = "%1/logger.php";
    const QString REQUEST_GET_ALL_EVENTS = SERVER_URL + "?method=getGscLog";
    const QString REQUEST_GET_LAST_EVENT = SERVER_URL + "?method=getGscLog&lastMsg=%2";

    const QString QUERY_KEY_METHOD = "method";
    const QString QUERY_KEY_DATA = "data";

    const QString QUERY_VALUE_ADD_MSG = "addMessage";
    const QString QUERY_VALUE_CLEAR_LOG = "clearLog";
    const QString QUERY_VALUE_UPDATE_OBJ = "updateObject";
}


class IssoRemoteLogger : public QObject
{
        Q_OBJECT

    private:
//        const QString REQUEST_GET_ALL_EVENTS = "%1/logger.php?method=getGscLog";
//        const QString REQUEST_GET_LAST_EVENT = "%1/logger.php?method=getGscLog&lastMsg=%2";

//        const QString REQUEST_ADD_TO_LOG = "%1/logger.php?method=addMessage&data=%2";
//        const QString REQUEST_CLEAR_LOG = "%1/logger.php?method=clearLog";
//        const QString REQUEST_SEND_PHOTO = "%1/logger.php?method=saveCamShot&uid=%2&blob=%3";

//        const QString REQUEST_GET_LOG_UPDATES = "%1/issoLogPoller.php?src=%2";
//        const QString ARG_SRC_GSC = "gsc";
//        const QString ARG_SRC_SOO = "soo";

        QString m_url;
        QNetworkAccessManager* mp_NetManager;

        void sendPostRequest(const QUrlQuery& params);

    public:
        explicit IssoRemoteLogger(const QString& url = "", QObject *parent = nullptr);
        ~IssoRemoteLogger();

        /// \brief  Задать имя сервера
        void setUrl(const QString& url);
        /// \brief  Добавить сообщение в журнал событий на удаленном сервере
        void addMessage(const QString& msg);
        /// \brief  Очистить журнал событий на удаленном сервере
        void clearLog();
        /// \brief  Обновить конфигурацию объекта на удаленном сервере
        void addObject(const QString& config);

//        /// \brief  Отправить фото события на сервер
//        void sendEventPhoto(int id, const QString& base64Data);
        /// \brief  Запросить обновления журнала событий на удаленном сервере
        void getLogUpdates(IssoAppType sourceApp, int lastEventId = -1);
    signals:
        /// \brief  Известить об успешной отправке сообщения
        void requestCompleted(const QString& replyMsg);
        /// \brief  Известить об ошибке отправки сообщения
        void requestFailed(const QString& errorMsg);

    private slots:
        /// \brief Обработчик ответа от сервера
        void processReply(QNetworkReply* pReply);

};

#endif // ISSOREMOTELOGGER_H
