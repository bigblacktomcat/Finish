#include "issoaudiocontroller.h"


IssoAudioController::IssoAudioController(const QHostAddress &ip, quint16 bindPort,
                                         int sampleRate, int channelCount, int sampleSize,
                                         QObject *parent)
    : QObject(parent),
      m_ip(ip),
      m_bindPort(bindPort)
{
    mp_mp3Reader = new Mp3FileReader(sampleRate, channelCount, sampleSize);
    mp_encoder = new SpeexEncoder();
}


IssoAudioController::~IssoAudioController()
{
    qDebug() << "~IssoAudioController";
    //
    stopPlaying();
    //
    if (mp_encoder)
        delete mp_encoder;
    if (mp_mp3Reader)
        delete mp_mp3Reader;
}


IssoModuleRequest IssoAudioController::makeRequest(const QHostAddress &ip, quint16 port,
                                                   quint8 recordNum, quint8 totalBlocks,
                                                   quint8 currentBlock, const QByteArray &audioData)
{
    // сформировать буфер параметров
    IssoParamBuffer params;
    params.insertParam(new IssoByteParam(STORED_REC_NUM, recordNum));
    params.insertParam(new IssoByteParam(PLAYBACK_TYPE, 0));
    params.insertParam(new IssoByteParam(REC_BLOCK_TOTAL, totalBlocks));
    params.insertParam(new IssoByteParam(REC_BLOCK_CURRENT, currentBlock));
    params.insertParam(new IssoShortParam(REC_BLOCK_SIZE, (short)audioData.length()));
    params.insertParam(new IssoArrayParam(REC_BLOCK_DATA, audioData));
    // создать  и вернуть запрос
    return IssoModuleRequest(ip, port, ++m_packNum, CMD_VOICE, params);
}


void IssoAudioController::encodeAndSend(const QHostAddress &ip, quint16 port,
                                        const QByteArray &pcmSamples)
{
    // создать отправителя
    IssoRequestSender sender(m_ip, m_bindPort);
    // кодировать кодеком speex
    QByteArray encodedData = mp_encoder->encodeAudioData(pcmSamples);
    // установить флаг выполнения
    m_running = true;
    //
    // поток чтения массива
    QDataStream stream(&encodedData, QIODevice::ReadOnly);
    // размер считанного аудиоблока
    int blockSize;
    // общее число блоков данных
    quint8 totalBlocks = encodedData.size() / MAX_AUDIO_BLOCK_LEN;
    // если остаток от деления != 0, то число блоков на 1 больше
    if ((encodedData.size() % MAX_AUDIO_BLOCK_LEN) != 0)
        totalBlocks++;
    // номер текущего блока
    quint8 curBlock = 0;
    // успех отправки запроса
    bool success;
    //
    // разбить массив на блоки по MAX_AUDIO_DATA_LEN байт
    while(m_running && (blockSize = stream.device()->bytesAvailable()) > 0)
    {
        // если в потоке осталось больше MAX_AUDIO_DATA_LEN данных,
        // считать MAX_AUDIO_DATA_LEN,
        // иначе считать остаток
        if (blockSize >= MAX_AUDIO_BLOCK_LEN)
            blockSize = MAX_AUDIO_BLOCK_LEN;
        // выделить текущий блок из начала потока
        QByteArray audioBlock(blockSize, 0);
        stream.readRawData(audioBlock.data(), audioBlock.size());
        // создать запрос на воспроизведение записи из памяти контроллера
        IssoModuleRequest request =
                makeRequest(ip, port, 0xFF, totalBlocks, curBlock, audioBlock);
        // отправить запрос
        sender.sendRequestRepeatedly(request, success, IssoConst::SEND_TRY_COUNT);
        // если получен корректный ответ
        if (success)
        {
            // известить об успешной отправке очередного блока
            emit recordBlockSent(totalBlocks, curBlock);
            // если отправлен последний блок, известить о завершении отправки
            if (curBlock == totalBlocks - 1)
                emit recordSendComplete();
        }
        else
        {
            // известить об ошибке отправки
            emit recordSendFailed(totalBlocks, curBlock);
            break;
        }
        curBlock++;
    }
    // сбросить флаг выполнения
    m_running = false;
}


bool IssoAudioController::playMp3FileAsync(const QHostAddress &ip, quint16 port,
                                           const QString &fileName)
{
    if (!m_running)
    {
        QtConcurrent::run(this, &IssoAudioController::playMp3File,
                          ip, port, fileName);
        return true;
    }
    else
        return false;
}


bool IssoAudioController::playVoiceRecordAsync(const QHostAddress &ip, quint16 port,
                                               const QByteArray &pcmSamples)
{
    if (!m_running)
    {
        QtConcurrent::run(this, &IssoAudioController::playVoiceRecord,
                          ip, port, pcmSamples);
        return true;
    }
    else
        return false;
}


void IssoAudioController::playVoiceRecord(const QHostAddress &ip, quint16 port,
                                          const QByteArray &pcmSamples)
{
    // кодировать pcm-сэмплы и отправить в модуль
    encodeAndSend(ip, port, pcmSamples);
}


void IssoAudioController::stopPlaying()
{
    m_running = false;
}


void IssoAudioController::playMp3File(const QHostAddress &ip, quint16 port,
                                      const QString &fileName)
{
    // считать из mp3-файла pcm-сэмплы
    QByteArray pcmSamples = mp_mp3Reader->readMp3File(fileName, false);
    // кодировать pcm-сэмплы и отправить в модуль
    encodeAndSend(ip, port, pcmSamples);
}


bool IssoAudioController::playModuleRecordAsync(const QHostAddress &ip, quint16 port,
                                                quint8 recordNum)
{
    if (!m_running)
    {
        QtConcurrent::run(this, &IssoAudioController::playModuleRecord,
                                ip, port, recordNum);
        return true;
    }
    else
        return false;
}


void IssoAudioController::playModuleRecord(const QHostAddress &ip, quint16 port,
                                           quint8 recordNum)
{
    // создать отправителя
    IssoRequestSender sender(m_ip, m_bindPort);
    // установить флаг выполнения
    m_running = true;
    //
    // создать запрос на воспроизведение записи из памяти контроллера
    IssoModuleRequest request = makeRequest(ip, port, recordNum, 0, 0, QByteArray());
    // успех отправки запроса
    bool success;
    // отправить запрос
    sender.sendRequestRepeatedly(request, success, IssoConst::SEND_TRY_COUNT);
    // если получен корректный ответ
    if (success)
        // известить об успешном выполнении запроса
        emit playbackRequestComplete(recordNum);
    else
        // известить об ошибке выполнения запроса
        emit playbackRequestFailed(recordNum);
    //
    // сбросить флаг выполнения
    m_running = false;
}

