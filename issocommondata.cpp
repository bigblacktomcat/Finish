/**
* Содержит описание и значение базовых структур данных
*/
#include "issocommondata.h"

#include <QInputDialog>
#include <QMessageBox>



const QMap<IssoCmdId, QString> IssoCommonData::m_cmdIds =
{
    {CMD_INFO,                      "INFO"},
    {CMD_DESC_GET,                  "DESG"},
    {CMD_DESC_SET,                  "DESS"},
    {CMD_ALARM,                     "ALRM"},
    {CMD_RELAY,                     "RELY"},
    {CMD_MS_INFO,                   "MASG"},
    {CMD_MS_INFO_RAW,               "MRDG"},
    {CMD_MS_POLL_ADDRS,             "MVSS"},
    {CMD_MS_SINGLE_INPUT_REG_GET,   "MIRG"},
    {CMD_MS_SINGLE_HOLD_REG_GET,    "MHRG"},
    {CMD_MS_SINGLE_HOLD_REG_SET,    "MHRS"},
    {CMD_MS_BLOCK_HOLD_REG_GET,     "MHMG"},
    {CMD_MS_BLOCK_HOLD_REG_SET,     "MHMS"},
    {CMD_MULTISENSOR_EXIST_GET,     "MVSV"},
  //  {CMD_MS_VALID_SLAVES_GET,       "MVSG"}, // вместо MVSV по-кругу
    {CMD_UNDEFINED,                 ""    }
};


const QMap<IssoParamId, QString> IssoCommonData::m_paramIds =
{
    {BTN_FIRE,          QT_TRANSLATE_NOOP("IssoCommonData", "Кнопка Пожар")},
    {BTN_SECURITY,      QT_TRANSLATE_NOOP("IssoCommonData", "Кнопка Охрана")},
    {SENSOR_SMOKE,      QT_TRANSLATE_NOOP("IssoCommonData", "Дым")},
    {SENSOR_CO,         QT_TRANSLATE_NOOP("IssoCommonData", "Угарный газ")},
    {ANALOG_CHAIN_1,    QT_TRANSLATE_NOOP("IssoCommonData", "Шлейф 1")},
    {ANALOG_CHAIN_2,    QT_TRANSLATE_NOOP("IssoCommonData", "Шлейф 2")},
    {ANALOG_CHAIN_3,    QT_TRANSLATE_NOOP("IssoCommonData", "Шлейф 3")},
    {ANALOG_CHAIN_4,    QT_TRANSLATE_NOOP("IssoCommonData", "Шлейф 4")},
    {ANALOG_CHAIN_5,    QT_TRANSLATE_NOOP("IssoCommonData", "Шлейф 5")},
    {ANALOG_CHAIN_6,    QT_TRANSLATE_NOOP("IssoCommonData", "Шлейф 6")},

    {INT_RELAY_1,       QT_TRANSLATE_NOOP("IssoCommonData", "Реле 1")},
    {INT_RELAY_2,       QT_TRANSLATE_NOOP("IssoCommonData", "Реле 2")},
    {INT_RELAY_3,       QT_TRANSLATE_NOOP("IssoCommonData", "Реле 3")},
    {INT_RELAY_4,       QT_TRANSLATE_NOOP("IssoCommonData", "Реле 4")},
    {INT_RELAY_5,       QT_TRANSLATE_NOOP("IssoCommonData", "Реле 5")},
    {INT_RELAY_6,       QT_TRANSLATE_NOOP("IssoCommonData", "Реле 6")},

    {VOLT_BATTERY,      QT_TRANSLATE_NOOP("IssoCommonData", "АКБ")},
    {VOLT_BUS,          QT_TRANSLATE_NOOP("IssoCommonData", "Питание")},
    {VOLT_SENSORS,      QT_TRANSLATE_NOOP("IssoCommonData", "Датчики")},
    {SUPPLY_EXTERNAL,   QT_TRANSLATE_NOOP("IssoCommonData", "Внешняя сеть")},
    {SUPPLY_INTERNAL,   QT_TRANSLATE_NOOP("IssoCommonData", "Сеть")},
    {TEMP_OUT,          QT_TRANSLATE_NOOP("IssoCommonData", "Темп.")},
    {VIBRATION,         QT_TRANSLATE_NOOP("IssoCommonData", "Вибрация")},
    {SENSOR_OPEN,       QT_TRANSLATE_NOOP("IssoCommonData", "Вскрытие")},

    {MULTISENSOR,       QT_TRANSLATE_NOOP("IssoCommonData", "ИПМ")},
    {MULTISENSOR_ADDR,  QT_TRANSLATE_NOOP("IssoCommonData", "Адрес ИПМ")},
    {MULTISENSOR_STATE_RAW,  QT_TRANSLATE_NOOP("IssoCommonData", "ИПМ (сырое)")},
    {MULTISENSOR_ADDRS, QT_TRANSLATE_NOOP("IssoCommonData", "Адреса ИПМ")},
    {SINGLE_REG_ADDR,          QT_TRANSLATE_NOOP("IssoCommonData", "Адрес регистра")},
    {SINGLE_REG_DATA,          QT_TRANSLATE_NOOP("IssoCommonData", "Данные регистра")},
    {MULTISENSOR_EXIST_DATA, QT_TRANSLATE_NOOP("IssoCommonData", "Данные наличия ИПМ")},
    {BLOCK_REG_ADDR,          QT_TRANSLATE_NOOP("IssoCommonData", "Адрес блока регистров")},
    {BLOCK_REG_DATA,          QT_TRANSLATE_NOOP("IssoCommonData", "Данные блока регистров")},

    {EXT_RELAY_1,       QT_TRANSLATE_NOOP("IssoCommonData", "Внешнее реле 1")},
    {EXT_RELAY_2,       QT_TRANSLATE_NOOP("IssoCommonData", "Внешнее реле 2")},
    {EXT_RELAY_3,       QT_TRANSLATE_NOOP("IssoCommonData", "Внешнее реле 3")},
    {EXT_RELAY_4,       QT_TRANSLATE_NOOP("IssoCommonData", "Внешнее реле 4")},
    {EXT_RELAY_5,       QT_TRANSLATE_NOOP("IssoCommonData", "Внешнее реле 5")}
};


