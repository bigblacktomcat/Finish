#ifndef ISSOCONFIGUTILS_H
#define ISSOCONFIGUTILS_H

#include "issodatabase.h"
#include "issoobject.h"


class IssoConfigUtils
{
    public:
        static const QString AUTORUN_FILENAME;

        static int createConfig(IssoDataBase *db);
        static bool insertConfig(IssoDataBase *db, const QByteArray& configData);
        static bool deleteConfig(IssoDataBase *db, int objectId);
        static IssoObject* loadObject(IssoDataBase *db, int objectId);
        static QList<int> getObjectIds(IssoDataBase *db);
        static int getFirstObjectId(IssoDataBase *db);
        static QMap<int, QString> getObjectNames(IssoDataBase *db);
        static bool updateConfig(IssoDataBase *db, IssoObject* obj);
        static bool objectExists(IssoDataBase *db, int objectId);

        static void createAutorunFile();
        static void deleteAutorunFile();
        static bool needAutorun();
    signals:

    public slots:
};

#endif // ISSOCONFIGUTILS_H
