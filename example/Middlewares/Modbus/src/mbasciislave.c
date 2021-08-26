/******************************************************************************/
/** 模块名称：Modbus通讯                                                     **/
/** 文件名称：mbasciislave.c                                                 **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：用于声明Modbus ASCII从站相关属性及方法                         **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2018-09-11          木南              创建文件               **/
/**                                                                          **/
/******************************************************************************/

#include "mbasciislave.h"

/*处理读线圈状态命令*/
static uint16_t HandleReadCoilStatusCommand(uint16_t startAddress, uint16_t quantity, uint8_t *receivedMessage, uint8_t *respondBytes);
/*处理读输入状态命令*/
static uint16_t HandleReadInputStatusCommand(uint16_t startAddress, uint16_t quantity, uint8_t *receivedMessage, uint8_t *respondBytes);
/*处理读保持寄存器命令*/
static uint16_t HandleReadHoldingRegisterCommand(uint16_t startAddress, uint16_t quantity, uint8_t *receivedMessage, uint8_t *respondBytes);
/*处理读输入寄存器命令*/
static uint16_t HandleReadInputRegisterCommand(uint16_t startAddress, uint16_t quantity, uint8_t *receivedMessage, uint8_t *respondBytes);
/*处理写单个线圈命令*/
static uint16_t HandleWriteSingleCoilCommand(uint16_t coilAddress, uint16_t coilValue, uint8_t *receivedMessage, uint8_t *respondBytes);
/*处理写单个寄存器命令*/
static uint16_t HandleWriteSingleRegisterCommand(uint16_t registerAddress, uint16_t registerValue, uint8_t *receivedMessage, uint8_t *respondBytes);
/*处理写多个线圈状态*/
static uint16_t HandleWriteMultipleCoilCommand(uint16_t startAddress, uint16_t quantity, uint8_t *receivedMessage, uint8_t *respondBytes);
/*处理写多个寄存器状态*/
static uint16_t HandleWriteMultipleRegisterCommand(uint16_t startAddress, uint16_t quantity, uint8_t *receivedMessage, uint8_t *respondBytes);

uint16_t (*HandleAsciiMasterCommand[])(uint16_t, uint16_t, uint8_t *, uint8_t *) = {HandleReadCoilStatusCommand,
HandleReadInputStatusCommand,
HandleReadHoldingRegisterCommand,
HandleReadInputRegisterCommand,
HandleWriteSingleCoilCommand,
HandleWriteSingleRegisterCommand,
HandleWriteMultipleCoilCommand,
HandleWriteMultipleRegisterCommand};

/*解析接收到的信息，并返回合成的回复信息和信息的字节长度，通过回调函数*/
uint16_t ParsingAsciiMasterAccessCommand(uint8_t *receivedMessage, uint8_t *respondBytes, uint16_t rxLength, uint8_t StationAddress)
{
  uint16_t respondLength = 0;
  
  /*判断是否为Modbus ASCII消息*/
  if (0x3A != receivedMessage[0])
  {
    return 0;
  }
  
  /*判断消息是否接收完整*/
  if ((rxLength < 17) || (receivedMessage[rxLength - 2] != 0x0D) || (receivedMessage[rxLength - 1] != 0x0A))
  {
    return 65535;
  }
  
  uint16_t length = rxLength - 3;
  uint8_t hexMessage[256];
  
  if (!CovertAsciiMessageToHex(receivedMessage + 1, hexMessage, length))
  {
    return 0;
  }
  
  /*校验接收到的数据是否正确*/
  if (!CheckASCIIMessageIntegrity(hexMessage, length/2))
  {
    return 0;
  }
  
  /*判断是否是本站，如不是不处理*/
  uint8_t slaveAddress = *hexMessage;
  if (slaveAddress != StationAddress)
  {
    return 0;
  }
  
  /*判断功能码是否有误*/
  FunctionCode fc = (FunctionCode)(*(hexMessage + 1));
  if (CheckFunctionCode(fc) != Modbus_OK)
  {
    return 0;
  }
  
  uint16_t startAddress = (uint16_t)(*(hexMessage + 2));
  startAddress = (startAddress << 8) + (uint16_t)(*(hexMessage + 3));
  uint16_t quantity = (uint16_t)(*(hexMessage + 4));
  quantity = (quantity << 8) + (uint16_t)(*(hexMessage + 5));
  
  uint8_t index = (fc > 0x08) ? (fc - 0x09) : (fc - 0x01);
  
  respondLength = HandleAsciiMasterCommand[index](startAddress, quantity, hexMessage, respondBytes);
  
  return respondLength;
}

