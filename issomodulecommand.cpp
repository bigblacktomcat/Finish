#include "issomodulecommand.h"


IssoModuleCommand::IssoModuleCommand()
    : IssoModuleCommand(CMD_UNDEFINED)
{
}


IssoModuleCommand::IssoModuleCommand(IssoCmdId cmdId)
    : m_id(cmdId)
{
}


IssoModuleCommand::IssoModuleCommand(IssoCmdId cmdId, IssoParamBuffer params)
    : IssoModuleCommand(cmdId)
{
    m_paramBuffer = params;
}


IssoModuleCommand::IssoModuleCommand(const QByteArray &data)
{
    // строковый id команды
    QString replyIdStr = QString(data.left(4));
    m_id = IssoCommonData::cmdIdByString(replyIdStr);
    // разобрать параметры ответа
    // формируем карту параметров из массива данных
    m_paramBuffer = IssoParamBuffer(data.mid(4));
}


IssoModuleCommand::IssoModuleCommand(const IssoModuleCommand &other)
    : m_id(other.m_id),
      m_paramBuffer(other.m_paramBuffer)
{
}


IssoCmdId IssoModuleCommand::id() const
{
    return m_id;
}


IssoParamBuffer IssoModuleCommand::paramBuffer() const
{
    return m_paramBuffer;
}


const QByteArray IssoModuleCommand::bytes() const
{
    QByteArray ba;
    QDataStream stream(&ba, QIODevice::WriteOnly);
    //
    // записать в поток id команды
    QString cmdIdStr = IssoCommonData::stringByCmdId(m_id);
    QByteArray cmdIdData = cmdIdStr.toUtf8();
    stream.writeRawData(cmdIdData.data(), cmdIdData.length());
    // записать в поток набор параметров
    QByteArray paramData = m_paramBuffer.bytes();
    stream.writeRawData(paramData.data(), paramData.length());
    // вернуть результирующий массив
    return ba;
}


int IssoModuleCommand::paramCount() const
{
    return m_paramBuffer.paramCount();
}


IssoParam *IssoModuleCommand::paramById(const IssoParamId &paramId)
{
    return m_paramBuffer.paramById(paramId);
}


QSharedPointer<IssoParam> IssoModuleCommand::paramPtrByIndex(int index)
{
    return m_paramBuffer.paramPtrByIndex(index);
}


bool IssoModuleCommand::paramExists(const IssoParamId &paramId) const
{
    return m_paramBuffer.paramExists(paramId);
}

