#include "speexencoder.h"


SpeexEncoder::SpeexEncoder(QObject *parent)
    : QObject(parent)
{
    init();
}


SpeexEncoder::~SpeexEncoder()
{
    uninit();
}


void SpeexEncoder::init()
{
    // создать состояние кодировщика в узкополосном режиме
    mp_state = speex_encoder_init(&speex_nb_mode);
    // установить качество 4
    int quality = 4;
    speex_encoder_ctl(mp_state, SPEEX_SET_QUALITY, &quality);
    // получить размер фрейма
    speex_encoder_ctl(mp_state, SPEEX_GET_FRAME_SIZE, &m_frameSize);
    // инициализирвать структуру, хранящую биты
    speex_bits_init(&m_bits);

//    m_frameSize = 209;

    // DEBUG
//    int bitRate;
//    speex_encoder_ctl(mp_state, SPEEX_GET_BITRATE, &bitRate);
//    int sampleRate;
//    speex_encoder_ctl(mp_state, SPEEX_GET_SAMPLING_RATE, &sampleRate);
//
//    qDebug() << "Encoder:";
//    qDebug() << "BitRate = " << bitRate;
//    qDebug() << "Sample Rate = " << sampleRate;
//    qDebug() << "Frame Size = " << m_frameSize << endl;
}


void SpeexEncoder::uninit()
{
    // уничтожить состояние кодировщика
    speex_encoder_destroy(mp_state);
    // уничтожить структуру, хранящую биты
    speex_bits_destroy(&m_bits);
}


QByteArray SpeexEncoder::encodeAudioData(const QByteArray &audioData)
{
    // создать поток для считывания аудиоданных для кодирования
    QByteArray ba = audioData;
    QDataStream stream(&ba, QIODevice::ReadOnly);
    // массив кодированных аудиоданных
    QByteArray encodedData;
    // пока длина потока не меньше длины фрейма, обрабатывать фрейм
    while(stream.device()->bytesAvailable() >= m_frameSize * sizeof(short))
    {
        // выделить текущий фрейм из начала потока
        QByteArray frameData(m_frameSize * sizeof(short), 0);
        stream.readRawData(frameData.data(), frameData.size());
        // кодировать фрейм
        QByteArray speexFrame = encodeFrame(frameData);
        // добавить кодированный фрейм в массив кодированных данных
        encodedData.append(speexFrame);
        // инициировать событие
        emit frameEncoded(speexFrame);
    }
    // вернуть массив кодированных данных
    return encodedData;
}


QByteArray SpeexEncoder::encodeFrame(const QByteArray &frameData)
{
    // создать массив для обработки кодеком speex
    float speexProcessArray[m_frameSize];
    // создать поток для преобразования данных фрейма
    QDataStream stream(frameData);
    // задать порядок следования байт
    stream.setByteOrder(QDataStream::LittleEndian);

    // переменные для считывания данных в массив для обработки
    short shortSample;
    int i = 0;
    // считать данные в массив
    while(stream.device()->bytesAvailable() >= sizeof(short))
    {
        // считать сэмпл
        stream >> shortSample;
        // добавить в массив для обработки кодеком
        speexProcessArray[i++] = shortSample;
    }
    // подготовить битовый массив к кодированию
    speex_bits_reset(&m_bits);
    // кодировать фрейм
    speex_encode(mp_state, speexProcessArray, &m_bits);
    // определить количество кодированных байт
    int bytesEncoded = speex_bits_nbytes(&m_bits);
    // создать массив для копирования кодированных данных
    QByteArray encodedData(bytesEncoded, 0);
    // копировать кодированные данные в массив char
    int bytesWritten = speex_bits_write(&m_bits, encodedData.data(), encodedData.length());
    // установить длину массива = числу записанных данных
    if (bytesWritten != bytesEncoded)
        encodedData.resize(bytesWritten);

//    // инициировать событие
//    emit frameEncoded(encodedData);

    // вернуть кодированный аудио фрейм
    return encodedData;
}
