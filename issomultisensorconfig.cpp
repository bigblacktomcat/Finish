#include "issomultisensorconfig.h"


IssoMultiSensorConfig::IssoMultiSensorConfig(quint8 id)
    : m_id(id)
{
    initDigitalSensorCfgs();
//    m_digitalSensorConfigs.insert(DIGITALSENSOR_SMOKE_O,
//                                  IssoDigitalSensorConfig(DIGITALSENSOR_SMOKE_O));
//    m_digitalSensorConfigs.insert(DIGITALSENSOR_TEMP_A,
//                                  IssoDigitalSensorConfig(DIGITALSENSOR_TEMP_A));
//    m_digitalSensorConfigs.insert(DIGITALSENSOR_TEMP_D,
//                                  IssoDigitalSensorConfig(DIGITALSENSOR_TEMP_D));
//    m_digitalSensorConfigs.insert(DIGITALSENSOR_SMOKE_E,
//                                  IssoDigitalSensorConfig(DIGITALSENSOR_SMOKE_E));
//    m_digitalSensorConfigs.insert(DIGITALSENSOR_FLAME_MIN_VAL,
//                                  IssoDigitalSensorConfig(DIGITALSENSOR_FLAME_MIN_VAL));
//    m_digitalSensorConfigs.insert(DIGITALSENSOR_FLAME_STD_DEV,
//                                  IssoDigitalSensorConfig(DIGITALSENSOR_FLAME_STD_DEV));
//    m_digitalSensorConfigs.insert(DIGITALSENSOR_CO,
//                                  IssoDigitalSensorConfig(DIGITALSENSOR_CO));
//    m_digitalSensorConfigs.insert(DIGITALSENSOR_VOC,
//                                  IssoDigitalSensorConfig(DIGITALSENSOR_VOC));
}


IssoMultiSensorConfig::~IssoMultiSensorConfig()
{
    uninitDigitalSensorCfgs();
//    qDebug() << "~IssoMultiSensorConfig -" <<  m_id << endl;
}


IssoMsType IssoMultiSensorConfig::getMsType() const
{
    return m_msType;
}


void IssoMultiSensorConfig::setMsType(const IssoMsType &msType)
{
    m_msType = msType;
}


int IssoMultiSensorConfig::channelCount()
{
    return IssoMsUtils::channelCount(m_msType);
}


void IssoMultiSensorConfig::initDigitalSensorCfgs()
{
    m_digitalSensorConfigs.insert(DIGITALSENSOR_SMOKE_O,
                                  new IssoDigitalSensorConfig(DIGITALSENSOR_SMOKE_O));
    m_digitalSensorConfigs.insert(DIGITALSENSOR_TEMP_A,
                                  new IssoDigitalSensorConfig(DIGITALSENSOR_TEMP_A));
    m_digitalSensorConfigs.insert(DIGITALSENSOR_TEMP_D,
                                  new IssoDigitalSensorConfig(DIGITALSENSOR_TEMP_D));
    m_digitalSensorConfigs.insert(DIGITALSENSOR_SMOKE_E,
                                  new IssoDigitalSensorConfig(DIGITALSENSOR_SMOKE_E));
    m_digitalSensorConfigs.insert(DIGITALSENSOR_FLAME_MIN_VAL,
                                  new IssoDigitalSensorConfig(DIGITALSENSOR_FLAME_MIN_VAL));
    m_digitalSensorConfigs.insert(DIGITALSENSOR_FLAME_STD_DEV,
                                  new IssoDigitalSensorConfig(DIGITALSENSOR_FLAME_STD_DEV));
    m_digitalSensorConfigs.insert(DIGITALSENSOR_CO,
                                  new IssoDigitalSensorConfig(DIGITALSENSOR_CO));
    m_digitalSensorConfigs.insert(DIGITALSENSOR_VOC,
                                  new IssoDigitalSensorConfig(DIGITALSENSOR_VOC));
}


void IssoMultiSensorConfig::uninitDigitalSensorCfgs()
{
    // удалить все конфиги
    foreach (auto cfg, m_digitalSensorConfigs)
        delete cfg;
    // очистить список
    m_digitalSensorConfigs.clear();
}


