#ifndef ISSOAUDIOCONTROLLER_H
#define ISSOAUDIOCONTROLLER_H

#include <QObject>
#include <QtConcurrent/QtConcurrent>

#include "issomodulereply.h"
#include "issomodulerequest.h"
#include "issorequestsender.h"
#include "mp3filereader.h"
#include "speexencoder.h"


class IssoAudioController : public QObject
{
        Q_OBJECT
    private:
        const int MAX_AUDIO_BLOCK_LEN = 240;

        QHostAddress m_ip;
        quint16 m_bindPort;
        volatile bool m_running = false;
        quint16 m_packNum = 0;

        Mp3FileReader* mp_mp3Reader = nullptr;
        SpeexEncoder* mp_encoder = nullptr;

        IssoModuleRequest makeRequest(const QHostAddress& ip, quint16 port,
                                      quint8 recordNum, quint8 totalBlocks,
                                      quint8 currentBlock, const QByteArray& audioData);
        void encodeAndSend(const QHostAddress &ip, quint16 port,
                           const QByteArray& pcmSamples);

    public:
        explicit IssoAudioController(const QHostAddress& ip, quint16 bindPort, int sampleRate,
                                     int channelCount, int sampleSize,
                                     QObject *parent = nullptr);
        ~IssoAudioController();

        void playModuleRecord(const QHostAddress &ip, quint16 port, quint8 recordNum);
        bool playModuleRecordAsync(const QHostAddress &ip, quint16 port, quint8 recordNum);

        void playMp3File(const QHostAddress &ip, quint16 port, const QString& fileName);
        bool playMp3FileAsync(const QHostAddress &ip, quint16 port, const QString& fileName);

        void playVoiceRecord(const QHostAddress &ip, quint16 port, const QByteArray& pcmSamples);
        bool playVoiceRecordAsync(const QHostAddress &ip, quint16 port, const QByteArray& pcmSamples);

        void stopPlaying();

    signals:
        void playbackRequestComplete(int recordNum);
        void playbackRequestFailed(int recordNum);

        void recordBlockSent(quint8 totalBlocks, quint8 currentBlock);
        void recordSendComplete();
        void recordSendFailed(quint8 totalBlocks, quint8 currentBlock);
};

#endif // ISSOAUDIOCONTROLLER_H
