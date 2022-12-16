#ifndef ISSOCOMMONDATA_H
#define ISSOCOMMONDATA_H

#include "issomoduledescriptor.h"

#include <QMap>
#include <QRegExpValidator>
#include <QApplication>
#include <QBuffer>
#include <QImage>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QProgressDialog>


namespace IssoConst
{
    const QString ORGANIZATION_NAME = "АйТи-Трейд";
    const QString APPLICATION_NAME = "СНК";

    // количество попыток
    const int LOGIN_TRY_COUNT = 3;
    const int SEND_TRY_COUNT = 3;
    // таймауты
//    const int SEND_DATA_TIMEOUT = 200;
    const int RECV_DATA_TIMEOUT = 600;
    const int POLL_STATE_INTERVAL = 100;
    // порты
    const quint16 SEND_PORT = 2323;
    const quint16 RECV_PORT = 2424;
    const quint16 ALARM_PORT = 2525;
    const quint16 AUDIO_SEND_PORT = 2626;
    const quint16 AUDIO_RECV_PORT = 2727;
    // характеристики аудио формата
    const int AUDIO_FORMAT_SAMPLE_RATE = 8000;
    const int AUDIO_FORMAT_CHANNEL_COUNT = 1;
    const int AUDIO_FORMAT_SAMPLE_SIZE = 16;
    // стили
    const QString INACTIVE_BTN_STYLE = "color: black; background-color: rgb(230, 230, 230); ";
    const QString ACTIVE_BTN_STYLE = "color: white; background-color: green; ";
    // url
    const QString LOG_SERVER_URL = "http://иссо.рф";
//                                 "http://localhost/иссо.рф";
    // пароль
    const QString MODIFY_PASSWORD = "2205";
    // версия сборки
    const QString BUILD_VERSION = "2.02.50";
    //
    // настройки модуля по умолчанию:
    //
    // ip модуля
    const QHostAddress DEFAULT_MODULE_IP = QHostAddress("192.168.1.100");
    // MAC-адрес
    const QByteArray DEFAULT_MAC = IssoModuleDescriptor::macStringToBytes("aa:bb:cc:dd:ee:ff");
    // IP сервера
    const QHostAddress DEFAULT_SERVER_IP = QHostAddress("192.168.1.10");
    // номер телефона
    const QByteArray DEFAULT_PHONE = IssoModuleDescriptor::phoneStringToBytes("89051112233");
    //
    // настройки камеры по умолчанию:
    //
    // ip камеры
    const QHostAddress DEFAULT_CAMERA_IP = QHostAddress("192.168.1.11");
    // порт камеры
    const quint16 DEFAULT_CAMERA_PORT = 554;
    //
    // количество регистров пороговых значений МД
    const int THRESHOLD_REG_COUNT = 19;
    //
    // настройки мультидачика по умолчанию:
    //
    // адрес
    const int DEFAULT_MS_ADDR = 247;
}




/*!
 * \brief   Состояния модуля МАСО
 */
enum IssoModuleState
{
    ALARM,              /// тревожное состояние
    FAIL,               /// неисправное состояние
    INACTIVE,           /// неактивное состояние
    UNKNOWN,            /// неизвестное состояние
    ACTIVE  = 0xFF      /// активное нетревожное состояние
};
Q_DECLARE_METATYPE(IssoModuleState)

QDataStream &operator<<(QDataStream &out, const IssoModuleState& state);
QDataStream &operator>>(QDataStream &in, IssoModuleState& state);




enum IssoCmdId
{
    CMD_INFO,
    CMD_DESC_GET,
    CMD_DESC_SET,
    CMD_ALARM,
    CMD_RELAY,
    CMD_VOICE,
    CMD_MS_INFO,
    CMD_MS_INFO_RAW,
    CMD_MS_POLL_ADDRS,
    CMD_MS_SINGLE_INPUT_REG_GET,
    CMD_MS_SINGLE_HOLD_REG_GET,
    CMD_MS_SINGLE_HOLD_REG_SET,
    CMD_MS_BLOCK_HOLD_REG_GET,
    CMD_MS_BLOCK_HOLD_REG_SET,
    CMD_MULTISENSOR_EXIST_GET,
    // CMD_MS_VALID_SLAVES_GET,
    CMD_UNDEFINED = 0xFF
};
Q_DECLARE_METATYPE(IssoCmdId)