QMap<IssoDigitalSensorId, IssoDigitalSensorConfig *> IssoMultiSensorConfig::digitalSensorConfigs() const
{
    return m_digitalSensorConfigs;
}


//void IssoMultiSensorConfig::setDigitalSensorConfigs(const QMap<IssoDigitalSensorId, IssoDigitalSensorConfig> &digitalConfigs)
//{
//    m_digitalSensorConfigs = digitalConfigs;
//}


//void IssoMultiSensorConfig::insertDigitalSensorConfig(const IssoDigitalSensorConfig &cfg)
//{
//    m_digitalSensorConfigs.insert(cfg.id(), cfg);
//}


//void IssoMultiSensorConfig::removeDigitalSensorConfig(IssoDigitalSensorId cfgId)
//{
//    m_digitalSensorConfigs.remove(cfgId);
//}


IssoDigitalSensorConfig *IssoMultiSensorConfig::digitalSensorConfig(IssoDigitalSensorId id)
{
    return m_digitalSensorConfigs.value(id);
}


quint16 IssoMultiSensorConfig::smokeOptSens() const
{
    return m_smokeOptSens;
}


void IssoMultiSensorConfig::setSmokeOptSens(const quint16 &smokeOptSens)
{
    m_smokeOptSens = smokeOptSens;
}


quint16 IssoMultiSensorConfig::smokeOptBright() const
{
    return m_smokeOptBright;
}


void IssoMultiSensorConfig::setSmokeOptBright(const quint16 &smokeOptBright)
{
    m_smokeOptBright = smokeOptBright;
}


quint8 IssoMultiSensorConfig::id() const
{
    return m_id;
}


QString IssoMultiSensorConfig::displayName() const
{
    return IssoCommonData::makeMultiSensorName(m_id);
}


int IssoMultiSensorConfig::cameraId() const
{
    return m_cameraId;
}


void IssoMultiSensorConfig::setCameraId(int cameraId)
{
    m_cameraId = cameraId;
}


QString IssoMultiSensorConfig::locationDesc() const
{
    return m_locationDesc;
}


void IssoMultiSensorConfig::setLocationDesc(const QString &locationDesc)
{
    m_locationDesc = locationDesc;
}


void IssoMultiSensorConfig::toString(QTextStream &stream)
{
    stream << "  MultiSensor = " << displayName() << endl;
    stream << "    DigitalSensorCount = " << m_digitalSensorConfigs.size() << endl << endl;
    foreach (auto cfg, m_digitalSensorConfigs.values())
    {
        cfg->toString(stream);
    }
}


quint16 IssoMultiSensorConfig::sensorMask()
{
    // сформировать список подключений (биты)
    QList<quint8> maskBits;
    maskBits << (digitalSensorConfig(DIGITALSENSOR_SMOKE_O)->enabled() ? 1 : 0)
             << (digitalSensorConfig(DIGITALSENSOR_TEMP_A)->enabled() ? 1 : 0)
             << (digitalSensorConfig(DIGITALSENSOR_TEMP_D)->enabled() ? 1 : 0)
             << (digitalSensorConfig(DIGITALSENSOR_SMOKE_E)->enabled() ? 1 : 0)
             << (digitalSensorConfig(DIGITALSENSOR_FLAME_STD_DEV)->enabled() ? 1 : 0)
             << (digitalSensorConfig(DIGITALSENSOR_CO)->enabled() ? 1 : 0)
             << (digitalSensorConfig(DIGITALSENSOR_VOC)->enabled() ? 1 : 0);
    // сформировать маску
    quint16 result = 0;
    for (int i = 0; i < maskBits.size(); ++i)
        result |= (maskBits.at(i) << i);
    return result;
}


