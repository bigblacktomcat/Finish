#ifndef ISSOPARAM_H
#define ISSOPARAM_H

#include "issocommondata.h"

#include <QHostAddress>
#include <QtEndian>


//========== IssoParam ==========

class IssoParam
{
    protected:
        IssoParamId m_id;
    public:
        IssoParam();
        IssoParam(IssoParamId id);
        IssoParam(const IssoParam& other);
        virtual ~IssoParam();

        virtual bool loadFromStream(QDataStream *stream);
        virtual void saveToStream(QDataStream *stream);
        virtual int size();
        virtual QString description();

        IssoParamId id() const;
        QByteArray bytes();
};
Q_DECLARE_METATYPE(IssoParam)



//========== IssoSensorStateParam ==========

class IssoSensorStateParam : public IssoParam
{
    protected:
        IssoState m_state;
    public:
        IssoSensorStateParam();
        IssoSensorStateParam(IssoParamId id, IssoState state);
        IssoSensorStateParam(const IssoSensorStateParam& other);
        ~IssoSensorStateParam();

        virtual bool loadFromStream(QDataStream *stream);
        virtual void saveToStream(QDataStream *stream);
        virtual int size();
        virtual QString description();

        IssoState state() const;
};



//========== IssoSensorExtStateParam ==========

class IssoSensorExtStateParam : public IssoSensorStateParam
{
    protected:
        float m_floatValue;

    public:
        IssoSensorExtStateParam();
        IssoSensorExtStateParam(IssoParamId id, IssoState state, float value);
        ~IssoSensorExtStateParam();

        virtual bool loadFromStream(QDataStream *stream);
        virtual void saveToStream(QDataStream *stream);
        virtual int size();
        virtual QString description();

        float value() const;
};



//========== IssoIpParam ==========

class IssoIpParam : public IssoParam
{
    protected:
        quint32 m_ip;

    public:
        IssoIpParam();
        IssoIpParam(IssoParamId id, quint32 ip);
        ~IssoIpParam();

        virtual bool loadFromStream(QDataStream *stream);
        virtual void saveToStream(QDataStream *stream);
        virtual int size();
        virtual QString description();

        quint32 ip() const;
};



//========== IssoByteParam ==========

class IssoByteParam : public IssoParam
{
    protected:
        quint8 m_byteValue;

    public:
        IssoByteParam();
        IssoByteParam(IssoParamId id, quint8 byteValue);
        ~IssoByteParam();

        virtual bool loadFromStream(QDataStream *stream);
        virtual void saveToStream(QDataStream *stream);
        virtual int size();
        virtual QString description();

        quint8 byteValue() const;
};


//========== IssoSingleRegAddrParam ==========

class IssoSingleRegAddrParam : public IssoByteParam
{
    protected:
        quint8 m_regAddress;

    public:
        IssoSingleRegAddrParam();
        IssoSingleRegAddrParam(IssoParamId id, quint8 multiSensorId, quint8 regAddress);
        ~IssoSingleRegAddrParam();

        virtual bool loadFromStream(QDataStream *stream);
        virtual void saveToStream(QDataStream *stream);
        virtual int size();
        virtual QString description();

        quint8 multiSensorId() const;
        quint8 regAddress() const;
};


//========== IssoSingleRegInfoParam ==========

class IssoSingleRegInfoParam : public IssoSingleRegAddrParam
{
    protected:
        quint16 m_regValue;

    public:
        IssoSingleRegInfoParam();
        IssoSingleRegInfoParam(IssoParamId id, quint8 multiSensorId,
                               quint8 regAddress, quint16 regValue);
        ~IssoSingleRegInfoParam();

        virtual bool loadFromStream(QDataStream *stream);
        virtual void saveToStream(QDataStream *stream);
        virtual int size();
        virtual QString description();

        quint8 regValue() const;
};


//========== IssoBlockRegAddrParam ==========