const QMap<IssoDigitalSensorId, QString> IssoCommonData::m_digitalSensorIds =
{
    {DIGITALSENSOR_UNDEFINED,       QT_TRANSLATE_NOOP("IssoCommonData", "???")},
    {DIGITALSENSOR_SMOKE_O_LED_ON,  QT_TRANSLATE_NOOP("IssoCommonData", "Дым (о) - LED ON")},
    {DIGITALSENSOR_SMOKE_O_LED_OFF, QT_TRANSLATE_NOOP("IssoCommonData", "Дым (о) - LED OFF")},
    {DIGITALSENSOR_SMOKE_O,         QT_TRANSLATE_NOOP("IssoCommonData", "Дым (о)")},
    {DIGITALSENSOR_TEMP_A,          QT_TRANSLATE_NOOP("IssoCommonData", "Темп (а)")},
    {DIGITALSENSOR_TEMP_D,          QT_TRANSLATE_NOOP("IssoCommonData", "Темп (ц)")},
    {DIGITALSENSOR_SMOKE_E,         QT_TRANSLATE_NOOP("IssoCommonData", "Дым (э)")},
    {DIGITALSENSOR_FLAME_1,         QT_TRANSLATE_NOOP("IssoCommonData", "Пламя - 1")},
    {DIGITALSENSOR_FLAME_2,         QT_TRANSLATE_NOOP("IssoCommonData", "Пламя - 2")},
    {DIGITALSENSOR_FLAME_3,         QT_TRANSLATE_NOOP("IssoCommonData", "Пламя - 3")},
    {DIGITALSENSOR_FLAME_4,         QT_TRANSLATE_NOOP("IssoCommonData", "Пламя - 4")},
    {DIGITALSENSOR_FLAME_5,         QT_TRANSLATE_NOOP("IssoCommonData", "Пламя - 5")},
    {DIGITALSENSOR_FLAME_6,         QT_TRANSLATE_NOOP("IssoCommonData", "Пламя - 6")},
    {DIGITALSENSOR_FLAME_MIN_VAL,   QT_TRANSLATE_NOOP("IssoCommonData", "Пламя (мин)")},
    {DIGITALSENSOR_FLAME_STD_DEV,   QT_TRANSLATE_NOOP("IssoCommonData", "Пламя (сред)")},
    {DIGITALSENSOR_CO,              QT_TRANSLATE_NOOP("IssoCommonData", "CO")},
    {DIGITALSENSOR_VOC,             QT_TRANSLATE_NOOP("IssoCommonData", "ЛОВ")}
};


