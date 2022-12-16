#ifndef ISSOSOUNDPLAYER_H
#define ISSOSOUNDPLAYER_H

#include <QObject>
#include <QSound>
#include <QDebug>
#include <QMutex>
#include <QThread>


enum IssoSoundType
{
    SOUND_NONE,
    SOUND_ALARM,
    SOUND_FAIL
};
Q_DECLARE_METATYPE(IssoSoundType)



class IssoSoundPlayer : public QObject
{
        Q_OBJECT
    private:
        // звук тревоги
        QSound* m_sound = nullptr;
        IssoSoundType m_soundType = SOUND_NONE;
        QSound* m_currentSound = nullptr;
        QMap<IssoSoundType, QSound*> m_sounds;
        QMutex m_mutex;

        void createSounds();
        void destroySounds();

    public:
        explicit IssoSoundPlayer(QObject *parent = nullptr);
        ~IssoSoundPlayer();

        void playSound(IssoSoundType soundType);
        void stopSound();

        IssoSoundType soundType() const;
        void setSoundType(const IssoSoundType &soundType);

    signals:

    public slots:
};

#endif // ISSOSOUNDPLAYER_H
