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

/*发送命令后，向已发送命令列表中添加命令*/
void AddCommandBytesToList(TCPLocalClientType *client,uint8_t *commandBytes);
/*接收到返回信息后，判断是否是发送命令列表中命令的返回信息，*/
static int FindCommandForRecievedMessage(TCPLocalClientType *client,uint8_t *recievedMessage);
/*处理读从站状态量返回信息，读线圈状态位0x01功能码*/
static void HandleReadCoilStatusRespond(TCPLocalClientType *client,uint8_t *receivedMesasage,uint16_t startAddress,uint16_t quantity);
/*处理读从站状态量返回信息，读输入状态位0x02功能码*/
static void HandleReadInputStatusRespond(TCPLocalClientType *client,uint8_t *receivedMesasage,uint16_t startAddress,uint16_t quantity);
/*处理读从站寄存器值的返回信息，读保持寄存器0x03功能码）*/
static void HandleReadHoldingRegisterRespond(TCPLocalClientType *client,uint8_t *receivedMesasage,uint16_t startAddress,uint16_t quantity);
/*处理读从站寄存器值的返回信息，读输入寄存器0x04功能码*/
static void HandleReadInputRegisterRespond(TCPLocalClientType *client,uint8_t *receivedMesasage,uint16_t startAddress,uint16_t quantity);

