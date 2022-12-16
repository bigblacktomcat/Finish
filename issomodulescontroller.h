#ifndef ISSOMODULESCONTROLLER_H
#define ISSOMODULESCONTROLLER_H

#include "issobuilding.h"
#include "issomodule.h"
#include "issomoduledescriptor.h"
#include "issomodulereply.h"
#include "issomodulerequest.h"
#include "issoobject.h"
#include "issosensortype.h"

#include <QObject>
#include <QMap>


/*!
 * \brief   Класс, отвечающий за обработку ответных сообщений от модуля МАСО
 */
class IssoModulesController : public QObject
{
        Q_OBJECT

    private:
        IssoObject* m_object;

        /// \brief      Проанализировать параметры ответной команды
        /// \details    Метод анализирует каждый параметр на вхождение
        ///             в диапазон допустимых значений. Если параметр выходит за
        ///             рамки штатных значений, он добавляется в список тревожных
        ///             параметров модуля, и наоборот, если входит, то удаляется
        ///             из этого списка. При этом, если статус параметра поменялся,
        ///             инициируется соответствующее событие (тревога обнаружена / сброшена).
        ///             Если после сброса тревог, у модуля не осталось тревожных
        ///             параметров, инициируется событие сброса всех тревог.
        void analyzeInfoReply(const IssoModuleReply& reply);
        /// \brief  Проанализировать состояние реле
        void analyzeRelayReply(const IssoModuleReply& reply);
        /// \brief  Проанализировать дескриптор
        void analyzeDescriptorReply(const IssoModuleReply &reply);
        /// \brief  Обработать подтверждение установки опрашиваемых адресов мультидатчиков
        void analyzeMsPollAddrs(const IssoModuleReply& reply);
        /// \brief  Обработать данные одиночного регистра мультидатчика
        void analyzeMsSingleRegSetReply(const IssoModuleReply& reply);
        /// \brief  Обработать таймаут запроса одиночного регистра мультидатчика
        void analyzeMsSingleRegSetTimeOut(const IssoModuleRequest& request);

        /// \brief  Обработать результат чтения блока регистров мультидатчика
        void analyzeMsBlockRegGetReply(const IssoModuleReply& reply);
        /// \brief  Обработать таймаут записи блока регистров мультидатчика
        void analyzeMsBlockRegGetTimeOut(const IssoModuleRequest& request);
        //
        /// \brief  Обработать результат записи блока регистров мультидатчика
        void analyzeMsBlockRegSetReply(const IssoModuleReply& reply);
        /// \brief  Обработать таймаут записи блока регистров мультидатчика
        void analyzeMsBlockRegSetTimeOut(const IssoModuleRequest& request);


        /// \brief  Найти модуль по адресу
        IssoModule* findModuleByAddress(QHostAddress addr);
        /// \brief  Найти здание по номеру
        IssoBuilding* findBuildingByNumber(int number);


        /// \brief  Найти модуль по имени
        IssoModule* findModuleByDisplayName(const QString& displayName);
        void parseParamBuffer(IssoModule* module, const IssoParamBuffer& buffer);
        void parseStateParam(IssoModule* module, const QSharedPointer<IssoParam> &stateParamPtr);

        void resetMsState(const QHostAddress& moduleIp, int msId);
        bool saveThresholdData(const QHostAddress &moduleId, int multiSensorId,
                               const QByteArray& thresholdData);

    public:
        explicit IssoModulesController(QObject *parent = nullptr);
        ~IssoModulesController();

        IssoObject *object() const;
        void setObject(IssoObject *object);

    signals:
        void descriptorRead(const QString& moduleName,
                            const IssoModuleDescriptor& descr);
        void descriptorWritten(const QString& moduleName,
                               const IssoModuleDescriptor& descr);
        void descriptorFailed(const QString& moduleName, IssoCmdId cmdId);


        void sensorStatesChanged(const QString& moduleName);
        void relayStateChanged(const QString& moduleName, IssoParamId paramId, bool pressed);
        void moduleStateChanged(const QString& moduleName, IssoModuleState state);

        void multiSensorRawDataReceived(const QString& moduleName,
                                        quint8 multiSensorId, IssoState multiSensorState,
                                        const QMap<IssoDigitalSensorId, IssoState>& sensorStates,
                                        const QMap<IssoDigitalSensorId, quint16>& sensorValues);
        void multiSensorPollingAddrsReceived(int moduleId, const QByteArray& addrs);
//        void multiSensorRegisterInfoReceived(int moduleId, quint8 multiSensorId,
//                                             quint8 regNumber, quint16 regValue,
//                                             IssoCmdId cmdId);
        void multiSensorRegActionCompleted(IssoMsRegAction action,
                                             const QHostAddress& moduleIp,
                                             quint8 msId, bool success);

    public slots:
        void processReplyReceived(IssoModuleReply reply, bool ok);
        void processReplyTimedOut(const IssoModuleRequest& request);
        void processAlarmReceived(IssoModuleReply reply);
        void updateModuleParams(const QString& moduleName,
                                const IssoParamBuffer& paramBuffer);
};

#endif // ISSOMODULESCONTROLLER_H
