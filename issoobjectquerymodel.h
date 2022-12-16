#ifndef ISSOOBJECTQUERYMODEL_H
#define ISSOOBJECTQUERYMODEL_H

#include <QSqlQueryModel>



class IssoObjectQueryModel : public QSqlQueryModel
{
    private:
        mutable QMap<QString, bool> m_objectSounds;

    public:
        static const int COLUMN_SOUND = 12;

        enum ObjectColName
        {
            COL_ID,
            COL_OBJ_NAME,
            COL_CONTACT,
            COL_PHONE,
            COL_ADDRESS,
            COL_STATE,
            COL_MODULES_TOTAL,
            COL_MODULES_ALARM,
            COL_MS_TOTAL,
            COL_MS_ALARM,
            COL_CHANNELS_TOTAL,
            COL_CHANNELS_ALARM,
            COL_SOUND
        };

        IssoObjectQueryModel();

        virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
        virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
        virtual QVariant headerData(int section, Qt::Orientation orientation,
                                    int role = Qt::DisplayRole) const override;

        virtual bool setData(const QModelIndex &index,
                             const QVariant &value, int role = Qt::EditRole) override;

};

#endif // ISSOOBJECTQUERYMODEL_H