enum IssoParamId
{
    PARAM_UNDEFINED,
    DESC_IP,            /// Параметр дескриптора: IP-адрес контроллера
    DESC_MAC,           /// Параметр дескриптора: MAC-адрес контроллера
    DESC_PORT,          /// Параметр дескриптора: Порт контроллера
    DESC_SERVER_IP,     /// Параметр дескриптора: IP-адрес сервера
    DESC_SERVER_PORT,   /// Параметр дескриптора: Порт сервера
    DESC_PHONE,         /// Параметр дескриптора: Номер телефона
    DESC_PHONE_ENABLED, /// Параметр дескриптора: Разрешение использования телефона
    DESC_CRC,           /// Параметр дескриптора: Контрольная сумма
    BTN_FIRE,           /// Кнопка «Пожар»
    BTN_SECURITY,       /// Кнопка «Охрана»
    SENSOR_SMOKE,       /// Датчик «Дым»
    SENSOR_CO,          /// Датчик «CO»
    ANALOG_CHAIN_1,     /// Аналоговый шлейф 1
    ANALOG_CHAIN_2,     /// Аналоговый шлейф 2
    ANALOG_CHAIN_3,     /// Аналоговый шлейф 3
    ANALOG_CHAIN_4,     /// Аналоговый шлейф 4
    ANALOG_CHAIN_5,     /// Аналоговый шлейф 5
    ANALOG_CHAIN_6,     /// Аналоговый шлейф 6

    INT_RELAY_1,        /// Внутреннее реле «1»
    INT_RELAY_2,        /// Внутреннее реле «2»
    INT_RELAY_3,        /// Внутреннее реле «3»
    INT_RELAY_4,        /// Внутреннее реле «4»
    INT_RELAY_5,        /// Внутреннее реле «5»
    INT_RELAY_6,        /// Внутреннее реле «6»

    VOLT_BATTERY = 25,  /// Напряжение АКБ
    VOLT_BUS,           /// Напряжение шины 12 В
    VOLT_SENSORS,       /// Напряжение датчиков
    SUPPLY_EXTERNAL,    /// Датчик наличия внешнего питания 220В
    SUPPLY_INTERNAL,    /// Датчик наличия внутреннего питания 220В
    TEMP_OUT,           /// Датчик внешней температура
    VIBRATION,          /// Датчик вибрации
    SENSOR_OPEN,        /// Датчик вскрытия

    STORED_REC_NUM,     /// Голос: номер записи в памяти контроллера
    PLAYBACK_TYPE,      /// Голос: тип воспроизведения
    REC_BLOCK_TOTAL,    /// Голос: общее число блоков данных
    REC_BLOCK_CURRENT,  /// Голос: номер текущего блока данных
    REC_BLOCK_SIZE,     /// Голос: размер блока данных
    REC_BLOCK_DATA,     /// Голос: данные текущего блока
    //
    //
    MULTISENSOR_ADDRS = 62,
    MULTISENSOR_DUMMY = 65,
    MULTISENSOR,
    MULTISENSOR_ADDR,
    MULTISENSOR_STATE_RAW,
    SINGLE_REG_ADDR,
    SINGLE_REG_DATA = 70,
    MULTISENSOR_EXIST_DATA = 73,
    BLOCK_REG_ADDR = 74,
    BLOCK_REG_DATA = 75,

    EXT_RELAY_1,            /// Внешнее реле «1»
    EXT_RELAY_2,            /// Внешнее реле «2»
    EXT_RELAY_3,            /// Внешнее реле «3»
    EXT_RELAY_4,            /// Внешнее реле «4»
    EXT_RELAY_5             /// Внешнее реле «5»
};
Q_DECLARE_METATYPE(IssoParamId)

QDataStream &operator<<(QDataStream &out, IssoParamId id);
QDataStream &operator>>(QDataStream &in, IssoParamId& id);