const QMap<IssoModuleState, QString> IssoCommonData::m_moduleStates =
{
    {INACTIVE,  QT_TRANSLATE_NOOP("IssoCommonData", "Неактивен")},
    {ACTIVE,    QT_TRANSLATE_NOOP("IssoCommonData", "Активен")},
    {ALARM,     QT_TRANSLATE_NOOP("IssoCommonData", "ТРЕВОГА")},
    {FAIL,      QT_TRANSLATE_NOOP("IssoCommonData", "Неисправен")},
    {UNKNOWN,   QT_TRANSLATE_NOOP("IssoCommonData", "Нет связи")}
};



const QMap<IssoState, QString> IssoCommonData::m_paramStates =
{
    {RELAY_OFF,          QT_TRANSLATE_NOOP("IssoCommonData", "Выключено")},
    {RELAY_ON,           QT_TRANSLATE_NOOP("IssoCommonData", "Включено")},
    {VOLTAGE_LOW,        QT_TRANSLATE_NOOP("IssoCommonData", "Уровень заряда: низкий")},
    {VOLTAGE_MEDIUM,     QT_TRANSLATE_NOOP("IssoCommonData", "Уровень заряда: нормальный")},
    {VOLTAGE_HIGH,       QT_TRANSLATE_NOOP("IssoCommonData", "Уровень заряда: высокий")},
    {STATE_NORMAL,       QT_TRANSLATE_NOOP("IssoCommonData", "Норма")},
    {STATE_ERROR,        QT_TRANSLATE_NOOP("IssoCommonData", "Отказ")},
    {STATE_ALARM,        QT_TRANSLATE_NOOP("IssoCommonData", "Тревога")},
    {STATE_WARNING,      QT_TRANSLATE_NOOP("IssoCommonData", "Внимание")},
    {STATE_SELF_CONTROL, QT_TRANSLATE_NOOP("IssoCommonData", "Самоконтроль")},
    {STATE_SHORT_CIRCUIT,QT_TRANSLATE_NOOP("IssoCommonData", "КЗ")},
    {STATE_BREAK,        QT_TRANSLATE_NOOP("IssoCommonData", "Обрыв")},
    {STATE_FAIL,         QT_TRANSLATE_NOOP("IssoCommonData", "Неисправность")},
    {STATE_UNDEFINED,    QT_TRANSLATE_NOOP("IssoCommonData", "Неизвестно")}/*,
    {STATE_OFFLINE,      QT_TRANSLATE_NOOP("IssoCommonData", "Не в сети")}*/
};


const QMap<IssoScenarioScope, QString> IssoCommonData::m_scenarioScopes =
{
    {IssoScenarioScope::FLOOR,      QT_TRANSLATE_NOOP("IssoCommonData", "Этаж")},
    {IssoScenarioScope::BUILDING,   QT_TRANSLATE_NOOP("IssoCommonData", "Здание")}
};


const QMap<IssoFixedModuleBtnFunc, QString> IssoCommonData::m_fixedModuleBtnFuncs =
{
//    {IssoFixedModuleBtnFunc::CAMERA,    QT_TRANSLATE_NOOP("IssoCommonData", "Камера")},
    {IssoFixedModuleBtnFunc::VOICE,     QT_TRANSLATE_NOOP("IssoCommonData", "Голос")}
};


const QMap<IssoState, quint8> IssoCommonData::m_statePriorities =
{
    {STATE_ALARM,   0},
    {STATE_WARNING, 1},
    {STATE_BREAK,   2},
    {STATE_FAIL,    3},
    {STATE_NORMAL,  4}
};



