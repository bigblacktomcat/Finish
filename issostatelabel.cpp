#include "issostatelabel.h"


IssoStateLabel::IssoStateLabel(QWidget *parent)
    : QLabel(parent)
{
    // задать исходный стиль
    setState(STATE_UNDEFINED);
}


void IssoStateLabel::setState(IssoState state, const QString& title)
{
    m_state = state;
    // остановить мигание
    stopBlinking();
    // сформировать стиль согласно состоянию
    QString stateStyle = QString("IssoStateLabel { color: %1; background-color: %2; }")
                            .arg(IssoCommonData::foregroundByState(state).name())
                            .arg(IssoCommonData::backgroundByState(state).name());
    // задать стиль
    setStyleSheet(stateStyle);
    // отобразить заголовок в лейбле
    setText(title);
    // если состояние Внимание, запустить мигание
    if ((state == STATE_WARNING) || (state == STATE_BREAK))
    {
        // сформировать стиль неопределенного состояния
        QString undefinedStyle = QString("IssoStateLabel { color: %1; background-color: %2; }")
                    .arg(IssoCommonData::foregroundByState(STATE_UNDEFINED).name())
                    .arg(IssoCommonData::backgroundByState(STATE_UNDEFINED).name());
        // запустить мигание
        startBlinking(stateStyle, undefinedStyle);
    }
}


QPalette IssoStateLabel::makePalette(QColor textColor, QColor fillColor)
{
    QPalette p = palette();
    p.setColor(QPalette::Foreground, textColor);
    p.setColor(QPalette::Window, fillColor);
    return p;
}


void IssoStateLabel::startBlinking(const QString &blinkStyle1,
                                   const QString &blinkStyle2,
                                   int interval)
{
    // задать интервал мигания
    m_blinkTimer.setInterval(interval);
    // задать метод смены стиля по таймеру
    connect(&m_blinkTimer, &QTimer::timeout,
            [this, blinkStyle1, blinkStyle2]()
            {
                // сменить стиль
                setStyleSheet(styleSheet() == blinkStyle1 ? blinkStyle2 : blinkStyle1);
            });
    // запустить мигание
    m_blinkTimer.start();
}


void IssoStateLabel::stopBlinking()
{
    // остановить мигание
    m_blinkTimer.stop();
    // отключить обработик таймера
    disconnect(&m_blinkTimer, 0, 0, 0);
}