/*处理读线圈状态命令*/
static uint16_t HandleReadCoilStatusCommand(uint16_t startAddress, uint16_t quantity, uint8_t *receivedMessage, uint8_t *respondBytes)
{
  uint16_t length = 0;
  bool statusList[250];
  
  GetCoilStatus(startAddress, quantity, statusList);
  
  length = SyntheticAsciiSlaveAccessRespond(receivedMessage, statusList, NULL, respondBytes);
  
  return length;
}

/*处理读输入状态命令*/
static uint16_t HandleReadInputStatusCommand(uint16_t startAddress, uint16_t quantity, uint8_t *receivedMessage, uint8_t *respondBytes)
{
  uint16_t length = 0;
  
  bool statusList[250];
  
  GetInputStatus(startAddress, quantity, statusList);
  
  length = SyntheticAsciiSlaveAccessRespond(receivedMessage, statusList, NULL, respondBytes);
  
  return length;
}

/*处理读保持寄存器命令*/
static uint16_t HandleReadHoldingRegisterCommand(uint16_t startAddress, uint16_t quantity, uint8_t *receivedMessage, uint8_t *respondBytes)
{
  uint16_t length = 0;
  
  uint16_t registerList[125];
  
  GetHoldingRegister(startAddress, quantity, registerList);
  
  length = SyntheticAsciiSlaveAccessRespond(receivedMessage, NULL, registerList, respondBytes);
  
  return length;
}

/*处理读输入寄存器命令*/
static uint16_t HandleReadInputRegisterCommand(uint16_t startAddress, uint16_t quantity, uint8_t *receivedMessage, uint8_t *respondBytes)
{
  uint16_t length = 0;
  
  uint16_t registerList[125];
  
  GetInputRegister(startAddress, quantity, registerList);
  
  length = SyntheticAsciiSlaveAccessRespond(receivedMessage, NULL, registerList, respondBytes);
  
  return length;
}

/*处理写单个线圈命令*/
static uint16_t HandleWriteSingleCoilCommand(uint16_t coilAddress, uint16_t coilValue, uint8_t *receivedMessage, uint8_t *respondBytes)
{
  uint16_t length = 0;
  bool value;
  
  length = SyntheticAsciiSlaveAccessRespond(receivedMessage, NULL, NULL, respondBytes);
  
  GetCoilStatus(coilAddress, 1, &value);
  
  value = CovertSingleCommandCoilToBoolStatus(coilValue, value);
  SetSingleCoil(coilAddress, value);
  
  return length;
}

/*处理写单个寄存器命令*/
static uint16_t HandleWriteSingleRegisterCommand(uint16_t registerAddress, uint16_t registerValue, uint8_t *receivedMessage, uint8_t *respondBytes)
{
  uint16_t length = 0;
  
  length = SyntheticAsciiSlaveAccessRespond(receivedMessage, NULL, NULL, respondBytes);
  
  SetSingleRegister(registerAddress, registerValue);
  
  return length;
}

/*处理写多个线圈状态*/
static uint16_t HandleWriteMultipleCoilCommand(uint16_t startAddress, uint16_t quantity, uint8_t *receivedMessage, uint8_t *respondBytes)
{
  uint16_t length = 0;
  bool statusValue[250];
  
  length = SyntheticAsciiSlaveAccessRespond(receivedMessage, statusValue, NULL, respondBytes);
  
  SetMultipleCoil(startAddress, quantity, statusValue);
  
  return length;
}

/*处理写多个寄存器状态*/
static uint16_t HandleWriteMultipleRegisterCommand(uint16_t startAddress, uint16_t quantity, uint8_t *receivedMessage, uint8_t *respondBytes)
{
  uint16_t length = 0;
  uint16_t registerValue[125];
  
  length = SyntheticAsciiSlaveAccessRespond(receivedMessage, NULL, registerValue, respondBytes);
  
  SetMultipleRegister(startAddress, quantity, registerValue);
  return length;
}

/*********** (C) COPYRIGHT 1999-2018 Moonan Technology *********END OF FILE****/