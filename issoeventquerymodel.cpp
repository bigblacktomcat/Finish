#include "issoeventquerymodel.h"
#include "issocommondata.h"
#include "issoparam.h"


IssoEventQueryModel::IssoEventQueryModel(QObject *parent)
    : QSqlQueryModel(parent)
{
}



void IssoEventQueryModel::clearMsLocations()
{
    m_msLocations.clear();
}


void IssoEventQueryModel::insertMsLocation(const QString &moduleName,
                                           const QString &msName,
                                           const QString &msLocation)
{
    // добавить пару [идентификатор мультидатчика : местоположение]
    m_msLocations.insert(getMsID(moduleName, msName), msLocation);
}


QString IssoEventQueryModel::msLocation(const QString &moduleName, const QString &msName) const
{
    return m_msLocations.value(getMsID(moduleName, msName), "");
}


QString IssoEventQueryModel::getMsID(const QString &moduleName, const QString &msName) const
{
    return QString(MS_ID_FORMAT).arg(moduleName).arg(msName);
}

/**
 * @brief IssoEventQueryModel::makeMsDetailString получаем список датчиков с состяниянием "не норма"
 * @param detailsValue
 * @return получаем строку Id датчика
 */
QString IssoEventQueryModel::makeMsDetailString(quint16 detailsValue)
{
    // строка детализации состояний датчиков
    QString detailStr = "";
    // если 0, то все в Норме
    if (detailsValue == 0)
    {
        detailStr = tr("Все в норме");
    }
    else
    {
        QStringList list;
        // разобрать значение по состояниям
        auto states = IssoMultiSensorData::makeDigitalSensorStates(detailsValue);
        // записать в результат только те, что не в Норме
        QMapIterator<IssoDigitalSensorId, IssoState> it(states);
        while(it.hasNext())
        {
            it.next();
            // если состояние датчика не Норма, добавить в результат
            if (it.value() != STATE_NORMAL)
                list << QString("%1: %2").arg(IssoCommonData::stringByDigitalSensorId(it.key()))
                                         .arg(IssoCommonData::stringBySensorState(it.value()));
        }
        // преобразовать список в строку
        detailStr = list.join(";   ");
    }
    // вернуть результат
    return detailStr;
}


int IssoEventQueryModel::columnCount(const QModelIndex &parent) const
{
    return QSqlQueryModel::columnCount(parent) + 1;
}


QVariant IssoEventQueryModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
    {
        QString cellText;
        // если столбец Расположение
        if (index.column() == columnCount(QModelIndex()) - 1)
        {
            // имя модуля
            QString moduleName = QSqlQueryModel::data(this->index(index.row(), COL_MODULE/*3*/)).toString();
            // имя датчика
            QString sensorName = QSqlQueryModel::data(this->index(index.row(), COL_SENSOR/*5*/)).toString();
            // вернуть месторасположение
            return msLocation(moduleName, sensorName);
        }
        // если столбец Детализация (7 столбец)
        else if (index.column() == COL_DETAILS/*7*/)
        {
            // получить расширенное значение
            int extValue = QSqlQueryModel::data(index).toInt();
            // имя датчика
            QString sensorName = QSqlQueryModel::data(this->index(index.row(), COL_SENSOR/*5*/)).toString();
            //
            // получить ID параметра по имени датчика
            IssoParamId paramId;
            if (sensorName.contains(IssoCommonData::stringByParamId(MULTISENSOR)))
                paramId = MULTISENSOR;
            else
                paramId = IssoCommonData::paramIdByString(sensorName);
            //
            switch(paramId)
            {
                // Мультидатчик
                case MULTISENSOR:
                {
                    // отобразить детализацию встроенных датчиков
                    cellText = makeMsDetailString(extValue);
                    break;
                }
                // Уровень напряжение на АКБ
                case VOLT_BATTERY:
                // Уровень напряжение на шине 12 В
                case VOLT_BUS:
                // Уровень напряжения датчиков
                case VOLT_SENSORS:
                {
                    // отобразить напряжение
                    cellText = tr(" %1 В").arg(extValue / 10.0, 0, 'f', 1);
                    break;
                }
                // внешняя температура
                case TEMP_OUT:
                {
                    // отобразить температуру
                    cellText = tr(" %1 C\u00B0").arg(extValue / 10.0, 0, 'f', 1);
                    break;
                }
                default:
                {
                    cellText = "";
                    break;
                }
            }
            return cellText;
        }
    }
    // обработка по умолчанию
    return QSqlQueryModel::data(index, role);
}


QVariant IssoEventQueryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if ((orientation == Qt::Horizontal) && (role == Qt::DisplayRole))
    {
        // заголовки столбцов
        QString headerTitle = "";
        switch(section)
        {
            case COL_DATE:
            {
                headerTitle = tr("Дата");
                break;
            }
            case COL_TIME:
            {
                headerTitle = tr("Время");
                break;
            }
            case COL_MODULE:
            {
                headerTitle = tr("Модуль");
                break;
            }
            case COL_STATE:
            {
                headerTitle = tr("Состояние");
                break;
            }
            case COL_SENSOR:
            {
                headerTitle = tr("Датчик");
                break;
            }
            case COL_DETAILS:
            {
                headerTitle = tr("Детализация");
                break;
            }
            case COL_PHOTO:
            {
                headerTitle = tr("Фото");
                break;
            }
            case COL_LOCATION:
            {
                headerTitle = tr("Расположение");
                break;
            }
            default:
                break;
        }
        return QVariant::fromValue(headerTitle);
    }
    return QSqlQueryModel::headerData(section, orientation, role);
}


void IssoEventQueryModel::update(IssoObject *object)
{
    if (!object)
        return;
    // очистить коллекцию
    clearMsLocations();
    //
    foreach (IssoModule* module, object->locatedModules())
    {
        // получить конфиг адресного шлейфа
        auto msChainCfg = object->moduleAddressChainConfig(module->moduleConfigName());
        if (!msChainCfg)
            continue;
        // обойти конфиг каждого мультидатчика шлейфа
        foreach (IssoMultiSensorConfig* msCfg, msChainCfg->multiSensorConfigs())
        {
            insertMsLocation(module->displayName(),
                             msCfg->displayName(),
                             msCfg->locationDesc());
        }
    }
}

