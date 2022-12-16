/**
\brief Реализация класса MicrophoneCapture звукозаписи с микрофона
\date  2020-08-18
**/

#include "microphonecapture.h"


MicrophoneCapture::MicrophoneCapture(int sampleRate, int channelCount,
                                     int sampleSize, const QString &codec,
                                     QAudioFormat::Endian byteOrder,
                                     QAudioFormat::SampleType sampleType,
                                     QObject *parent)
    : QObject(parent)
{
    setAudioFormat(sampleRate, channelCount, sampleSize, codec, byteOrder, sampleType);
}


MicrophoneCapture::MicrophoneCapture(const QAudioFormat &audioFormat, QObject *parent)
    : QObject(parent)
{
    setAudioFormat(audioFormat);
}


MicrophoneCapture::~MicrophoneCapture()
{
    stop();
}


QAudioFormat MicrophoneCapture::audioFormat() const
{
    return m_audioFormat;
}


void MicrophoneCapture::setAudioFormat(const QAudioFormat &audioFormat)
{
    m_audioFormat = audioFormat;
}


void MicrophoneCapture::setAudioFormat(int sampleRate, int channelCount,
                                    int sampleSize, const QString &codec,
                                    QAudioFormat::Endian byteOrder,
                                    QAudioFormat::SampleType sampleType)
{
    m_audioFormat.setSampleRate(sampleRate);
    m_audioFormat.setChannelCount(channelCount);
    m_audioFormat.setSampleSize(sampleSize);
    m_audioFormat.setCodec(codec);
    m_audioFormat.setByteOrder(byteOrder);
    m_audioFormat.setSampleType(sampleType);
}


void MicrophoneCapture::start(int captureInterval, qreal volume)
{
    // получить данные микрофона (line-in)
    QAudioDeviceInfo deviceInfo = QAudioDeviceInfo::defaultInputDevice();
    // убедиться, что установленный аудиоформат поддерживается устройством
    if(!deviceInfo.isFormatSupported(m_audioFormat))
    {
        qWarning() << tr("Формат не поддерживается устройством, "
                         "попробуем использовать ближайший");
        m_audioFormat = deviceInfo.nearestFormat(m_audioFormat);
    }
    // создать устройство аудиозахвата
    mp_input = new QAudioInput(deviceInfo, m_audioFormat, this);
    // установить громкость
    mp_input->setVolume(volume);
    // установить интервал захвата сэмплов
    mp_input->setNotifyInterval(captureInterval);
    // установить обработчик готовности сэмплов
    connect(mp_input, SIGNAL(notify()), SLOT(readInputDevice()));
    // начать захват в буфер
    mp_inputBuffer = mp_input->start();
}


void MicrophoneCapture::stop()
{
    if (!mp_input)
        return;
    // сбросить все обработчики событий
    disconnect(mp_input, 0, 0, 0);
    // остановить аудиозахват
    mp_input->stop();
    // уничтожить устройство айдиозахвата
    delete mp_input;
    // занулить устройство
    mp_input = nullptr;
    // занулить буфер
    mp_inputBuffer = nullptr;
}


void MicrophoneCapture::readInputDevice()
{
    // считать доступные сэмплы
    QByteArray samples = mp_inputBuffer->readAll();
    // инициировать событие захвата сэмплов
    emit samplesRead(samples);
}