void (*HandleServerRespond[])(TCPLocalClientType *client,uint8_t *,uint16_t,uint16_t)={HandleReadCoilStatusRespond,
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
void ParsingServerRespondMessage(TCPLocalClientType *client,uint8_t *recievedMessage)
{
  /*判断接收到的信息是否有相应的命令*/
  int cmdIndex=FindCommandForRecievedMessage(client,recievedMessage);
  
  if((cmdIndex<0))      //没有对应的请求命令，事务号不相符
  {
    return;
  }
  
  if((recievedMessage[2]!=0x00)||(recievedMessage[3]!=0x00)) //不是Modbus TCP协议
  {
    return;
  }
  
  if(recievedMessage[7]>0x04)   //功能码大于0x04则不是读命令返回
  {
    return;
  }
  
  uint16_t mLength=(recievedMessage[4]<<8)+recievedMessage[4];
  uint16_t dLength=(uint16_t)recievedMessage[8];
  if(mLength!=dLength+3)        //数据长度不一致
  {
    return;
  }
  
  FunctionCode fuctionCode=(FunctionCode)recievedMessage[7];
  
  if(fuctionCode!=client->pReadCommand[cmdIndex][7])
  {
    return;
  }
  
  uint16_t startAddress=(uint16_t)client->pReadCommand[cmdIndex][8];
  startAddress=(startAddress<<8)+(uint16_t)client->pReadCommand[cmdIndex][9];
  uint16_t quantity=(uint16_t)client->pReadCommand[cmdIndex][10];
  quantity=(quantity<<8)+(uint16_t)client->pReadCommand[cmdIndex][11];
  
  if(quantity*2!=dLength)       //请求的数据长度与返回的数据长度不一致
  {
    return;
  }
  
  if((fuctionCode>=ReadCoilStatus)&&(fuctionCode<=ReadInputRegister))
  {
    HandleServerRespond[fuctionCode-1](client,recievedMessage,startAddress,quantity);
  }
}

/*发送命令后，向已发送命令列表中添加命令*/
void AddCommandBytesToList(TCPLocalClientType *client,uint8_t *commandBytes)
{
  if(client->cmdOrder>=client->cmdNumber)
  {
    client->cmdOrder=0;
  }
  
  for(int i=0;i<12;i++)
  {
    client->pReadCommand[client->cmdOrder][i]=commandBytes[i];
  }

  client->cmdOrder++;
}

/*接收到返回信息后，判断是否是发送命令列表中命令的返回信息，*/
/*若是则从已发送命令列表中删除命令，若不是则丢弃该条返回信息*/
static int FindCommandForRecievedMessage(TCPLocalClientType *client,uint8_t *recievedMessage)
{
  int cmdIndex=-1;
  for(int i=0;i<client->cmdNumber;i++)
  {
    if((recievedMessage[0]==client->pReadCommand[i][0])&&(recievedMessage[1]==client->pReadCommand[i][1]))
    {
      cmdIndex=i;
      break;
    }
  }
  
  return cmdIndex;
}

/*处理读从站状态量返回信息，读线圈状态位0x01功能码*/
static void HandleReadCoilStatusRespond(TCPLocalClientType *client,uint8_t *receivedMessage,uint16_t startAddress,uint16_t quantity)
{
  bool coilStatus[256];
  
  TransformClientReceivedData(receivedMessage+6,quantity,coilStatus,NULL);
  
  client->pUpdateCoilStatus(1,startAddress,quantity,coilStatus);
}

/*处理读从站状态量返回信息，读输入状态位0x02功能码*/
static void HandleReadInputStatusRespond(TCPLocalClientType *client,uint8_t *receivedMessage,uint16_t startAddress,uint16_t quantity)
{
  bool inputStatus[256];
  
  TransformClientReceivedData(receivedMessage+6,quantity,inputStatus,NULL);
  
  client->pUpdateInputStatus(1,startAddress,quantity,inputStatus);
}

/*处理读从站寄存器值的返回信息，读保持寄存器0x03功能码）*/
static void HandleReadHoldingRegisterRespond(TCPLocalClientType *client,uint8_t *receivedMessage,uint16_t startAddress,uint16_t quantity)
{
  uint16_t holdingRegister[125];
  
  TransformClientReceivedData(receivedMessage+6,quantity,NULL,holdingRegister);
  
  client->pUpdateHoldingRegister(1,startAddress,quantity,holdingRegister);
}

/*处理读从站寄存器值的返回信息，读输入寄存器0x04功能码*/
static void HandleReadInputRegisterRespond(TCPLocalClientType *client,uint8_t *receivedMessage,uint16_t startAddress,uint16_t quantity)
{
  uint16_t inputRegister[125];
  
  TransformClientReceivedData(receivedMessage+6,quantity,NULL,inputRegister);
  
  client->pUpdateInputResgister(1,startAddress,quantity,inputRegister);
}

/* 实例化TCP服务器对象 */
void InstantiateTCPServerObject(TCPAccessedServerType *server,
                                uint8_t ipSegment1,uint8_t ipSegment2,
                                uint8_t ipSegment3,uint8_t ipSegment4)
{
  server->ipAddress.ipSegment[0]=ipSegment1;
  server->ipAddress.ipSegment[1]=ipSegment2;
  server->ipAddress.ipSegment[2]=ipSegment3;
  server->ipAddress.ipSegment[3]=ipSegment4;
  
  server->flagPresetServer=0;
  
  server->pWritedCoilHeadNode.writedCoilNumber=0;
  server->pWritedCoilHeadNode.pWritedCoilNode=NULL;
  
  server->pWritedRegisterHeadNode.writedRegisterNumber=0;
  server->pWritedRegisterHeadNode.pWritedRegisterNode=NULL;
  
  server->pNextNode=NULL;
}

/*初始化TCP客户端对象*/
void InitializeTCPClientObject(TCPLocalClientType *client,
                               uint16_t cmdNumber,
                               uint8_t (*pReadCommand)[12],
                               UpdateCoilStatusType pUpdateCoilStatus,
                               UpdateInputStatusType pUpdateInputStatus,
                               UpdateHoldingRegisterType pUpdateHoldingRegister,
                               UpdateInputResgisterType pUpdateInputResgister
                               )
{
  client->transaction=0;
  
  client->cmdNumber=cmdNumber;
  
  client->cmdOrder=0;
  
  client->pReadCommand=pReadCommand;
  
  client->ServerHeadNode.pServerNode=NULL;
  client->ServerHeadNode.serverNumber=0;

  client->pUpdateCoilStatus=pUpdateCoilStatus!=NULL?pUpdateCoilStatus:UpdateCoilStatus;
  
  client->pUpdateInputStatus=pUpdateInputStatus!=NULL?pUpdateInputStatus:UpdateInputStatus;
  
  client->pUpdateHoldingRegister=(pUpdateHoldingRegister!=NULL)?pUpdateHoldingRegister:UpdateHoldingRegister;
  
  client->pUpdateInputResgister=(pUpdateInputResgister!=NULL)?pUpdateInputResgister:UpdateInputResgister;
}

/* 向TCP客户端添加TCP服务器列表节点 */
void AddTCPServerNode(TCPLocalClientType *client,TCPAccessedServerType *server)
{
  TCPAccessedServerType *currentNode=NULL;
  
  if((client==NULL)||(server==NULL))
  {
    return;
  }
  
  currentNode=client->ServerHeadNode.pServerNode;
  
  if(currentNode==NULL)
  {
    client->ServerHeadNode.pServerNode=server;
  }
  else if(server->ipAddress.ipSegment[3]<currentNode->ipAddress.ipSegment[3])
  {
    client->ServerHeadNode.pServerNode=server;
    server->pNextNode=currentNode;
  }
  else
  {
    while(currentNode->pNextNode!=NULL)
    {
      if((currentNode->ipAddress.ipSegment[3]<=server->ipAddress.ipSegment[3])||(server->ipAddress.ipSegment[3]<currentNode->pNextNode->ipAddress.ipSegment[3]))
      {
        server->pNextNode=currentNode->pNextNode;
        currentNode->pNextNode=server;
        break;
      }
      else
      {
        currentNode=currentNode->pNextNode;
      }
    }
    
    if(currentNode->pNextNode==NULL)
    {
      currentNode->pNextNode=server;
    }
  }
  client->ServerHeadNode.serverNumber++;
}

/* 使能或者失能写从站操作标志位（修改从站的写使能标志位） */
void ModifyWriteTCPServerEnableFlag(TCPLocalClientType *client,uint8_t ipAddress,bool en)
{
  TCPAccessedServerType *currentNode;
  currentNode=client->ServerHeadNode.pServerNode;
  
  while(currentNode!=NULL)
  {
    if(currentNode->ipAddress.ipSegment[3]==ipAddress)
    {
      if(en)
      {
        currentNode->flagPresetServer=1;
      }
      else
      {
        currentNode->flagPresetServer=0;
      }
    }
    currentNode=currentNode->pNextNode;
  }
}

/* 获得从站的写使能标志位的状态 */
bool GetWriteTCPServerEnableFlag(TCPLocalClientType *client,uint8_t ipAddress)
{
  bool status=false;
  
  TCPAccessedServerType *currentNode;
  currentNode=client->ServerHeadNode.pServerNode;
  
  while(currentNode!=NULL)
  {
    if((currentNode->ipAddress.ipSegment[3]==ipAddress)&&(currentNode->flagPresetServer>0))
    {
      status=true;
      break;
    }
    currentNode=currentNode->pNextNode;
  }

  return status;
}

/* 判断当前是否有写操作使能 */
bool CheckWriteTCPServerNone(TCPLocalClientType *client)
{
  bool status=true;
  
  TCPAccessedServerType *currentNode;
  currentNode=client->ServerHeadNode.pServerNode;
  
  while(currentNode!=NULL)
  {
    if(currentNode->flagPresetServer>0)
    {
      status=false;
      break;
    }
    currentNode=currentNode->pNextNode;
  }

  return status;
}

/*********** (C) COPYRIGHT 1999-2016 Moonan Technology *********END OF FILE****/