class IssoBlockRegAddrParam : public IssoSingleRegAddrParam
{
    protected:
        quint8 m_regCount;

    public:
        IssoBlockRegAddrParam();
        IssoBlockRegAddrParam(IssoParamId id, quint8 multiSensorId,
                              quint8 regAddress, quint8 regCount);
        ~IssoBlockRegAddrParam();

        virtual bool loadFromStream(QDataStream *stream);
        virtual void saveToStream(QDataStream *stream);
        virtual int size();
        virtual QString description();

        quint8 regCount() const;
};


//========== IssoBlockRegInfoParam ==========

class IssoBlockRegInfoParam : public IssoBlockRegAddrParam
{
    protected:
        QByteArray m_regArray;

    public:
        IssoBlockRegInfoParam();
        IssoBlockRegInfoParam(IssoParamId id, quint8 multiSensorId,
                              quint8 regAddress, const QByteArray& regArray);
        ~IssoBlockRegInfoParam();

        virtual bool loadFromStream(QDataStream *stream);
        virtual void saveToStream(QDataStream *stream);
        virtual int size();
        virtual QString description();

        QByteArray regArray() const;
};


//========== IssoShortParam ==========

class IssoShortParam : public IssoParam
{
    protected:
        quint16 m_shortValue;

    public:
        IssoShortParam();
        IssoShortParam(IssoParamId id, quint16 shortValue);
        ~IssoShortParam();

        virtual bool loadFromStream(QDataStream *stream);
        virtual void saveToStream(QDataStream *stream);
        virtual int size();
        virtual QString description();

        quint16 shortValue() const;
};



//========== IssoArrayParam ==========

class IssoArrayParam : public IssoParam
{
    protected:
        QByteArray m_array;

    public:
        IssoArrayParam(int size);
        IssoArrayParam(IssoParamId id, const QByteArray& array);
        ~IssoArrayParam();

        virtual bool loadFromStream(QDataStream *stream);
        virtual void saveToStream(QDataStream *stream);
        virtual int size();
        virtual QString description();

        QByteArray array() const;
};



//========== IssoSizedArrayParam ==========

class IssoSizedArrayParam : public IssoArrayParam
{
    public:
        IssoSizedArrayParam();
        IssoSizedArrayParam(IssoParamId id, const QByteArray& array);
        ~IssoSizedArrayParam();

        virtual bool loadFromStream(QDataStream *stream);
        virtual void saveToStream(QDataStream *stream);
        virtual int size();
        virtual QString description();
};



//========== IssoBoolParam ==========

class IssoBoolParam : public IssoByteParam
{
    public:
        IssoBoolParam();
        IssoBoolParam(IssoParamId id, bool value);
        ~IssoBoolParam();
        virtual QString description();

        bool boolValue();
};




//========== IssoMultiSensorData ==========

struct IssoMultiSensorData
{
    private:
        quint8 m_multiSensorId; //! multiSensor Id
        quint8 m_stateValue;    //! параметры состояния
        quint16 m_detailsValue;

        QMap<IssoDigitalSensorId, IssoState> m_detailsMap;

    public:
        IssoMultiSensorData();
        IssoMultiSensorData(quint8 multiSensorId, quint8 stateValue, quint16 detailsValue);

        quint8 multiSensorId() const;
        void setMultiSensorId(const quint8 &multiSensorId);
        quint8 stateValue() const;
        void setStateValue(const quint8 &stateValue);
        quint16 detailsValue() const;
        void setDetailsValue(const quint16 &detailsValue);

        bool valid() const;
        void setValid(bool valid);

        IssoState state();

        // общее состояние - тревожное?
        bool alarmed();
        // число тревожных внутренних датчиков
        int alarmChannels();

        QMap<IssoDigitalSensorId, IssoState> getDigitalSensorStates();
        static QMap<IssoDigitalSensorId, IssoState> makeDigitalSensorStates(quint16 detailsValue);
        static quint16 makeDetailsValue(const QMap<IssoDigitalSensorId, IssoState>& digitalStates);


