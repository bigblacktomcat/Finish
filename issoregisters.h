#ifndef ISSOREGISTERS_H
#define ISSOREGISTERS_H


/*!
 * \brief   Адреса Input-регистров
 */
namespace IssoInputReg
{
    enum
    {
        // 0 - неизвестно
        // 1 - мультидатчик
        // 2 - панель управления
        // 3 - реле
        DEVICE_TYPE,
        SN_FIRST,
        SN_SECOND,
        FW_VER_MAJOR,
        FW_VER_MINOR,
        MS_ADDR,
        //
        // 0 - Норма
        // 1 - Внимание
        // 2 - Тревога
        // 3 - калибровка
        MS_STATE,
        //
        // по 2 бита на состояние каждого датчика
        // + старший бит - признак актуальности данных
        // (0 - актуальны, 1 - устарели)
        SENSOR_STATES,
        // RESERVED = 8
        SMOKE_O_LED_ON = 9,
        SMOKE_O_LED_OFF,
        SMOKE_O_LED_ON_OFF,
        TEMP_A,
        TEMP_D,
        SMOKE_E,
        FLAME_1,
        FLAME_2,
        FLAME_3,
        FLAME_4,
        FLAME_5,
        FLAME_6,
        FLAME_MIN_VAL,
        FLAME_STD_DEV,
        CO,
        VOC
    };
}



/*!
 * \brief   Адреса Holding-регистров
 */
namespace IssoHoldReg
{
    enum
    {
        SN_FIRST,
        SN_SECOND,
        MS_ADDR,
        SENSOR_MASK,
        SMOKE_O_WARNING,
        SMOKE_O_ALARM,
        TEMP_A_WARNING,
        TEMP_A_ALARM,
        TEMP_D_WARNING,
        TEMP_D_ALARM,
        SMOKE_E_WARNING,
        SMOKE_E_ALARM,
        FLAME_MIN_VAL_WARNING,
        FLAME_MIN_VAL_ALARM,
        FLAME_STD_DEV_WARNING,
        FLAME_STD_DEV_ALARM,
        CO_WARNING,
        CO_ALARM,
        VOC_WARNING,
        VOC_ALARM,
        SMOKE_O_SENS,
        SMOKE_O_BRIGHT,
        //
        // 0 - нет действия
        // 1 - чтение Ident     (байты 0 - 2)
        // 2 - запись Ident     (байты 0 - 2)
        // 4 - чтение Threshold (байты 3 - 21)
        // 8 - запись Threshold (байты 3 - 21)
        // 16 - чтение UInfo    (байт 26)
        // 32 - запись UInfo    (байт 26)
        ROM_CMD,
        // RESERVED = 23
        //
        // 0 - Норма
        // 1 - Внимание
        // 2 - Тревога
        MS_STATE = 24,
        //
        // 0 - короткий звук
        // 1 - начать калибровку
        MS_ACTION,
        UART_SETTINGS,
        // RESERVED = 27
        // RESERVED = 28
        // RESERVED = 29
    };
}



/*!
 * \brief   Значение Holding-регистра, определяющего команду ПЗУ
 *          (чтение / запись структуры)
 */
namespace IssoRomCmdValue
{
    enum
    {
        // 0 - нет действия
        NOTHING = 0,
        // 1 - чтение Ident     (байты 0 - 2)
        READ_IDENT = 1,
        // 2 - запись Ident     (байты 0 - 2)
        WRITE_IDENT = 2,
        // 4 - чтение Threshold (байты 3 - 21)
        READ_THRES = 4,
        // 8 - запись Threshold (байты 3 - 21)
        WRITE_THRES = 8,
        // 16 - чтение UInfo    (байт 26)
        READ_UINFO = 16,
        // 32 - запись UInfo    (байт 26)
        WRITE_UINFO = 32
    };
}



/*!
 * \brief   Значение Holding-регистра, определяющего состояние мультидатчика
 */
namespace IssoMsStateValue
{
    enum
    {
        // 0 - Норма
        MS_STATE_NORMAL,
        // 1 - Внимание
        MS_STATE_WARNING,
        // 2 - Тревога
        MS_STATE_ALARM
    };
}



/*!
 * \brief   Значение Holding-регистра, определяющего действие мультидатчика
 */
namespace IssoMsActionValue
{
    enum
    {
        // 0 - короткий звук
        SHORT_BEEP,
        // 1 - начать калибровку
        START_CALIBRATION
    };
}


#endif // ISSOREGISTERS_H
