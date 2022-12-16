#include "issoanalogsensorsetuppanel.h"
#include "ui_issoanalogsensorsetuppanel.h"


IssoAnalogSensorSetupPanel::IssoAnalogSensorSetupPanel(QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::IssoAnalogSensorSetupPanel)
{
    ui->setupUi(this);
    init();
}


IssoAnalogSensorSetupPanel::~IssoAnalogSensorSetupPanel()
{
    delete ui;
}


void IssoAnalogSensorSetupPanel::init()
{
    // заполнить карту ассоциации состояний с виджетами
    m_stateWidgets.insert(STATE_NORMAL, ui->wgtNormal);
    m_stateWidgets.insert(STATE_WARNING, ui->wgtWarning);
    m_stateWidgets.insert(STATE_ALARM, ui->wgtAlarm);
    m_stateWidgets.insert(STATE_SHORT_CIRCUIT, ui->wgtShortCircuit);
    m_stateWidgets.insert(STATE_BREAK, ui->wgtBreak);
}


void IssoAnalogSensorSetupPanel::setStateRange(const IssoState &state,
                                               double min, double max)
{
    switch(state)
    {
        // норма
        case STATE_NORMAL:
        {
            ui->dsbNormalMin->setValue(min);
            ui->dsbNormalMax->setValue(max);
            break;
        }
        // внимание
        case STATE_WARNING:
        {
            ui->dsbWarningMin->setValue(min);
            ui->dsbWarningMax->setValue(max);
            break;
        }
        // тревога
        case STATE_ALARM:
        {
            ui->dsbAlarmMin->setValue(min);
            ui->dsbAlarmMax->setValue(max);
            break;
        }
        // короткое замывание
        case STATE_SHORT_CIRCUIT:
        {
            ui->dsbShortCircuitMin->setValue(min);
            ui->dsbShortCircuitMax->setValue(max);
            break;
        }
        // обрыв
        case STATE_BREAK:
        {
            ui->dsbBreakMin->setValue(min);
            ui->dsbBreakMax->setValue(max);
            break;
        }
        default:
            break;
    }
}


void IssoAnalogSensorSetupPanel::getStateRange(const IssoState &state,
                                               double &min, double &max)
{
    switch(state)
    {
        // норма
        case STATE_NORMAL:
        {
            min = ui->dsbNormalMin->value();
            max = ui->dsbNormalMax->value();
            break;
        }
        // внимание
        case STATE_WARNING:
        {
            min = ui->dsbWarningMin->value();
            max = ui->dsbWarningMin->value();
            break;
        }
        // тревога
        case STATE_ALARM:
        {
            min = ui->dsbAlarmMin->value();
            max = ui->dsbAlarmMin->value();
            break;
        }
        // короткое замывание
        case STATE_SHORT_CIRCUIT:
        {
            min = ui->dsbShortCircuitMin->value();
            max = ui->dsbShortCircuitMax->value();
            break;
        }
        // обрыв
        case STATE_BREAK:
        {
            min = ui->dsbBreakMin->value();
            max = ui->dsbBreakMax->value();
            break;
        }
        default:
            break;
    }
}


QMap<IssoState, IssoStateRange> IssoAnalogSensorSetupPanel::stateRanges()
{
    QMap<IssoState, IssoStateRange> ranges;
    // норма
    if (m_stateWidgets.value(STATE_NORMAL)->isVisible())
    {
        ranges.insert(STATE_NORMAL, IssoStateRange(ui->dsbNormalMin->value(),
                                                   ui->dsbNormalMax->value()));
    }
    // внимание
    if (m_stateWidgets.value(STATE_WARNING)->isVisible())
    {
        ranges.insert(STATE_WARNING, IssoStateRange(ui->dsbWarningMin->value(),
                                                    ui->dsbWarningMax->value()));
    }
    // тревога
    if (m_stateWidgets.value(STATE_ALARM)->isVisible())
    {
        ranges.insert(STATE_ALARM, IssoStateRange(ui->dsbAlarmMin->value(),
                                                  ui->dsbAlarmMax->value()));
    }
    // короткое замыкание
    if (m_stateWidgets.value(STATE_SHORT_CIRCUIT)->isVisible())
    {
        ranges.insert(STATE_SHORT_CIRCUIT, IssoStateRange(ui->dsbShortCircuitMin->value(),
                                                          ui->dsbShortCircuitMax->value()));
    }
    // обрыв
    if (m_stateWidgets.value(STATE_BREAK)->isVisible())
    {
        ranges.insert(STATE_BREAK, IssoStateRange(ui->dsbBreakMin->value(),
                                                  ui->dsbBreakMax->value()));
    }
    return ranges;
}


void IssoAnalogSensorSetupPanel::setStateRanges(const QMap<IssoState, IssoStateRange> &ranges)
{
    foreach (IssoState state, ranges.keys())
    {
        // если виджет невидимый, не обрабатывать
        if (!m_stateWidgets.value(state)->isVisible())
            continue;
        // получить диапазон значений состояния
        IssoStateRange range = ranges.value(state);
        // вывести в компонент диапазон соответствующего состояния
        setStateRange(state, range.min(), range.max());
    }
}


void IssoAnalogSensorSetupPanel::clearStateRanges()
{
    foreach (IssoState state, m_stateWidgets.keys())
    {
        // обнулить диапазон соответствующего состояния
        setStateRange(state, 0, 0);
    }
}


QSet<IssoState> IssoAnalogSensorSetupPanel::displayingStates() const
{
    QSet<IssoState> result;
    foreach (IssoState state, m_stateWidgets.keys())
    {
        if (m_stateWidgets.value(state)->isVisible())
            result.insert(state);
    }
    return result;
}


void IssoAnalogSensorSetupPanel::setDisplayingStates(const QSet<IssoState> states)
{
    foreach (IssoState curState, m_stateWidgets.keys())
    {
        // текущее состояние видимое, если содержится в наборе
        bool visible = states.contains(curState);
        // задать видимость
        m_stateWidgets.value(curState)->setVisible(visible);
    }
}
