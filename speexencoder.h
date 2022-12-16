#ifndef SPEEXENCODER_H
#define SPEEXENCODER_H

#include <QObject>
#include <QDataStream>
#include <QDebug>
#include "speex.h"
#include "speex_bits.h"


class SpeexEncoder : public QObject
{
        Q_OBJECT
    private:
        void* mp_state;
        SpeexBits m_bits;
        int m_frameSize;

        void init();
        void uninit();

        QByteArray encodeFrame(const QByteArray& frameData);

    public:
        explicit SpeexEncoder(QObject *parent = nullptr);
        ~SpeexEncoder();

    signals:
        void frameEncoded(const QByteArray &encodedData);

    public slots:
        QByteArray encodeAudioData(const QByteArray& audioData);
};

#endif // SPEEXENCODER_H
