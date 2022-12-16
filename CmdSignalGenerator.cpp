/**
CMD_MS_VALID_SLAVES_GET
MultiSensor Valid Slaves Get
**/
//* получаем список модулей
//* создаем запросы
//* ставим запросы в очередь
//* выпускаем signals Multi Sensor Valid Slaves Get
/*
"49535352 000C 0006 4D565347 3C 0C ABCD"
49535352 преамбула
000C номер пакета
0006 длина команды
4D565347  команда MVSG 
3C        ID параметров
0C        Данные команды
ABCD      CRC16
*/
typedef struct {
	uint8 cmdID;
	QString cmdValue;
	uint8 cmdLen;
	uint8 respLen;
} cmdInfo;

typedef struct {
	uchar8   cHeader[4];
	uchar16	 iPackOrder;
	uchar16	 iCmdLen;
	uchar8	 cCmd[4];
	uchar8   *uData;
	uchar16	 CRC16; 
} cmdPacket;

typedef { bool (*cmdHandler)(uint8 moduleID, IssoCmdId id, QByteArray ba ) } IssoCmdHandler;

// обращение к одному модулю для одного мультидатчика
 IssoModuleRequest existNonactivatedRequest =
                IssoModuleRequest(ip, port, nextPackNum(), CMD_MULTISENSOR_EXIST_GET, params);
// даем команду CMD_MULTISENSOR_EXIST_GET для всех присоединенных к moduleId ms
    requestStateAsync(moduleId, CMD_MULTISENSOR_EXIST_GET)
    {
    	// сформировать запрос
    IssoModuleRequest request(ip, port, nextPackNum(), cmdId);
      IssoModuleRequest(QHostAddress addr, quint16 port,
                          quint16 number, IssoCmdId cmdId)
      {
      	IssoModulePacket(addr, port, number, IssoModuleCommand(cmdId))
      	IssoModuleCommand::IssoModuleCommand(IssoCmdId cmdId)
    : m_id(cmdId)
{
}
{
    m_crc = calcCrc16();
}
      }
    // асинхронно добавить запрос в очередь
    }

//* создаем обработчик слота для Multi Sensor Valid Slaves Get



/** 
Multi Sensor Valid Slave Verify
**/