QDataStream &operator<<(QDataStream &out, const IssoModuleState &state)
{
    out << (qint32&)state;
    return out;
}
QDataStream &operator>>(QDataStream &in, IssoModuleState &state)
{
    in >> (qint32&)state;
    return in;
}


QDataStream &operator<<(QDataStream &out, IssoParamId id)
{
    quint8 byteVal = (quint8)id;
    out << byteVal;
    return out;
}

QDataStream &operator>>(QDataStream &in, IssoParamId& id)
{
    quint8 byteVal;
    in >> byteVal;
    id = static_cast<IssoParamId>(byteVal);
    return in;
}


QDataStream &operator<<(QDataStream &out, const IssoState& state)
{
    out << (qint32&)state;
    return out;
}

QDataStream &operator>>(QDataStream &in, IssoState& state)
{
    in >> (qint32&)state;
    return in;
}


QDataStream &operator<<(QDataStream &out, IssoDigitalSensorId id)
{
    out << (qint32&)id;
    return out;
}

QDataStream &operator>>(QDataStream &in, IssoDigitalSensorId& id)
{
    in >> (qint32&)id;
    return in;
}


QDataStream &operator<<(QDataStream &out, IssoStateCondition condition)
{
    out << (qint32&)condition;
    return out;
}

QDataStream &operator>>(QDataStream &in, IssoStateCondition& condition)
{
    in >> (qint32&)condition;
    return in;
}


QDataStream &operator<<(QDataStream &out, IssoChainType type)
{
    out << (qint32&)type;
    return out;
}

QDataStream &operator>>(QDataStream &in, IssoChainType& type)
{
    in >> (qint32&)type;
    return in;
}


QDataStream &operator<<(QDataStream &out, IssoScenarioScope scope)
{
    out << (qint32&)scope;
    return out;
}

QDataStream &operator>>(QDataStream &in, IssoScenarioScope &scope)
{
    in >> (qint32&)scope;
    return in;
}



IssoStateRange::IssoStateRange() : IssoStateRange(0, 0)
{
}


IssoStateRange::IssoStateRange(double min, double max)
    : m_min(min),
      m_max(max)
{
}


double IssoStateRange::max() const
{
    return m_max;
}


void IssoStateRange::setMax(double max)
{
    m_max = max;
}


QDataStream& operator<<(QDataStream &out, const IssoStateRange &range)
{
    out << range.m_min;
    out << range.m_max;
    return out;
}


QDataStream& operator>>(QDataStream &in, IssoStateRange &range)
{
    in >> range.m_min;
    in >> range.m_max;
    return in;
}



double IssoStateRange::min() const
{
    return m_min;
}


void IssoStateRange::setMin(double min)
{
    m_min = min;
}


IssoCmdId IssoCommonData::cmdIdByString(const QString &strValue)
{
    return m_cmdIds.key(strValue);
}


QString IssoCommonData::stringByCmdId(const IssoCmdId &id)
{
    return m_cmdIds.value(id, "");
}


IssoParamId IssoCommonData::paramIdByString(const QString &strValue)
{
    return m_paramIds.key(strValue);
}


QString IssoCommonData::stringByParamId(const IssoParamId &id)
{
    return qApp->translate("IssoCommonData", m_paramIds.value(id).toUtf8().data());
}


QStringList IssoCommonData::eventSourceNames()
{
    QList<IssoParamId> eventSourceIds =
    {
        BTN_FIRE, BTN_SECURITY, SENSOR_SMOKE, SENSOR_CO,
        ANALOG_CHAIN_1, ANALOG_CHAIN_2, ANALOG_CHAIN_3,
        ANALOG_CHAIN_4, ANALOG_CHAIN_5,ANALOG_CHAIN_6,
        INT_RELAY_1, INT_RELAY_2, INT_RELAY_3,
        INT_RELAY_4, INT_RELAY_5, INT_RELAY_6,
        VOLT_BATTERY, VOLT_BUS, VOLT_SENSORS,
        SUPPLY_EXTERNAL, SUPPLY_INTERNAL, TEMP_OUT,
        VIBRATION, SENSOR_OPEN, MULTISENSOR,
        EXT_RELAY_1, EXT_RELAY_2, EXT_RELAY_3,
        EXT_RELAY_4, EXT_RELAY_5
    };

    QStringList list;
    foreach (IssoParamId id, eventSourceIds)
    {
        list << stringByParamId(id);
    }
    return list;
}


