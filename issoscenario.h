#ifndef ISSOSCENARIO_H
#define ISSOSCENARIO_H

#include "issocommondata.h"


struct IssoRelayClass
{
    QString name;
    QString descr;
    int delay;

    IssoRelayClass(const QString& name = "", const QString& descr = "", int delay = 0)
    {
        this->name = name;
        this->descr = descr;
        this->delay = delay;
    }

    bool isValid()
    {
        return !name.isEmpty();
    }
    //
//    friend QDataStream &operator<<(QDataStream &out, const IssoRelayClass& relayClass);
//    friend QDataStream &operator>>(QDataStream &in, IssoRelayClass& relayClass);

    friend QDataStream& operator<<(QDataStream &out, const IssoRelayClass &relayClass)
    {
        out << relayClass.name;
        out << relayClass.descr;
        out << relayClass.delay;
        return out;
    }

    friend QDataStream& operator>>(QDataStream &in, IssoRelayClass &relayClass)
    {
        in >> relayClass.name;
        in >> relayClass.descr;
        in >> relayClass.delay;
        return in;
    }
};


struct IssoScenarioInfo
{
    QString name;
    int buildNum;
    int floorNum;

    IssoScenarioInfo(const QString& name = "", int buildNum = 0, int floorNum = 0)
    {
        this->name = name;
        this->buildNum = buildNum;
        this->floorNum = floorNum;
    }

    QString complexName() const
    {
        return QString("%1_%2_%3").arg(name)
                                  .arg(buildNum)
                                  .arg(floorNum);
    }

    bool isValid()
    {
        return (!name.isEmpty()) &&
                (buildNum != 0) &&
                (floorNum != 0);
    }
};
Q_DECLARE_METATYPE(IssoScenarioInfo)


class IssoScenario
{
    private:
        QString m_name;
        IssoScenarioScope m_scope;
        QStringList m_relayClassNames;

    public:
        IssoScenario(const QString& name = "",
                     IssoScenarioScope scope = IssoScenarioScope::FLOOR);

        /// \brief  Загрузить конфигурацию из потока
        void loadFromStream(QDataStream& stream);
        /// \brief  Сохранить конфигурацию в поток
        void saveToStream(QDataStream& stream);

        QString name() const;
        void setName(const QString &name);

        IssoScenarioScope scope() const;
        void setScope(const IssoScenarioScope &scope);

        QStringList relayClassNames() const;
        void setRelayClassNames(const QStringList &relayClassNames);

        void addRelayClassName(const QString& className);
        void removeRelayClassName(const QString& className);
        void replaceRelayClassName(const QString& oldName, const QString& newName);
        bool containsRelayClassName(const QString& className);
        void clearRelayClassNames();

        bool isValid() const;
};

#endif // ISSOSCENARIO_H
