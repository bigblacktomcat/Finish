#ifndef AUDIOMESSAGEDIALOG_H
#define AUDIOMESSAGEDIALOG_H

#include <QDialog>
#include <QMap>
#include <QPushButton>
#include <QButtonGroup>
#include <QDir>
#include <QTime>

#include "microphonecapture.h"
#include "issocommondata.h"


namespace Ui {
    class AudioMessageDialog;
}


enum AudioMsgType
{
    VOICE,
    MODULE_REC,
    PC_REC
};


class AudioMessageDialog : public QDialog
{
        Q_OBJECT
    private:
        Ui::AudioMessageDialog *ui;
        QButtonGroup m_radioButtons;

        MicrophoneCapture* mp_capture;
        QByteArray m_voiceRecord;
        QTime m_recordTime;

        void initWidgets();
        void initRecords();
        void initMicCapture();
        void uninitMicCapture();

    public:
        explicit AudioMessageDialog(QWidget *parent = 0);
        ~AudioMessageDialog();

        AudioMsgType audioMsgType() const;
        quint8 recordNumber() const;
        QString recordFileName() const;
        QByteArray voiceRecord() const;

    private slots:
        void processSamplesRead(const QByteArray& samples);
        void processMsgTypeChanged();
        void startRecord();
        void stopRecord();
};

#endif // AUDIOMESSAGEDIALOG_H