IssoDigitalSensorId IssoCommonData::digitalSensorIdByString(const QString &strValue)
{
    return m_digitalSensorIds.key(strValue);
}


QString IssoCommonData::stringByDigitalSensorId(const IssoDigitalSensorId &id)
{
    return qApp->translate("IssoCommonData", m_digitalSensorIds.value(id).toUtf8().data());
}


QList<IssoDigitalSensorId> IssoCommonData::displayingDigitalSensorIds()
{
    return QList<IssoDigitalSensorId>
            {
                DIGITALSENSOR_SMOKE_O,
                DIGITALSENSOR_TEMP_A,
                DIGITALSENSOR_TEMP_D,
                DIGITALSENSOR_SMOKE_E,
                DIGITALSENSOR_FLAME_STD_DEV,
                DIGITALSENSOR_CO,
                DIGITALSENSOR_VOC
            };
}


QString IssoCommonData::stringByModuleState(const IssoModuleState &state)
{
    return m_moduleStates.value(state, "");
}


IssoModuleState IssoCommonData::moduleStateByString(const QString &stateName)
{
    return m_moduleStates.key(stateName, UNKNOWN);
}


QString IssoCommonData::stringBySensorState(const IssoState &state)
{
    return m_paramStates.value(state, "");
}


IssoState IssoCommonData::sensorStateByString(const QString &stateName)
{
    return m_paramStates.key(stateName, STATE_UNDEFINED);
}


QString IssoCommonData::stringByScenarioScope(const IssoScenarioScope &scope)
{
    return m_scenarioScopes.value(scope);
}


IssoScenarioScope IssoCommonData::scenarioScopeByString(const QString &scopeName)
{
    return m_scenarioScopes.key(scopeName);
}


QStringList IssoCommonData::scenarioScopeStrings()
{
    return m_scenarioScopes.values();
}


QString IssoCommonData::stringByFixedModuleBtnFunc(IssoFixedModuleBtnFunc func)
{
    return m_fixedModuleBtnFuncs.value(func, "");
}


IssoFixedModuleBtnFunc IssoCommonData::fixedModuleBtnFuncByString(const QString &funcName)
{
    return m_fixedModuleBtnFuncs.key(funcName);
}


QStringList IssoCommonData::fixedModuleBtnFuncStrings()
{
    return m_fixedModuleBtnFuncs.values();
}


bool IssoCommonData::isFixedModuleBtnFunc(const QString &funcName)
{
    return m_fixedModuleBtnFuncs.values().contains(funcName);
}


QRegExpValidator *IssoCommonData::makeIpValidator(QObject *parent)
{
    QString ipRange = "(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])";
    QRegExp ipRegex ("^" + ipRange
                     + "\\." + ipRange
                     + "\\." + ipRange
                     + "\\." + ipRange + "$");
    QRegExpValidator *ipValidator = new QRegExpValidator(ipRegex, parent);
    return ipValidator;
}


QByteArray IssoCommonData::convertImageToByteArray(const QImage &image,
                                                   const char *imageFormat)
{
    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, imageFormat);
    return ba;
}


QImage IssoCommonData::convertBase64ToImage(const QString &base64String,
                                            const char *imageFormat)
{
    // преобразовать строку в массив данных изображения
    QByteArray ba = QByteArray::fromBase64(base64String.toUtf8());
    // сформировать изображение
    QImage image = QImage::fromData(ba, imageFormat);
    return (image.isNull() ? QImage() : image);
}


QString IssoCommonData::convertImageToBase64(const QImage &image,
                                             const char *imageFormat)
{
    // записать фоновое изображение в массив
    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, imageFormat);
    // преобразовать изображение в строку
    return QString::fromUtf8(ba.toBase64().data());
}


