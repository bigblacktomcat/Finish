#include "issomodulepacket.h"
#include <QDebug>


IssoModulePacket::IssoModulePacket()
    : m_address(QHostAddress::Null),
      m_port(0),
      m_number(0)
{
}


IssoModulePacket::IssoModulePacket(QHostAddress addr, quint16 port, quint16 number, IssoModuleCommand command)
    : m_address(addr),
      m_port(port),
      m_number(number),
      m_command(command)
{
}


IssoModulePacket::IssoModulePacket(const IssoModulePacket &other)
    : m_address(other.m_address),
      m_port(other.m_port),
      m_number(other.m_number),
      m_command(other.m_command)
{
//    this->m_address = other.address();
//    this->m_port = other.port();
//    this->m_number = other.number();
}


IssoModulePacket::~IssoModulePacket()
{
}


const QHostAddress IssoModulePacket::address() const
{
    return m_address;
}


quint16 IssoModulePacket::port() const
{
    return m_port;
}


IssoModuleCommand IssoModulePacket::command() const
{
    return m_command;
}


quint16 IssoModulePacket::crc() const
{
    return m_crc;
}


quint16 IssoModulePacket::number() const
{
    return m_number;
}

