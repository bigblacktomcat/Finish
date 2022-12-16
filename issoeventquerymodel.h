#ifndef ISSOEVENTQUERYMODEL_H
#define ISSOEVENTQUERYMODEL_H

#include "issoobject.h"
#include <QSqlQueryModel>

/**
 * @brief The IssoEventQueryModel class
 * описание таблицы событий
 */
class IssoEventQueryModel : public QSqlQueryModel
{
    private:
        // формат идентификатор мультидатчика в коллекции
        const QString MS_ID_FORMAT = "%1_%2";
        //
        // пары [модуль_датчик : местоположение]
        QMap<QString, QString> m_msLocations;

        void clearMsLocations();
        void insertMsLocation(const QString& moduleName,
                              const QString& msName,
                              const QString& msLocation);
        QString msLocation(const QString& moduleName,
                           const QString& msName) const;

        QString getMsID(const QString& moduleName,
                        const QString& msName) const;

        static QString makeMsDetailString(quint16 detailsValue);

    public:
        enum EventColName
        {
            COL_ID,
            COL_DATE,
            COL_TIME,
            COL_MODULE,
            COL_STATE,
            COL_SENSOR,
            COL_COMMENT,
            COL_DETAILS,
            COL_PHOTO,
            COL_LOCATION
        };

        explicit IssoEventQueryModel(QObject* parent = nullptr);

        virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
        virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
        virtual QVariant headerData(int section, Qt::Orientation orientation,
                                    int role = Qt::DisplayRole) const override;
        // обновить данные
        void update(IssoObject* object);
};

#endif // ISSOEVENTQUERYMODEL_H
