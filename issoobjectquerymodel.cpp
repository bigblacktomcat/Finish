#include "issoobjectquerymodel.h"

IssoObjectQueryModel::IssoObjectQueryModel()
{

}


int IssoObjectQueryModel::columnCount(const QModelIndex &parent) const
{
    return QSqlQueryModel::columnCount(parent) + 1;
}


QVariant IssoObjectQueryModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
    {
        // если столбец Звук
        if (index.column() == COL_SOUND/*COLUMN_SOUND*/)
        {
            // Наименование
            QString objectName = QSqlQueryModel::data(this->index(index.row(), COL_OBJ_NAME/*1*/)).toString();
            // если данных о звуке объекта нет в коллекции, добавить "Звук включен"
            if (!m_objectSounds.keys().contains(objectName))
                m_objectSounds.insert(objectName, true);
            // получить состояние звука
            bool soundOn = m_objectSounds.value(objectName);
            // вернуть состояние звука
            return (soundOn ? "Вкл." : "Выкл.");
        }

    }
    // обработка по умолчанию
    return QSqlQueryModel::data(index, role);
}


QVariant IssoObjectQueryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if ((orientation == Qt::Horizontal) && (role == Qt::DisplayRole))
    {
        // заголовки столбцов
        QString headerTitle = "";
        switch(section)
        {
            case COL_OBJ_NAME/*1*/:
            {
                headerTitle = tr("Наименование");
                break;
            }
            case COL_CONTACT/*2*/:
            {
                headerTitle = tr("Контактное лицо");
                break;
            }
            case COL_PHONE/*3*/:
            {
                headerTitle = tr("Телефон");
                break;
            }
            case COL_ADDRESS/*4*/:
            {
                headerTitle = tr("Адрес");
                break;
            }
            case COL_STATE/*5*/:
            {
                headerTitle = tr("Состояние");
                break;
            }
            case COL_MODULES_TOTAL/*6*/:
            {
                headerTitle = tr("Модули (Все)");
                break;
            }
            case COL_MODULES_ALARM/*7*/:
            {
                headerTitle = tr("Модули (Тревоги)");
                break;
            }
            case COL_MS_TOTAL/*8*/:
            {
                headerTitle = tr("ИПМ (Все)");
                break;
            }
            case COL_MS_ALARM/*9*/:
            {
                headerTitle = tr("ИПМ (Тревоги)");
                break;
            }
            case COL_CHANNELS_TOTAL/*10*/:
            {
                headerTitle = tr("Каналы (Все)");
                break;
            }
            case COL_CHANNELS_ALARM/*11*/:
            {
                headerTitle = tr("Каналы (Тревоги)");
                break;
            }
//            case COLUMN_SOUND:
            case COL_SOUND:
            {
                headerTitle = tr("Звук");
                break;
            }
            default:
                break;
        }
        return QVariant::fromValue(headerTitle);
    }
    return QSqlQueryModel::headerData(section, orientation, role);
}


bool IssoObjectQueryModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    // если столбец Звук
    if (index.column() == COL_SOUND/*COLUMN_SOUND*/)
    {
        // Наименование
        QString objectName = QSqlQueryModel::data(this->index(index.row(), COL_OBJ_NAME/*1*/)).toString();

        bool soundOn = value.toBool();
        m_objectSounds.insert(objectName, soundOn);
        return true;
    }
    return QSqlQueryModel::setData(index, value, role);
}
