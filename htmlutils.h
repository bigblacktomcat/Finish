#ifndef HTMLUTILS_H
#define HTMLUTILS_H

#include <QObject>
#include "issocommondata.h"


class HtmlUtils
{
    private:
        static const QMap<IssoState, QString> m_htmlClassNames;

    public:
        static QString makeAnalogChainHint(int moduleId, int chainId, IssoState chainState);
        static QString makeMultiSensorHint(const QString &moduleName, quint8 multiSensorId,
                                           IssoState multiSensorState,
                                           const QString& location,
                                           const QMap<IssoDigitalSensorId, IssoState>& digitalStates);
        static QString makeMultiSensorExtHint(const QString &moduleName, /*int chainId,*/
                                              quint8 multiSensorId, IssoState multiSensorState,
                                              const QString& location,
                                              const QMap<IssoDigitalSensorId, IssoState>& digitalStates,
                                              const QMap<IssoDigitalSensorId, quint16> &digitalValues);

};

#endif // HTMLUTILS_H
