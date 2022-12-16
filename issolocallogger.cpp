#include "issolocallogger.h"


IssoLocalLogger::IssoLocalLogger(const QString &eventFileName,
                                 const QString &commentFileName, QObject *parent)
    : QObject(parent)
{
    // фалй событий
    m_eventFile = new QFile(eventFileName);
    // если файл открыт, создать поток для записи событий
    if (m_eventFile->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
        m_eventStream = new QTextStream(m_eventFile);
    // файл комментариев к событиям
    m_commentFile = new QFile(commentFileName);
    // если файл открыт, создать поток для записи комментариев
    if (m_commentFile->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
        m_commentStream = new QTextStream(m_commentFile);
}


IssoLocalLogger::~IssoLocalLogger()
{
    // уничтожить потоки
    if (m_eventStream)
        delete m_eventStream;
    if (m_commentStream)
        delete m_commentStream;
    // закрыть файлы
    if (m_eventFile)
    {
        m_eventFile->close();
        m_eventFile->deleteLater();
    }
    if (m_commentFile)
    {
        m_commentFile->close();
        m_commentFile->deleteLater();
    }
}


void IssoLocalLogger::logEvent(const QString &eventMsg)
{
    if (m_eventStream)
        *m_eventStream << eventMsg << endl;
}


void IssoLocalLogger::logComment(const QString &comment)
{
    if (m_commentStream)
        *m_commentStream << comment << endl;
}