enum IssoState
{
    RELAY_OFF,
    RELAY_ON,
    VOLTAGE_LOW,
    VOLTAGE_MEDIUM,
    VOLTAGE_HIGH,
    STATE_NORMAL = 0xAA,
    STATE_ERROR = 0xBB,
    STATE_ALARM = 0xCC,
    STATE_WARNING = 0xDD,
    STATE_SELF_CONTROL = 0xEE,
    STATE_SHORT_CIRCUIT = 0xB1,
    STATE_BREAK = 0xB2,
    STATE_FAIL = 0xB7,
//    STATE_OFFLINE = 0xFD,
    STATE_SERVICE = 0xFE,
    STATE_UNDEFINED = 0xFF
};
Q_DECLARE_METATYPE(IssoState)

QDataStream &operator<<(QDataStream &out, const IssoState& state);
QDataStream &operator>>(QDataStream &in, IssoState& state);




enum IssoDigitalSensorId
{
    DIGITALSENSOR_UNDEFINED,
    DIGITALSENSOR_SMOKE_O_LED_ON,
    DIGITALSENSOR_SMOKE_O_LED_OFF,
    DIGITALSENSOR_SMOKE_O,
    DIGITALSENSOR_TEMP_A,
    DIGITALSENSOR_TEMP_D,
    DIGITALSENSOR_SMOKE_E,
    DIGITALSENSOR_FLAME_1,
    DIGITALSENSOR_FLAME_2,
    DIGITALSENSOR_FLAME_3,
    DIGITALSENSOR_FLAME_4,
    DIGITALSENSOR_FLAME_5,
    DIGITALSENSOR_FLAME_6,
    DIGITALSENSOR_FLAME_MIN_VAL,
    DIGITALSENSOR_FLAME_STD_DEV,
    DIGITALSENSOR_CO,
    DIGITALSENSOR_VOC
};
Q_DECLARE_METATYPE(IssoDigitalSensorId)

QDataStream &operator<<(QDataStream &out, IssoDigitalSensorId id);
QDataStream &operator>>(QDataStream &in, IssoDigitalSensorId& id);




enum IssoStateCondition
{
    CONDITION_OR,
    CONDITION_AND
};
Q_DECLARE_METATYPE(IssoStateCondition)

QDataStream &operator<<(QDataStream &out, IssoStateCondition condition);
QDataStream &operator>>(QDataStream &in, IssoStateCondition& condition);




enum IssoChainType
{
    SMOKE_CHAIN,
    MULTICRITERIAL,
    ALARM_BUTTON_CHAIN,
    MOVE_DETECTOR_CHAIN,
    TEMP_CHAIN,
    //
    VALVE_CHAIN,
    DOOR_CHAIN,
    PUMP_CHAIN,
    LATCH_CHAIN,
    //
    INTERNAL_SENSOR
};
Q_DECLARE_METATYPE(IssoChainType)

QDataStream &operator<<(QDataStream &out, IssoChainType type);
QDataStream &operator>>(QDataStream &in, IssoChainType& type);




enum class IssoGuiMode
{
    STANDART_MONITOR,
    TOUCH_SCREEN,
    TABLET
};
Q_DECLARE_METATYPE(IssoGuiMode)




enum class IssoAppType
{
    SNK,
    SOO,
    GSC
};
Q_DECLARE_METATYPE(IssoAppType)




enum class IssoScenarioScope
{
    FLOOR,
    BUILDING
};
Q_DECLARE_METATYPE(IssoScenarioScope)

QDataStream &operator<<(QDataStream &out, IssoScenarioScope scope);
QDataStream &operator>>(QDataStream &in, IssoScenarioScope& scope);




enum class IssoFixedModuleBtnFunc
{
//    CAMERA,
    VOICE
};
Q_DECLARE_METATYPE(IssoFixedModuleBtnFunc)




enum IssoSensorAction
{
    RESET_ALARM,
    RESET_ALL_ALARMS,
    SERVICE_MODE_ON,
    SERVICE_MODE_OFF,
    GET_RAW_DATA,
    SELF_CALIBRATION
};
Q_DECLARE_METATYPE(IssoSensorAction)



enum class IssoMsRegAction
{
    READ_THRESHOLD,
    WRITE_THRESHOLD,
    RESET_STATE,
    READ_RAW_DATA,
    BEEP,
    CALIBRATE
};
Q_DECLARE_METATYPE(IssoMsRegAction)