QString IssoCommonData::getAppTypeName(IssoAppType appType)
{
    QString appName;
    switch(appType)
    {
        case IssoAppType::SNK:
        {
            appName = QT_TRANSLATE_NOOP("IssoCommonData", "Система наблюдения и контроля (СНК)");
            break;
        }
        case IssoAppType::SOO:
        {
            appName = QT_TRANSLATE_NOOP("IssoCommonData", "Система обслуживания объекта (СОО)");
            break;
        }
        case IssoAppType::GSC:
        {
            appName = QT_TRANSLATE_NOOP("IssoCommonData", "Глобальный ситуационный центр (ГСЦ)");
            break;
        }
        default:
        {
            appName = "";
            break;
        }
    }
    return appName;
}


QList<QHostAddress> IssoCommonData::localIps()
{
    QList<QHostAddress> list;
    foreach (const QHostAddress& ip, QNetworkInterface::allAddresses())
    {
        if ((ip.protocol() == QAbstractSocket::IPv4Protocol) &&
                (ip != QHostAddress(QHostAddress::LocalHost)))
            list << ip;
    }
    return list;
}


IssoModuleState IssoCommonData::moduleStateByParamState(IssoState paramState)
{
    IssoModuleState moduleState;
    switch(paramState)
    {
        // тревога
        case STATE_ALARM:
        case STATE_WARNING:
        {
            moduleState = ALARM;
            break;
        }
        // неисправен
        case STATE_SHORT_CIRCUIT:
        case STATE_BREAK:
        case STATE_FAIL:
        case STATE_ERROR:
        case VOLTAGE_LOW:
        {
            moduleState = FAIL;
            break;
        }
        // активен
        case STATE_NORMAL:
        case STATE_SELF_CONTROL:
        case VOLTAGE_MEDIUM:
        case VOLTAGE_HIGH:
        {
            moduleState = ACTIVE;
            break;
        }
        // неактивен
        case STATE_UNDEFINED:
        {
            moduleState = INACTIVE;
            break;
        }
        default:
        {
            moduleState = FAIL; // 2020-08-24 ACHERNY
            //moduleState = ACTIVE;
            break;
        }
    }
    return moduleState;
}


IssoState IssoCommonData::higherPriorityState(IssoState state1, IssoState state2)
{
    // получить приоритеты состояний
    quint8 prior1 = m_statePriorities.value(state1, 0xFF);
    quint8 prior2 = m_statePriorities.value(state2, 0xFF);
    // вернуть состояние с высшим приоритетом (значение которого меньше)
    return (prior1 < prior2 ? state1 : state2);
}


QColor IssoCommonData::backgroundByModuleState(IssoModuleState state)
{
    QColor color;
    switch(state)
    {
        case ALARM:
        {
            color = QColor(Qt::red);
            break;
        }
        case FAIL:
        {
            color = QColor(Qt::yellow);
            break;
        }
        case ACTIVE:
        {
            color = QColor(0, 150, 50);
            break;
        }
        case INACTIVE:
        {
//            color = QColor(230, 230, 230);
            color = QColor(Qt::darkGray);
            break;
        }
        case UNKNOWN:
        {
            //color = QColor(255, 128, 0, 250); //  CHA 16.09.2020
             color = QColor(Qt::yellow);
            break;
        }
        default:
        {
            color = QColor(Qt::transparent);
            break;
        }
    }
    return color;
}


QColor IssoCommonData::foregroundByModuleState(IssoModuleState state)
{
    QColor color;
    switch(state)
    {
        case ALARM:
        case ACTIVE:
        case INACTIVE:
        {
            color = QColor(Qt::white);
            break;
        }
        case FAIL:
        case UNKNOWN:
//        case INACTIVE:
        {
            color = QColor(Qt::black);
            break;
        }
        default:
        {
            color = QColor(Qt::black);
            break;
        }
    }
    return color;
}

/**
 * @brief IssoCommonData::backgroundByState
 * отображает цвет элемента в зависимости от его состояния
 * @param state
 * @return
 */
