#ifndef MICROPHONECAPTURE_H
#define MICROPHONECAPTURE_H

#include <QObject>
#include <QAudioFormat>
#include <QAudioDeviceInfo>
#include <QAudioInput>
#include <QDebug>


class MicrophoneCapture : public QObject
{
        Q_OBJECT
    private:
        QAudioFormat m_audioFormat;
        QAudioInput* mp_input = nullptr;
        QIODevice* mp_inputBuffer = nullptr;

    public:
        explicit MicrophoneCapture(int sampleRate = 44100,
                                   int channelCount = 2,
                                   int sampleSize = 16,
                                   const QString& codec = "audio/pcm",
                                   QAudioFormat::Endian byteOrder = QAudioFormat::LittleEndian,
                                   QAudioFormat::SampleType sampleType = QAudioFormat::UnSignedInt,
                                   QObject *parent = nullptr);
        explicit MicrophoneCapture(const QAudioFormat &audioFormat,
                                   QObject *parent = nullptr);
        ~MicrophoneCapture();


        void setAudioFormat(int sampleRate, int channelCount, int sampleSize,
                            const QString& codec, QAudioFormat::Endian byteOrder,
                            QAudioFormat::SampleType sampleType);
        void setAudioFormat(const QAudioFormat &audioFormat);
        QAudioFormat audioFormat() const;

        void start(int captureInterval = 20, qreal volume = 1.0);
        void stop();

    signals:
        void samplesRead(const QByteArray& samples);

    private slots:
        void readInputDevice();
};

#endif // MICROPHONECAPTURE_H
