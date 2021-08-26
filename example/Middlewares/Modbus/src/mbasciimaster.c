/******************************************************************************/
/** 模块名称：Modbus通讯                                                     **/
/** 文件名称：mbasciimaster.c                                                **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：用于声明Modbus ASCII主站相关属性及方法                         **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2018-09-11          木南              创建文件               **/
/**                                                                          **/
/******************************************************************************/

#include "mbasciimaster.h"

/*处理读从站状态量返回信息，读线圈状态位0x01功能码*/
static void HandleReadCoilStatusRespond(AsciiLocalMasterType *master,uint8_t *receivedMesasage, uint16_t startAddress, uint16_t quantity);
/*处理读从站状态量返回信息，读输入状态位0x02功能码*/
static void HandleReadInputStatusRespond(AsciiLocalMasterType *master,uint8_t *receivedMesasage, uint16_t startAddress, uint16_t quantity);
/*处理读从站寄存器值的返回信息，读保持寄存器0x03功能码）*/
static void HandleReadHoldingRegisterRespond(AsciiLocalMasterType *master,uint8_t *receivedMesasage, uint16_t startAddress, uint16_t quantity);
/*处理读从站寄存器值的返回信息，读输入寄存器0x04功能码*/
static void HandleReadInputRegisterRespond(AsciiLocalMasterType *master,uint8_t *receivedMesasage, uint16_t startAddress, uint16_t quantity);
/*判断接收到的信息是否是发送命令的返回信息*/
static bool CheckMessageAgreeWithCommand(uint8_t *recievedMessage, uint8_t *command);

void (*HandleAsciiSlaveRespond[])(AsciiLocalMasterType *master,uint8_t *, uint16_t, uint16_t) = {HandleReadCoilStatusRespond,
HandleReadInputStatusRespond,
HandleReadHoldingRegisterRespond,
HandleReadInputRegisterRespond};

/*函数名：CreateAccessSlaveCommand，生成访问服务器的命令*/
/*参数：ObjAccessInfo objInfo,要生成访问命令的对象信息*/
/*      void *dataList,写的数据列表，寄存器为uint16_t类型，状态量为bool类型*/
/*      uint8_t *commandBytes,生成的命令列表*/
/*返回值：uint16_t，生成的命令的长度*/
uint16_t CreateAccessAsciiSlaveCommand(ObjAccessInfo objInfo, void *dataList, uint8_t *commandBytes)
{
  uint16_t commandLength = 0;
  /*生成读服务器对象的命令，功能码0x01、0x02、0x03、0x04,命令长度17个字节*/
  if ((objInfo.functionCode >= ReadCoilStatus) && (objInfo.functionCode <= ReadInputRegister))
  {
    commandLength = SyntheticReadWriteAsciiSlaveCommand(objInfo, NULL, NULL, commandBytes);
  }
  
  /*生成预置服务器对象的命令，功能码0x05,0x0F,命令长度随发送数据而变*/
  if ((objInfo.functionCode == WriteSingleCoil) || (objInfo.functionCode == WriteMultipleCoil))
  {
    bool *statusList = (bool *)dataList;
    commandLength = SyntheticReadWriteAsciiSlaveCommand(objInfo, statusList, NULL, commandBytes);
  }
  
  /*生成预置服务器对象的命令，功能码0x06,0x10,命令长度随发送数据而变*/
  if ((objInfo.functionCode == WriteSingleRegister) || (objInfo.functionCode == WriteMultipleRegister))
  {
    uint16_t *registerList = (uint16_t *)dataList;
    commandLength = SyntheticReadWriteAsciiSlaveCommand(objInfo, NULL, registerList, commandBytes);
  }
  
  return commandLength;
}

