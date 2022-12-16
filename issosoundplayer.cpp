#include "issosoundplayer.h"


IssoSoundPlayer::IssoSoundPlayer(QObject *parent) : QObject(parent)
{
    createSounds();
}


IssoSoundPlayer::~IssoSoundPlayer()
{
    qDebug() << "~IssoSoundPlayer" << endl;
    //
    destroySounds();
}


IssoSoundType IssoSoundPlayer::soundType() const
{
    return m_soundType;
}


void IssoSoundPlayer::setSoundType(const IssoSoundType &soundType)
{
    QMutexLocker locker(&m_mutex);
    //
    // если данный тип звука уже задан, не обрабатывать
    if (m_soundType == soundType)
        return;
    // если звук воспроизводится, остановить
    if (m_currentSound)
        m_currentSound->stop();

    // получить звуковой объект по его типу
    m_currentSound = m_sounds.value(soundType, nullptr);
    // если объект найден, воспроизвести его
    if (m_currentSound)
    {
        m_currentSound->setLoops(QSound::Infinite);
        m_currentSound->play();
    }
    // запомнить заданный тип звука
    m_soundType = soundType;
}


void IssoSoundPlayer::createSounds()
{
    m_sounds.insert(SOUND_ALARM, new QSound(":/resources/soundAlarm"));
    m_sounds.insert(SOUND_FAIL, new QSound(":/resources/soundFail"));
}


void IssoSoundPlayer::destroySounds()
{
    // обнулить указатель на текущий звук
    m_currentSound = nullptr;
    // уничтожить все звуки из коллекции
    foreach (QSound* sound, m_sounds)
        delete sound;
}


void IssoSoundPlayer::playSound(IssoSoundType soundType)
{
    setSoundType(soundType);
}


void IssoSoundPlayer::stopSound()
{
    setSoundType(SOUND_NONE);
}