void IssoMultiSensorConfig::setSensorMask(quint16 mask)
{
    // сформировать список подключений (булевы)
    QList<bool> maskBools;
    for (int i = 0; i < 7; ++i)
        maskBools << ((mask >> i) & 0x01);
    // задать значения конфигам
    digitalSensorConfig(DIGITALSENSOR_SMOKE_O)->setEnabled(maskBools.at(0));
    digitalSensorConfig(DIGITALSENSOR_TEMP_A)->setEnabled(maskBools.at(1));
    digitalSensorConfig(DIGITALSENSOR_TEMP_D)->setEnabled(maskBools.at(2));
    digitalSensorConfig(DIGITALSENSOR_SMOKE_E)->setEnabled(maskBools.at(3));
    digitalSensorConfig(DIGITALSENSOR_FLAME_STD_DEV)->setEnabled(maskBools.at(4));
    digitalSensorConfig(DIGITALSENSOR_CO)->setEnabled(maskBools.at(5));
    digitalSensorConfig(DIGITALSENSOR_VOC)->setEnabled(maskBools.at(6));
}


QByteArray IssoMultiSensorConfig::getThresholdData()
{
    // массив
    QByteArray ba;
    // создать поток для записи
    QDataStream stream(&ba, QIODevice::WriteOnly);
    // переменные
    IssoDigitalSensorConfig* cfg;
    //
    // маска подключенных датчиков
    stream << sensorMask();
    // Дым (о) - Внимание
    // Дым (о) - Тревога
    cfg = digitalSensorConfig(DIGITALSENSOR_SMOKE_O);
    stream << cfg->stateValue(STATE_WARNING)
           << cfg->stateValue(STATE_ALARM);
    // Темп (а) - Внимание
    // Темп (а) - Тревога
    cfg = digitalSensorConfig(DIGITALSENSOR_TEMP_A);
    stream << cfg->stateValue(STATE_WARNING)
           << cfg->stateValue(STATE_ALARM);
    // Темп (ц) - Внимание
    // Темп (ц) - Тревога
    cfg = digitalSensorConfig(DIGITALSENSOR_TEMP_D);
    stream << cfg->stateValue(STATE_WARNING)
           << cfg->stateValue(STATE_ALARM);
    // Дым (э) - Внимание
    // Дым (э) - Тревога
    cfg = digitalSensorConfig(DIGITALSENSOR_SMOKE_E);
    stream << cfg->stateValue(STATE_WARNING)
           << cfg->stateValue(STATE_ALARM);
    // Пламя (мин) - Внимание
    // Пламя (мин) - Тревога
    cfg = digitalSensorConfig(DIGITALSENSOR_FLAME_MIN_VAL);
    stream << cfg->stateValue(STATE_WARNING)
           << cfg->stateValue(STATE_ALARM);
    // Пламя (сред) - Внимание
    // Пламя (сред) - Тревога
    cfg = digitalSensorConfig(DIGITALSENSOR_FLAME_STD_DEV);
    stream << cfg->stateValue(STATE_WARNING)
           << cfg->stateValue(STATE_ALARM);
    // CO - Внимание
    // CO - Тревога
    cfg = digitalSensorConfig(DIGITALSENSOR_CO);
    stream << cfg->stateValue(STATE_WARNING)
           << cfg->stateValue(STATE_ALARM);
    // ЛОВ - Внимание
    // ЛОВ - Тревога
    cfg = digitalSensorConfig(DIGITALSENSOR_VOC);
    stream << cfg->stateValue(STATE_WARNING)
           << cfg->stateValue(STATE_ALARM);
    // Дым (о) - Чувствительность
    // Дым (о) - Яркость
    stream << m_smokeOptSens
           << m_smokeOptBright;
    // вернуть результат
    return ba;
}