/*解析收到的服务器相应信息*/
/*uint8_t *recievedMessage,接收到的消息列表*/
/*uint8_t *command,发送的读操作命令，若为NULL则在命令列表中查找*/
void ParsingAsciiSlaveRespondMessage(AsciiLocalMasterType *master,uint8_t *recievedMessage, uint8_t *command,uint16_t rxLength)
{
  int i=0;
  int j=0;
  uint8_t *cmd=NULL;
  
  /*判断是否为Modbus ASCII消息*/
  if (0x3A != recievedMessage[0])
  {
    return ;
  }
  
  /*判断消息是否接收完整*/
  if ((rxLength < 13) || (recievedMessage[rxLength - 2] != 0x0D) || (recievedMessage[rxLength - 1] != 0x0A))
  {
    return ;
  }
  
  uint16_t length = rxLength - 3;
  uint8_t hexMessage[256];
  
  if (!CovertAsciiMessageToHex(recievedMessage + 1, hexMessage, length))
  {
    return ;
  }
  
  /*校验接收到的数据是否正确*/
  if (!CheckASCIIMessageIntegrity(hexMessage, length/2))
  {
    return ;
  }
  
  /*判断功能码是否有误*/
  FunctionCode fuctionCode = (FunctionCode)hexMessage[1];
  if (CheckFunctionCode(fuctionCode) != Modbus_OK)
  {
    return;
  }
  
  if ((command == NULL)||(!CheckMessageAgreeWithCommand(recievedMessage, command)))
  {
    while(i<master->slaveNumber)
    {
      if(master->pSlave[i].stationAddress==hexMessage[0])
      {
        break;
      }
      i++;
    }
    
    if(i>=master->slaveNumber)
    {
      return;
    }
    
    if((master->pSlave[i].pLastCommand==NULL)||(!CheckMessageAgreeWithCommand(recievedMessage,master->pSlave[i].pLastCommand)))
    {
      j=FindAsciiCommandForRecievedMessage(recievedMessage,master->pSlave[i].pReadCommand,master->pSlave[i].commandNumber);
      
      if(j<0)
      {
        return;
      }
      
      cmd=master->pSlave[i].pReadCommand[j];
    }
    else
    {
      cmd=master->pSlave[i].pLastCommand;
    }
  }
  else
  {
    cmd=command;
  }
  
  uint8_t hexCommand[256];
  CovertAsciiMessageToHex(cmd + 1, hexCommand, 14);
  
  uint16_t startAddress = (uint16_t)hexCommand[2];
  startAddress = (startAddress << 8) + (uint16_t)hexCommand[3];
  uint16_t quantity = (uint16_t)hexCommand[4];
  quantity = (quantity << 8) + (uint16_t)hexCommand[5];
  
  if ((fuctionCode >= ReadCoilStatus) && (fuctionCode <= ReadInputRegister))
  {
    HandleAsciiSlaveRespond[fuctionCode - 1](master,hexMessage,startAddress,quantity);
  }
}

/*接收到返回信息后，判断是否是发送命令列表中命令的返回信息，*/
/*若是则从已发送命令列表中删除命令，若不是则丢弃该条返回信息*/
int FindAsciiCommandForRecievedMessage(uint8_t *recievedMessage, uint8_t (*commandList)[17], uint16_t commandNumber)
{
  int cmdIndex = -1;
  
  for (int i = 0; i < commandNumber; i++)
  {
    if (CheckMessageAgreeWithCommand(recievedMessage, commandList[i]) == true)
    {
      cmdIndex = i;
      break;
    }
  }
  return cmdIndex;
}

/*判断接收到的信息是否是发送命令的返回信息*/
static bool CheckMessageAgreeWithCommand(uint8_t *recievedMessage, uint8_t *command)
{
  bool aw = false;
  
  if ((recievedMessage[1] == command[1]) && (recievedMessage[2] == command[2]) && (recievedMessage[3] == command[3]) && (recievedMessage[4] == command[4]))
  {
    uint8_t hexMessage[256];
    uint8_t hexCommand[256];
    
    CovertAsciiMessageToHex(command + 1, hexCommand, 14);
    CovertAsciiMessageToHex(recievedMessage + 1, hexMessage, 6);
    
    uint16_t quantity = (uint16_t)hexCommand[4];
    quantity = (quantity << 8) + (uint16_t)hexCommand[5];
    uint8_t bytescount = 0;
    if ((hexMessage[1] == ReadCoilStatus) || (hexMessage[1] == ReadInputStatus))
    {
      bytescount = (uint8_t)((quantity - 1) / 8 + 1);
    }
    
    if ((hexMessage[1] == ReadHoldingRegister) || (hexMessage[1] == ReadInputRegister))
    {
      bytescount = quantity * 2;
    }
    
    if (hexMessage[2] == bytescount)
    {
      aw = true;
    }
  }
  
  return aw;
}
/*处理读从站状态量返回信息，读线圈状态位0x01功能码*/
static void HandleReadCoilStatusRespond(AsciiLocalMasterType *master,uint8_t *receivedMessage, uint16_t startAddress, uint16_t quantity)
{
  bool coilStatus[256];
  
  TransformClientReceivedData(receivedMessage, quantity, coilStatus, NULL);
  
  uint8_t slaveAddress=receivedMessage[0];
  
  master->pUpdateCoilStatus(slaveAddress,startAddress, quantity, coilStatus);
}

