/******************************************************************************/
/** 模块名称：Modbus通讯                                                     **/
/** 文件名称：mbtcpclient.c                                                  **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：用于声明Modbus TCP客户端的相关属性及方法                       **/
/**           1、Modbus TCP客户端用户应用的接口层                            **/
/**           2、实现服务器访问命令的生成并将其传回应用层                    **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2016-04-17          尹家军            创建文件               **/
/**                                                                          **/
/******************************************************************************/ 

#include "mbtcpclient.h"

#define StoreCommandNumber 10

uint8_t commandClientList[StoreCommandNumber][12];    /*定义已发送的读命令列表*/
uint16_t commandClientIndex=0;        /*定义命令列表下一个命令位置*/
uint16_t transactionIDCounter=0;/*事务标识符*/

/*发送命令后，向已发送命令列表中添加命令*/
static void AddCommandBytesToList(uint8_t *commandBytes);
/*接收到返回信息后，判断是否是发送命令列表中命令的返回信息，*/
static int FindCommandForRecievedMessage(uint8_t *recievedMessage);
/*处理读从站状态量返回信息，读线圈状态位0x012功能码*/
static void HandleReadCoilStatusRespond(uint8_t *receivedMesasage,uint16_t startAddress,uint16_t quantity);
/*处理读从站状态量返回信息，读输入状态位0x02功能码*/
static void HandleReadInputStatusRespond(uint8_t *receivedMesasage,uint16_t startAddress,uint16_t quantity);
/*处理读从站寄存器值的返回信息，读保持寄存器0x03功能码）*/
static void HandleReadHoldingRegisterRespond(uint8_t *receivedMesasage,uint16_t startAddress,uint16_t quantity);
/*处理读从站寄存器值的返回信息，读输入寄存器0x04功能码*/
static void HandleReadInputRegisterRespond(uint8_t *receivedMesasage,uint16_t startAddress,uint16_t quantity);

void (*HandleServerRespond[])(uint8_t *,uint16_t,uint16_t)={HandleReadCoilStatusRespond,
                                                                HandleReadInputStatusRespond,
                                                                HandleReadHoldingRegisterRespond,
                                                                HandleReadInputRegisterRespond};

/*生成访问服务器的命令*/
uint16_t CreateAccessServerCommand(ObjAccessInfo objInfo,void *dataList,uint8_t *commandBytes)
{
  uint16_t commandLength=0;
  /*生成读服务器对象的命令，功能码0x01、0x02、0x03、0x04,命令长度12个字节*/
  if((objInfo.functionCode>=ReadCoilStatus)&&(objInfo.functionCode <= ReadInputRegister))
  {
    commandLength=SyntheticReadWriteTCPServerCommand(objInfo,NULL,NULL,commandBytes);
    
    AddCommandBytesToList(commandBytes);        /*记录发送的读命令*/
  }

  /*生成预置服务器对象的命令，功能码0x05,0x0F,命令长度随发送数据而变*/
  if((objInfo.functionCode==WriteSingleCoil)&&(objInfo.functionCode==WriteMultipleCoil))
  {
    bool *statusList=(bool*)dataList;
    commandLength=SyntheticReadWriteTCPServerCommand(objInfo,statusList,NULL,commandBytes);
  }
  
  /*生成预置服务器对象的命令，功能码0x06,0x10,命令长度随发送数据而变*/
  if((objInfo.functionCode==WriteSingleRegister)&&(objInfo.functionCode==WriteMultipleRegister))
  {
    uint16_t *registerList=(uint16_t*)dataList;
    commandLength=SyntheticReadWriteTCPServerCommand(objInfo,NULL,registerList,commandBytes);
  }

  return commandLength;
}

/*解析收到的服务器相应信息*/
void ParsingServerRespondMessage(uint8_t *recievedMessage)
{
  /*判断接收到的信息是否有相应的命令*/
  int cmdIndex=FindCommandForRecievedMessage(recievedMessage);
  
  if((cmdIndex<0))
  {
    return;
  }
  
  if(recievedMessage[7]>0x04)
  {
    return;
  }
  FunctionCode fuctionCode=(FunctionCode)recievedMessage[7];
  
  if(fuctionCode!=commandClientList[cmdIndex][7])
  {
    return;
  }
  
  uint16_t startAddress=(uint16_t)commandClientList[cmdIndex][8];
  startAddress=(startAddress<<8)+(uint16_t)commandClientList[cmdIndex][9];
  uint16_t quantity=(uint16_t)commandClientList[cmdIndex][10];
  quantity=(quantity<<8)+(uint16_t)commandClientList[cmdIndex][11];
  
  if((fuctionCode>=ReadCoilStatus)&&(fuctionCode<=ReadInputRegister))
  {
    HandleServerRespond[fuctionCode-1](recievedMessage,startAddress,quantity);
  }
}

/*发送命令后，向已发送命令列表中添加命令*/
static void AddCommandBytesToList(uint8_t *commandBytes)
{
  if(commandClientIndex>StoreCommandNumber)
  {
    commandClientIndex=0;
  }
  
  for(int i=0;i<12;i++)
  {
    commandClientList[commandClientIndex][i]=commandBytes[i];
  }

  commandClientIndex++;
}

/*接收到返回信息后，判断是否是发送命令列表中命令的返回信息，*/
/*若是则从已发送命令列表中删除命令，若不是则丢弃该条返回信息*/
static int FindCommandForRecievedMessage(uint8_t *recievedMessage)
{
  int cmdIndex=-1;
  for(int i=0;i<StoreCommandNumber;i++)
  {
    if((recievedMessage[0]==commandClientList[i][0])&&(recievedMessage[1]==commandClientList[i][1]))
    {
      cmdIndex=i;
      break;
    }
  }
  
  return cmdIndex;
}

/*处理读从站状态量返回信息，读线圈状态位0x01功能码*/
static void HandleReadCoilStatusRespond(uint8_t *receivedMessage,uint16_t startAddress,uint16_t quantity)
{
  bool coilStatus[256];
  
  TransformClientReceivedData(receivedMessage+6,quantity,coilStatus,NULL);
  
  UpdateCoilStatus(startAddress,quantity,coilStatus);
}

/*处理读从站状态量返回信息，读输入状态位0x02功能码*/
static void HandleReadInputStatusRespond(uint8_t *receivedMessage,uint16_t startAddress,uint16_t quantity)
{
  bool inputStatus[256];
  
  TransformClientReceivedData(receivedMessage+6,quantity,inputStatus,NULL);
  
  UpdateInputStatus(startAddress,quantity,inputStatus);
}

/*处理读从站寄存器值的返回信息，读保持寄存器0x03功能码）*/
static void HandleReadHoldingRegisterRespond(uint8_t *receivedMessage,uint16_t startAddress,uint16_t quantity)
{
  uint16_t holdingRegister[125];
  
  TransformClientReceivedData(receivedMessage+6,quantity,NULL,holdingRegister);
  
  UpdateHoldingRegister(startAddress,quantity,holdingRegister);
}

/*处理读从站寄存器值的返回信息，读输入寄存器0x04功能码*/
static void HandleReadInputRegisterRespond(uint8_t *receivedMessage,uint16_t startAddress,uint16_t quantity)
{
  uint16_t inputRegister[125];
  
  TransformClientReceivedData(receivedMessage+6,quantity,NULL,inputRegister);
  
  UpdateInputResgister(startAddress,quantity,inputRegister);
}

/*********** (C) COPYRIGHT 1999-2016 Moonan Technology *********END OF FILE****/