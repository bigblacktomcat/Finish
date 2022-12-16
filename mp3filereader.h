/**
\brief Описание класса Mp3FileReader воспроизведение mp3
Основан на библиотеке mpg123
\date  2020-08-18
**/
#ifndef MP3FILEREADER_H
#define MP3FILEREADER_H

#include <QAudioOutput>
#include <QObject>
#include <QtConcurrent/QtConcurrent>
#include "mpg123.h"


class Mp3FileReader : public QObject
{
        Q_OBJECT
    private:
        int m_sampleRate;
        int m_channelCount;
        int m_sampleSize;

        mpg123_handle* mp_mp3Handle = nullptr;
        QBuffer* mp_decodedData = nullptr;
        QAudioOutput* mp_audioOut;

//        bool m_playback;
        volatile bool m_reading = false;

        void initMpg123();
        void uninitMpg123();

        QAudioFormat makeAudioFormat(int sampleRate, int channelCount, int sampleSize);
        int calculateFrameSize(int channelCount, int sampleSize);

    public:
        explicit Mp3FileReader(int sampleRate, int channelCount, int sampleSize,
//                               bool playback,
                               QObject *parent = nullptr);
        ~Mp3FileReader();

        void readMp3FileAsync(const QString& fileName, bool outToSpeakers);
        QByteArray readMp3File(const QString& fileName, bool outToSpeakers);
        void stopReading();

        void setAudioOutEnabled(bool getAudioOutEnabled);

        bool isReading() const;

    signals:
        void frameRead(const QByteArray &sample);

    public slots:
        void playAudioData(const QByteArray &audioData);
};

#endif // MP3FILEREADER_H