QColor IssoCommonData::backgroundByState(IssoState state)
{
    QColor color(Qt::transparent);
    switch (state)
    {
        /// тревога QColor(Qt::red)
        case STATE_ALARM:
        /// внимание QColor(Qt::red)
        case STATE_WARNING:
        {
            color = QColor(Qt::red);
            break;
        }
        /// короткое замыкание QColor(Qt::yellow)
        case STATE_SHORT_CIRCUIT:
        /// обрыв линии QColor(Qt::yellow)
        case STATE_BREAK:
        /// неисправность QColor(Qt::yellow)
        case STATE_FAIL:
        /// ошибка QColor(Qt::yellow)
        case STATE_ERROR:
        /// низкий уровень напряжения QColor(Qt::yellow)
        case VOLTAGE_LOW:
        {
            color = QColor(Qt::yellow);
            break;
        }
        /// нормальное состояние (Qt::darkGreen)
        case STATE_NORMAL:
        /// самоконтроль (Qt::darkGreen)
        case STATE_SELF_CONTROL:
        /// средний и высокий уровень напряжения (Qt::darkGreen)
        case VOLTAGE_MEDIUM:
        case VOLTAGE_HIGH:
        {
            color = QColor(Qt::darkGreen);
            break;
        }
        /// неопределенное состояние (исходное, модуль не выбран),
        /// либо нет связи QColor(230, 230, 230)
        case STATE_UNDEFINED:
        {
            color = QColor(230, 230, 230);
            break;
        }
        /// сервисное обслуживание QColor(Qt::blue)
        case STATE_SERVICE:
        {
//            color = QColor(255, 128, 0, 250);
            color = QColor(Qt::blue);
            break;
        }
        default:
            break;
    }
    return color;
}


QColor IssoCommonData::foregroundByState(IssoState state)
{
    QColor color(Qt::transparent);
    switch (state)
    {
        // тревога
        case STATE_ALARM:
        // внимание
        case STATE_WARNING:
        //
        // сервисное обслуживание
        case STATE_SERVICE:
        {
            color = QColor(Qt::white);
            break;
        }
        // короткое замыкание
        case STATE_SHORT_CIRCUIT:
        // обрыа линии
        case STATE_BREAK:
        // неисправность
        case STATE_FAIL:
        // ошибка
        case STATE_ERROR:
        // низкий уровень напряжения
        case VOLTAGE_LOW:
        {
            color = QColor(Qt::black);
            break;
        }
        // нормальное состояние
        case STATE_NORMAL:
        // самоконтроль
        case STATE_SELF_CONTROL:
        // средний и высокий уровень напряжения
        case VOLTAGE_MEDIUM:
        case VOLTAGE_HIGH:
        {
            color = QColor(Qt::white);
            break;
        }
        // неопределенное состояние (исходное, модуль не выбран),
        // либо нет связи
        case STATE_UNDEFINED:
        {
            color = QColor(Qt::black);
            break;
        }
        default:
            break;
    }
    return color;
}


QString IssoCommonData::makeMultiSensorName(int id)
{
    return QString("%1-%2")
                    .arg(stringByParamId(MULTISENSOR))
                    .arg(id);
}


QString IssoCommonData::showInputTextDialog(QWidget* parent, const QString &title,
                                            const QString &label, bool &ok)
{
    // создать диалог с кнопкой закрытия окна
    QInputDialog dlg(parent, Qt::WindowCloseButtonHint);
    // задать минимальную ширину
    dlg.setMinimumWidth(400);
    dlg.resize(dlg.size());
    // задать заголовок окна
    dlg.setWindowTitle(title);
    // задать тип ввода - текст
    dlg.setInputMode(QInputDialog::TextInput);
    // задать текст лейбла
    dlg.setLabelText(label);
    // перевести надписи на кнопках
    dlg.setOkButtonText(QT_TRANSLATE_NOOP("IssoCommonData", "ОК"));
    dlg.setCancelButtonText(QT_TRANSLATE_NOOP("IssoCommonData", "Отмена"));
    // ок, если нажата кнопка ОК
    ok = (dlg.exec() == QDialog::Accepted);
    // если нажата кнопка ОК, вернуть введенный текст,
    // иначе пустую строку
    return (ok ? dlg.textValue() : "");
}


