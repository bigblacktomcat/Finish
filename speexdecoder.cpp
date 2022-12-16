#include "speexdecoder.h"


SpeexDecoder::SpeexDecoder(QObject *parent)
    : QObject(parent)
{
    init();
}


SpeexDecoder::~SpeexDecoder()
{
    uninit();
}


void SpeexDecoder::decodeData(const QByteArray &data)
{
    // копировать кодированные данные в битовый массив для декодирования
    speex_bits_read_from(&m_bits, data.data(), data.length());
    // создать массив для обработки кодеком speex
    short speexProcessArray[m_frameSize];
    // декодировать данные в промежуточный массив
    speex_decode_int(mp_state, &m_bits, speexProcessArray);
    // создать массив декодированных данных
    QByteArray decodedData;
    // создать поток для заполнения результирующего массива
    QDataStream stream(&decodedData, QIODevice::WriteOnly);
    // задать порядок следования байт
    stream.setByteOrder(QDataStream::LittleEndian);

    // копировать декодированные данные из промежуточного массива в результирующий
    for (int i = 0; i < m_frameSize; ++i)
        stream << (short)speexProcessArray[i];

    // инициировать событие
    emit frameDecoded(decodedData);

//    qDebug() << "Decoding data size = " << data.length();
//    qDebug() << "Decoded data size = " << decodedData.length();
//    qDebug() << "Decoded data: " << decodedData.toHex(' ') << endl;
}


void SpeexDecoder::init()
{
    // создать состояние декодировщика в узкополосном режиме
    mp_state = speex_decoder_init(&speex_nb_mode);
    // получить размер фрейма
    speex_decoder_ctl(mp_state, SPEEX_GET_FRAME_SIZE, &m_frameSize);
    // включить шумоподавление
    int enhanced = 1;
    speex_decoder_ctl(mp_state, SPEEX_SET_ENH, &enhanced);
    // инициализирвать структуру, хранящую биты
    speex_bits_init(&m_bits);

//    m_frameSize = 209;

//    // DEBUG
//    int bitRate;
//    speex_decoder_ctl(mp_state, SPEEX_GET_BITRATE, &bitRate);
//    int sampleRate;
//    speex_decoder_ctl(mp_state, SPEEX_GET_SAMPLING_RATE, &sampleRate);
//    qDebug() << "Decoder:";
//    qDebug() << "BitRate = " << bitRate;
//    qDebug() << "Sample Rate = " << sampleRate;
//    qDebug() << "Frame Size = " << m_frameSize << endl;
}


void SpeexDecoder::uninit()
{
    // уничтожить состояние декодировщика
    speex_decoder_destroy(mp_state);
    // уничтожить структуру, хранящую биты
    speex_bits_destroy(&m_bits);
}