        static IssoState valueToState(quint8 value);
        static quint8 stateToValue(IssoState state);

        // получить из значения детализации состояние встроенного датчика согласно смещению
        static IssoState detailsToState(quint16 detailsValue, quint8 shiftBits);
        // получить значение детализации с выставленным состоянием согласно смещению
        static quint16 stateToDetails(quint16 detailsValue, quint8 shiftBits,
                                      IssoState stateValue);

        int size();

        friend QDataStream &operator>>(QDataStream &in, IssoMultiSensorData& data);
        friend QDataStream &operator<<(QDataStream &out, const IssoMultiSensorData& data);
};




//========== IssoMultiSensorStateParam ==========

class IssoMultiSensorStateParam : public IssoSensorStateParam
{
    private:
        // пары [адрес мультидатчика : данные мультидатчика]
        QMap<quint8, IssoMultiSensorData> m_dataMap;

        IssoState calcResultState();

    public:
        IssoMultiSensorStateParam();
        IssoMultiSensorStateParam(IssoParamId id,
                                  QMap<quint8, IssoMultiSensorData> dataMap);
        IssoMultiSensorStateParam(const IssoMultiSensorStateParam& other);
        ~IssoMultiSensorStateParam();

        QMap<quint8, IssoMultiSensorData> dataMap() const;
        IssoMultiSensorData multiSensorData(quint8 addr);

        virtual bool loadFromStream(QDataStream *stream);
        virtual void saveToStream(QDataStream *stream);
        virtual int size();
        virtual QString description();

        QMap<quint8, IssoMultiSensorData> diffDataMap(const QMap<quint8, IssoMultiSensorData>& otherDataMap);
};



//========== IssoMultiSensorRawDataParam ==========

class IssoMultiSensorRawDataParam : public IssoSensorStateParam
{
    protected:
        IssoMultiSensorData m_data;
        QMap<IssoDigitalSensorId, quint16> m_rawValues;
//        QMap<IssoDigitalSensorId, float> m_rawValues;

    public:
        // 16 регистров (по 2 байта)
        const int RAW_DATA_SIZE = 32;

        IssoMultiSensorRawDataParam();
        IssoMultiSensorRawDataParam(IssoParamId id);
        IssoMultiSensorRawDataParam(IssoParamId id,
                                    const IssoMultiSensorData& data,
                                    const QMap<IssoDigitalSensorId, quint16>& rawValues);
        ~IssoMultiSensorRawDataParam();

        virtual bool loadFromStream(QDataStream *stream);
        virtual void saveToStream(QDataStream *stream);
        virtual int size();
        virtual QString description();

        IssoMultiSensorData data() const;
        QMap<IssoDigitalSensorId, quint16> rawValues() const;
};


//========== IssoMultiSensorExistDataParam ==========

class IssoMultiSensorExistDataParam : public IssoParam
{
    protected:
        quint8 m_address;
        quint8 m_deviceType;
        quint16 m_firstSN;
        quint16 m_secondSN;
        quint16 m_majorVersion;
        quint16 m_minorVersion;

    public:
        IssoMultiSensorExistDataParam();
        IssoMultiSensorExistDataParam(IssoParamId id,
                                      quint8 address,  quint8 deviceType,
                                      quint16 firstSN, quint16 secondSN,
                                      quint16 majorVersion, quint16 minorVersion);
        ~IssoMultiSensorExistDataParam();

        virtual bool loadFromStream(QDataStream *stream);
        virtual void saveToStream(QDataStream *stream);
        virtual int size();
        virtual QString description();

        quint8 address() const;
        quint8 deviceType() const;
        quint16 firstSN() const;
        quint16 secondSN() const;
        quint16 majorVersion() const;
        quint16 minorVersion() const;
};


#endif // ISSOPARAM_H
