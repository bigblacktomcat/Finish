/**
\brief Реализация класса Mp3FileReader воспроизведение mp3
Основан на библиотеке mpg123
\date  2020-08-18
**/

#include "mp3filereader.h"


Mp3FileReader::Mp3FileReader(int sampleRate, int channelCount,
                             int sampleSize, QObject *parent)
    : QObject(parent),
      m_sampleRate(sampleRate),
      m_channelCount(channelCount),
      m_sampleSize(sampleSize)
{
    // инициализировать mpg123
    initMpg123();
    // сформировать аудио формат
    QAudioFormat outFormat = makeAudioFormat(sampleRate, channelCount, sampleSize);
    // создать аудиовыход
    mp_audioOut = new QAudioOutput(outFormat, this);
}


Mp3FileReader::~Mp3FileReader()
{
    // снять обработчик воспроизведения
    setAudioOutEnabled(false);
    // закрыть аудиовыход
    if (mp_audioOut)
    {
        mp_audioOut->stop();
        delete mp_audioOut;
    }
    // деинициализировать mpg123
    uninitMpg123();
}


void Mp3FileReader::setAudioOutEnabled(bool audioOutEnabled)
{
    if (audioOutEnabled)
        // установить обработчик воспроизведения декодированных сэмплов
        connect(this, SIGNAL(frameRead(QByteArray)), SLOT(playAudioData(QByteArray)));
    else
        // снять обработчик воспроизведения
        disconnect(this, SIGNAL(frameRead(QByteArray)), 0, 0);
}


bool Mp3FileReader::isReading() const
{
    return m_reading;
}


void Mp3FileReader::initMpg123()
{
    // создать буфер для воспроизведения файла
    mp_decodedData = new QBuffer(this);
    // результат выполнения функции
    int ret = 0;
    // загрузить библиотеку
    mpg123_init();
    // создать дескриптор
    mp_mp3Handle = mpg123_new(NULL, &ret);
    // убедиться, что дескриптор создан
    if(mp_mp3Handle == NULL)
    {
        qDebug() << "Cannot create MPG123 handle: %s" << mpg123_plain_strerror(ret) << endl;
    }
//    else
//    {
//        qDebug() << "MPG123 handle created" << endl;
//    }

//    long lflags;
//    double dflags;
//    mpg123_getparam(mp_mp3Handle, MPG123_FLAGS, &lflags, &dflags);

    // задать скорость
    mpg123_param(mp_mp3Handle, MPG123_FORCE_RATE, m_sampleRate, 0);

//    mpg123_param(mp_mp3Handle, MPG123_FLAGS, lflags, dflags);

    // задать число каналов
    long channelMode = (m_channelCount == 1 ? MPG123_FORCE_MONO : MPG123_FORCE_STEREO);
    mpg123_param(mp_mp3Handle, MPG123_ADD_FLAGS, channelMode, 0);
}


void Mp3FileReader::uninitMpg123()
{
    if (mp_mp3Handle)
    {
        // удалить дескриптор
        mpg123_delete(mp_mp3Handle);
        // выгрузить библиотеку
        mpg123_exit();
        // занулить указатель
        mp_mp3Handle = nullptr;
    }
}


QAudioFormat Mp3FileReader::makeAudioFormat(int sampleRate, int channelCount, int sampleSize)
{
    QAudioFormat audioFormat;
    audioFormat.setSampleRate(sampleRate);
    audioFormat.setChannelCount(channelCount);
    audioFormat.setSampleSize(sampleSize);
    audioFormat.setCodec("audio/pcm");
    audioFormat.setByteOrder(QAudioFormat::LittleEndian);
    audioFormat.setSampleType(QAudioFormat::UnSignedInt);
    return audioFormat;
}


int Mp3FileReader::calculateFrameSize(int channelCount, int sampleSize)
{
    return (160 * channelCount * (sampleSize / 8));
}


void Mp3FileReader::readMp3FileAsync(const QString &fileName, bool outToSpeakers)
{
    QtConcurrent::run(this, &Mp3FileReader::readMp3File, fileName, outToSpeakers);
}


QByteArray Mp3FileReader::readMp3File(const QString &fileName, bool outToSpeakers)
{
    // если нужно воспроизводить декодированные фреймы,
    // задать обработчик
    if (outToSpeakers)
        setAudioOutEnabled(true);
    //
    m_reading = true;
    // буфер
    unsigned char *frameData = nullptr;
    size_t frameSize = 0;
    size_t bytesDecoded = 0;
    int res = MPG123_ERR;
    // настройки декодера
    int channels = 0;
    int encoding = 0;
    long rate = 0;
    //
    // открыть mp3-файл
    res = mpg123_open(mp_mp3Handle, fileName.toUtf8().data());
    // получить формат
    res = mpg123_getformat(mp_mp3Handle, &rate, &channels, &encoding);
    // сбросить формат
    res = mpg123_format_none(mp_mp3Handle);
    // задать целевой формат для преобразования сэмплов
    res = mpg123_format(mp_mp3Handle, m_sampleRate, 1, encoding);
    // получить формат
    res = mpg123_getformat(mp_mp3Handle, &rate, &channels, &encoding);

//    // получить размер сэмпла
//    int m_bytes = mpg123_encsize(encoding);
//    qDebug() << "sample size = " << m_bytes << endl;
//    // непонятные манипуляции, без которых не работает)))
//    int m_rate = rate;
//    int m_channels = channels;

//    qDebug() << QString("bytes = %1,     rate = %2,      channels = %3")
//                .arg(m_bytes).arg(m_rate).arg(m_channels) << endl;

//    // получить максимальный размер декодированного фрейма при заданных настройках
//    frameSize = mpg123_outblock(mp_mp3Handle);

    // задать размер декодированного фрейма при заданных настройках
    frameSize = calculateFrameSize(m_channelCount, m_sampleSize);
    // выделить память для фрейма
    frameData = (unsigned char*) malloc(frameSize * sizeof(unsigned char));

    // массив pcm-сэмплов
    QByteArray pcmSamples;
    // считать и декодировать каждый фрейм из mp3-файла
    for (int totalBytes = 0;
         mpg123_read(mp_mp3Handle, frameData, frameSize, &bytesDecoded) == MPG123_OK && m_reading; )
    {
        // число декодированных байт
        totalBytes += bytesDecoded;
        // сформировать фрейм
        QByteArray decodedFrame((char*)frameData, frameSize);
        // добавить фрейм в массив pcm-сэмплов
        pcmSamples.append(decodedFrame);
        // инициировать событие декодирования очередного фрейма
        emit frameRead(decodedFrame);
    }
    // освободить память буфера
    free(frameData);
    // закрыть источник
    mpg123_close(mp_mp3Handle);
    //
    // если декодированные данные воспроизводились,
    // снять обработчик
    if (outToSpeakers)
        setAudioOutEnabled(false);

    // вернуть массив pcm-сэмплов
    return pcmSamples;
}


void Mp3FileReader::stopReading()
{
    // сбросить флаг активности
    m_reading = false;
    // остановить воспроизведение
    mp_audioOut->stop();
}


void Mp3FileReader::playAudioData(const QByteArray &audioData)
{
    // добавить декодированные аудиоданные в буфер
    mp_decodedData->buffer().append(audioData);
    // если аудиовыход не воспроизводит звук,
    // воспроизвести данные буфера
    if (mp_audioOut->state() != QAudio::ActiveState)
    {
        mp_decodedData->open(QIODevice::ReadOnly);
        mp_audioOut->start(mp_decodedData);
    }
}