void IssoMultiSensorConfig::setThresholdData(const QByteArray &bytes)
{
    // создать поток для записи
    QDataStream stream(bytes);
    // переменные
    quint16 mask, warning, alarm;
    //
    // маска подключенных датчиков
    stream >> mask;
    setSensorMask(mask);
    // Дым (о) - Внимание
    // Дым (о) - Тревога
    stream >> warning >> alarm;
    digitalSensorConfig(DIGITALSENSOR_SMOKE_O)->setStateValues(warning, alarm);
    // Темп (а) - Внимание
    // Темп (а) - Тревога
    stream >> warning >> alarm;
    digitalSensorConfig(DIGITALSENSOR_TEMP_A)->setStateValues(warning, alarm);
    // Темп (ц) - Внимание
    // Темп (ц) - Тревога
    stream >> warning >> alarm;
    digitalSensorConfig(DIGITALSENSOR_TEMP_D)->setStateValues(warning, alarm);
    // Дым (э) - Внимание
    // Дым (э) - Тревога
    stream >> warning >> alarm;
    digitalSensorConfig(DIGITALSENSOR_SMOKE_E)->setStateValues(warning, alarm);
    // Пламя (мин) - Внимание
    // Пламя (мин) - Тревога
    stream >> warning >> alarm;
    digitalSensorConfig(DIGITALSENSOR_FLAME_MIN_VAL)->setStateValues(warning, alarm);
    // Пламя (сред) - Внимание
    // Пламя (сред) - Тревога
    stream >> warning >> alarm;
    digitalSensorConfig(DIGITALSENSOR_FLAME_STD_DEV)->setStateValues(warning, alarm);
    // CO - Внимание
    // CO - Тревога
    stream >> warning >> alarm;
    digitalSensorConfig(DIGITALSENSOR_CO)->setStateValues(warning, alarm);
    // ЛОВ - Внимание
    // ЛОВ - Тревога
    stream >> warning >> alarm;
    digitalSensorConfig(DIGITALSENSOR_VOC)->setStateValues(warning, alarm);
    // Дым (о) - Чувствительность
    // Дым (о) - Яркость
    stream >> m_smokeOptSens >> m_smokeOptBright;
}


//QDataStream &operator<<(QDataStream &out, const IssoMultiSensorConfig& config)
//{
//    out << config.m_id;
//    out << config.m_digitalSensorConfigs;
//    out << config.m_smokeOptSens;
//    out << config.m_smokeOptBright;
//    out << config.m_cameraId;
//    out << config.m_locationDesc;
//    return out;
//}


//QDataStream &operator>>(QDataStream &in, IssoMultiSensorConfig& config)
//{
//    in >> config.m_id;
//    in >> config.m_digitalSensorConfigs;
//    in >> config.m_smokeOptSens;
//    in >> config.m_smokeOptBright;
//    in >> config.m_cameraId;
//    in >> config.m_locationDesc;
//    return in;
//}

QDataStream &operator<<(QDataStream &out, const IssoMultiSensorConfig& config)
{
    // ID конфига мультидатчика
    out << config.m_id;
    // количество встроенных датчиков
    out << config.m_digitalSensorConfigs.size();
    // встроенные датчики
    foreach (IssoDigitalSensorConfig* digitalCfg, config.m_digitalSensorConfigs)
        out << *digitalCfg;
    // расширенные настройки Дым (о)
    out << config.m_smokeOptSens;
    out << config.m_smokeOptBright;
    // ID камеры
    out << config.m_cameraId;
    // расположение
    out << config.m_locationDesc;
    // тип МД
    out << config.m_msType;
    return out;
}


QDataStream &operator>>(QDataStream &in, IssoMultiSensorConfig& config)
{
    // ID конфига мультидатчика
    in >> config.m_id;
    // количество встроенных датчиков
    int cfgCount;
    in >> cfgCount;
    // встроенные датчики
    for (int i = 0; i < cfgCount; ++i)
    {
        // загрузить конфиг датчика
        IssoDigitalSensorConfig newCfg;
        in >> newCfg;
        // найти существующий конфиг
        if (IssoDigitalSensorConfig* curCfg =
                config.digitalSensorConfig(newCfg.id()))
        {
            // скопировать новый объект в текущий
            *curCfg = newCfg;
        }
    }
    // расширенные настройки Дым (о)
    in >> config.m_smokeOptSens;
    in >> config.m_smokeOptBright;
    // ID камеры
    in >> config.m_cameraId;
    // расположение
    in >> config.m_locationDesc;
    // тип МД
    in >> config.m_msType;
    return in;
}