enum IssoEventType
{
    EVENT_CLEAR_LOG,
    EVENT_PING,
    EVENT_SENSORS,
    EVENT_PHOTO,
    EVENT_UNKNOWN = 0xFF,
};
Q_DECLARE_METATYPE(IssoEventType)





struct IssoStateRange
{
    private:
        double m_min;
        double m_max;
    public:
        IssoStateRange();
        IssoStateRange(double min, double max);

        double min() const;
        void setMin(double min);
        double max() const;
        void setMax(double max);

        friend QDataStream &operator<<(QDataStream &out, const IssoStateRange& range);
        friend QDataStream &operator>>(QDataStream &in, IssoStateRange& range);
};
Q_DECLARE_METATYPE(IssoStateRange)




class IssoCommonData
{
    private:
        static const QMap<IssoCmdId, QString> m_cmdIds;
        static const QMap<IssoParamId, QString> m_paramIds;
        static const QMap<IssoDigitalSensorId, QString> m_digitalSensorIds;
        static const QMap<IssoModuleState, QString> m_moduleStates;
        static const QMap<IssoState, QString> m_paramStates;
        static const QMap<IssoScenarioScope, QString> m_scenarioScopes;
        static const QMap<IssoFixedModuleBtnFunc, QString> m_fixedModuleBtnFuncs;
        static const QMap<IssoState, quint8> m_statePriorities;

    public:
        static IssoCmdId cmdIdByString(const QString& strValue);
        static QString stringByCmdId(const IssoCmdId& id);

        static IssoParamId paramIdByString(const QString& strValue);
        static QString stringByParamId(const IssoParamId& id);
        static QStringList eventSourceNames();

        static IssoDigitalSensorId digitalSensorIdByString(const QString& strValue);
        static QString stringByDigitalSensorId(const IssoDigitalSensorId& id);
        static QList<IssoDigitalSensorId> displayingDigitalSensorIds();

        static QRegExpValidator* makeIpValidator(QObject* parent);

        static QByteArray convertImageToByteArray(const QImage& image,
                                                  const char* imageFormat = "PNG");
        static QImage convertBase64ToImage(const QString& base64String,
                                           const char* imageFormat = "PNG");
        static QString convertImageToBase64(const QImage& image,
                                            const char* imageFormat = "PNG");

        static QString getAppTypeName(IssoAppType appType);

        static QString stringByModuleState(const IssoModuleState& state);
        static IssoModuleState moduleStateByString(const QString& stateName);

        static QString stringBySensorState(const IssoState& state);
        static IssoState sensorStateByString(const QString& stateName);

        static QString stringByScenarioScope(const IssoScenarioScope& scope);
        static IssoScenarioScope scenarioScopeByString(const QString& scopeName);
        static QStringList scenarioScopeStrings();

        static QString stringByFixedModuleBtnFunc(IssoFixedModuleBtnFunc func);
        static IssoFixedModuleBtnFunc fixedModuleBtnFuncByString(const QString& funcName);
        static QStringList fixedModuleBtnFuncStrings();
        static bool isFixedModuleBtnFunc(const QString& funcName);

        static QList<QHostAddress> localIps();

        static IssoModuleState moduleStateByParamState(IssoState paramState);

        static IssoState higherPriorityState(IssoState state1, IssoState state2);

        static QColor backgroundByModuleState(IssoModuleState state);
        static QColor foregroundByModuleState(IssoModuleState state);

        static QColor backgroundByState(IssoState state);
        static QColor foregroundByState(IssoState state);

        static QString makeMultiSensorName(int id);

        static QString showInputTextDialog(QWidget *parent, const QString& title,
                                           const QString& label, bool& ok);
        static int showInputNumberDialog(QWidget *parent, const QString& title,
                                      const QString& label, bool& ok,
                                      int min, int max, int defaultValue = 1);
        static QString showSelectItemDialog(QWidget *parent, const QString& title,
                                            const QString& label, const QStringList& items,
                                            bool& ok);
        static bool showQuestionDialog(QWidget *parent, const QString& title, const QString& label);

        static QProgressDialog *makeProgressDialog(const QString& title, const QString& label,
                                                   int min, int max, QWidget* parent = nullptr);

        static void printMsg(const QString& msg);

        static QHostAddress incrementIp(const QHostAddress& ip);
};




#endif // ISSOCOMMONDATA_H

