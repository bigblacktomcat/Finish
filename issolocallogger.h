#ifndef ISSOLOCALLOGGER_H
#define ISSOLOCALLOGGER_H

#include <QFile>
#include <QObject>
#include <QTextStream>


class IssoLocalLogger : public QObject
{
        Q_OBJECT
    private:
        QFile* m_eventFile;
        QFile* m_commentFile;
        QTextStream* m_eventStream = nullptr;
        QTextStream* m_commentStream = nullptr;

    public:
        explicit IssoLocalLogger(const QString& eventFileName,
                                 const QString& commentFileName,
                                 QObject *parent = nullptr);
        ~IssoLocalLogger();

        void logEvent(const QString& eventMsg);
        void logComment(const QString& comment);

    signals:

    public slots:
};

#endif // ISSOLOCALLOGGER_H