/*处理读从站状态量返回信息，读输入状态位0x02功能码*/
static void HandleReadInputStatusRespond(AsciiLocalMasterType *master,uint8_t *receivedMessage, uint16_t startAddress, uint16_t quantity)
{
  bool inputStatus[256];
  
  TransformClientReceivedData(receivedMessage, quantity, inputStatus, NULL);
  
  uint8_t slaveAddress=receivedMessage[0];
  
  master->pUpdateInputStatus(slaveAddress,startAddress, quantity, inputStatus);
}

/*处理读从站寄存器值的返回信息，读保持寄存器0x03功能码）*/
static void HandleReadHoldingRegisterRespond(AsciiLocalMasterType *master,uint8_t *receivedMessage, uint16_t startAddress, uint16_t quantity)
{
  uint16_t holdingRegister[125];
  
  TransformClientReceivedData(receivedMessage, quantity, NULL, holdingRegister);
  
  uint8_t slaveAddress=receivedMessage[0];
  
  master->pUpdateHoldingRegister(slaveAddress,startAddress, quantity, holdingRegister);
}

/*处理读从站寄存器值的返回信息，读输入寄存器0x04功能码*/
static void HandleReadInputRegisterRespond(AsciiLocalMasterType *master,uint8_t *receivedMessage, uint16_t startAddress, uint16_t quantity)
{
  uint16_t inputRegister[125];
  
  TransformClientReceivedData(receivedMessage, quantity, NULL, inputRegister);
  
  uint8_t slaveAddress=receivedMessage[0];
  
  master->pUpdateInputResgister(slaveAddress,startAddress, quantity, inputRegister);
}

/*初始化ASCII主站对象*/
void InitializeASCIIMasterObject(AsciiLocalMasterType *master,uint16_t slaveNumber,
                                 AsciiAccessedSlaveType *pSlave,
                                 UpdateCoilStatusType pUpdateCoilStatus,
                                 UpdateInputStatusType pUpdateInputStatus,
                                 UpdateHoldingRegisterType pUpdateHoldingRegister,
                                 UpdateInputResgisterType pUpdateInputResgister
                                 )
{
  master->slaveNumber=slaveNumber>255?255:slaveNumber;
  
  master->readOrder=0;
  
  master->pSlave=pSlave;
  
  for(int i=0;i<8;i++)
  {
    master->flagWriteSlave[i]=0x00000000;
  }
  
  master->pUpdateCoilStatus=pUpdateCoilStatus!=NULL?pUpdateCoilStatus:UpdateCoilStatus;
  
  
  master->pUpdateInputStatus=pUpdateInputStatus!=NULL?pUpdateInputStatus:UpdateInputStatus;
  
  master->pUpdateHoldingRegister=(pUpdateHoldingRegister!=NULL)?pUpdateHoldingRegister:UpdateHoldingRegister;
  
  master->pUpdateInputResgister=(pUpdateInputResgister!=NULL)?pUpdateInputResgister:UpdateInputResgister;
}

/* 使能或者失能写从站操作标志位（修改从站的写使能标志位） */
void ModifyWriteASCIISlaveEnableFlag(AsciiLocalMasterType *master,uint8_t slaveAddress,bool en)
{
  uint8_t row=0;
  uint8_t column=0;
  
  row=slaveAddress/32;
  column=slaveAddress%32;
  
  if(en)
  {
    master->flagWriteSlave[row]|=(0x00000001<<column);
  }
  else
  {
    master->flagWriteSlave[row]&=(~(0x00000001<<column));
  }
}

/* 获得从站的写使能标志位的状态 */
bool GetWriteASCIISlaveEnableFlag(AsciiLocalMasterType *master,uint8_t slaveAddress)
{
  bool status=false;
  uint8_t row=0;
  uint8_t column=0;
  
  row=slaveAddress/32;
  column=slaveAddress%32;
  
  if((master->flagWriteSlave[row]&(0x00000001<<column))==(0x00000001<<column))
  {
    status=true;
  }
  
  return status;
}

/* 判断当前是否有写操作使能 */
bool CheckWriteASCIISlaveNone(AsciiLocalMasterType *master)
{
  bool status=true;
  
  for(int i=0;i<8;i++)
  {
    if(master->flagWriteSlave[i]>0x00000000)
    {
      status=false;
    }
  }
  
  return status;
}

/*********** (C) COPYRIGHT 1999-2018 Moonan Technology *********END OF FILE****/