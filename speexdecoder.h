#ifndef SPEEXDECODER_H
#define SPEEXDECODER_H

#include <QObject>
#include <QDataStream>
#include <QDebug>
#include "speex.h"
#include "speex_bits.h"


class SpeexDecoder : public QObject
{
        Q_OBJECT
    private:
        void* mp_state;
        SpeexBits m_bits;
        int m_frameSize;

        void init();
        void uninit();

    public:
        explicit SpeexDecoder(QObject *parent = nullptr);
        ~SpeexDecoder();

    signals:
        void frameDecoded(const QByteArray &decodedData);

    public slots:
        void decodeData(const QByteArray& data);
};

#endif // SPEEXDECODER_H