int IssoCommonData::showInputNumberDialog(QWidget *parent, const QString &title,
                                       const QString &label, bool &ok,
                                       int min, int max, int defaultValue)
{
    // создать диалог с кнопкой закрытия окна
    QInputDialog dlg(parent, Qt::WindowCloseButtonHint);
    // задать минимальную ширину
    dlg.setMinimumWidth(250);
    dlg.resize(dlg.size());
    dlg.setLayoutDirection(Qt::LeftToRight);
    // задать заголовок окна
    dlg.setWindowTitle(title);
    // задать тип ввода - текст
    dlg.setInputMode(QInputDialog::IntInput);
    // задать диапазон допустимых значений
    dlg.setIntRange(min, max);
    // задать значение по умолчанию
    dlg.setIntValue(defaultValue);
    // задать текст лейбла
    dlg.setLabelText(label);
    // перевести надписи на кнопках
    dlg.setOkButtonText(QT_TRANSLATE_NOOP("IssoCommonData", "ОК"));
    dlg.setCancelButtonText(QT_TRANSLATE_NOOP("IssoCommonData", "Отмена"));
    // ок, если нажата кнопка ОК
    ok = (dlg.exec() == QDialog::Accepted);
    // если нажата кнопка ОК, вернуть введенный текст,
    // иначе пустую строку
    return (ok ? dlg.intValue() : -1);
}


QString IssoCommonData::showSelectItemDialog(QWidget *parent, const QString &title,
                                             const QString &label, const QStringList &items,
                                             bool &ok)
{
    // создать диалог с кнопкой закрытия окна
    QInputDialog dlg(parent, Qt::WindowCloseButtonHint);
    // задать минимальную ширину
    dlg.setMinimumWidth(400);
    dlg.resize(dlg.size());
    // задать заголовок окна
    dlg.setWindowTitle(title);

//    // задать тип ввода - текст
//    dlg.setInputMode(QInputDialog::TextInput);

    // задать текст лейбла
    dlg.setLabelText(label);
    dlg.setComboBoxEditable(false);
    // задать элементов комбобокса
    dlg.setComboBoxItems(items);
    // перевести надписи на кнопках
    dlg.setOkButtonText(QT_TRANSLATE_NOOP("IssoCommonData", "ОК"));
    dlg.setCancelButtonText(QT_TRANSLATE_NOOP("IssoCommonData", "Отмена"));
    // ок, если нажата кнопка ОК
    ok = (dlg.exec() == QDialog::Accepted);
    // если нажата кнопка ОК, вернуть введенный текст,
    // иначе пустую строку
    return (ok ? dlg.textValue() : "");
}


bool IssoCommonData::showQuestionDialog(QWidget *parent,
                                        const QString &title, const QString &label)
{
    return QMessageBox::question(parent, title, label,
                                 QT_TRANSLATE_NOOP("IssoCommonData","Да"),
                                 QT_TRANSLATE_NOOP("IssoCommonData","Нет"),
                                 QString(), 0, 1) == 0;
}


QProgressDialog *IssoCommonData::makeProgressDialog(const QString &title, const QString &label,
                                                    int min, int max, QWidget *parent)
{
    QProgressDialog* dlg = new QProgressDialog(label, QString(), min, max, parent);
    dlg->setValue(min);
    dlg->setWindowTitle(title);
    dlg->setMinimumWidth(400);
    dlg->setWindowModality(Qt::WindowModal);
    dlg->setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    return dlg;
}


void IssoCommonData::printMsg(const QString &msg)
{

}


QHostAddress IssoCommonData::incrementIp(const QHostAddress &ip)
{
    // преобразовать в int
    quint32 curIpValue = ip.toIPv4Address();
    // инкрементировать
    curIpValue++;
    // получить младший разряд
    quint8 lsb = curIpValue & 0xFF;
    // младший разряд не может быть равен 0, 1, 255
    switch(lsb)
    {
        case 0:
            curIpValue += 2;
            break;

        case 1:
        case 255:
            curIpValue++;
            break;

        default:
            break;
    }
    // преобразовать из BigEndian
    quint32 nextIpValue = curIpValue;
    // преобразовать в адрес
    return QHostAddress(nextIpValue);
}

