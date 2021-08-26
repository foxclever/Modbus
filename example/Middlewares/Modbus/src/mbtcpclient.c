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
/**     V1.0.0  2016-04-17          木南              创建文件               **/
/**     V1.1.0  2020-09-01          木南              修正一些已知问题       **/
/**                                                                          **/
/******************************************************************************/ 

#include "mbtcpclient.h"

/*生成MBAP报文头*/
static uint16_t CreateMbapHeadPart(TCPLocalClientType *client,uint8_t * mbapHead);
/* 向TCP客户端添加TCP服务器列表节点 */
static void AddTCPServerNode(TCPLocalClientType *client,TCPAccessedServerType *server);
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
uint16_t CreateAccessServerCommand(TCPLocalClientType *client,ObjAccessInfo objInfo,void *dataList,uint8_t *commandBytes)
{
  uint16_t commandLength=0;
  
  CreateMbapHeadPart(client,commandBytes);
  
  /*生成读服务器对象的命令，功能码0x01、0x02、0x03、0x04,命令长度12个字节*/
  if((objInfo.functionCode>=ReadCoilStatus)&&(objInfo.functionCode <= ReadInputRegister))
  {
    commandLength=SyntheticReadWriteTCPServerCommand(objInfo,NULL,NULL,commandBytes);
  }
  
  /*生成预置服务器对象的命令，功能码0x05,0x0F,命令长度随发送数据而变*/
  if((objInfo.functionCode==WriteSingleCoil)||(objInfo.functionCode==WriteMultipleCoil))
  {
    bool *statusList=(bool*)dataList;
    commandLength=SyntheticReadWriteTCPServerCommand(objInfo,statusList,NULL,commandBytes);
  }
  
  /*生成预置服务器对象的命令，功能码0x06,0x10,命令长度随发送数据而变*/
  if((objInfo.functionCode==WriteSingleRegister)||(objInfo.functionCode==WriteMultipleRegister))
  {
    uint16_t *registerList=(uint16_t*)dataList;
    commandLength=SyntheticReadWriteTCPServerCommand(objInfo,NULL,registerList,commandBytes);
  }
  
  return commandLength;
}

/*解析收到的服务器相应信息*/
void ParsingServerRespondMessage(TCPLocalClientType *client,uint8_t *recievedMessage)
{
  TCPAccessedServerType *currentServer=client->pCurrentServer;
  
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
  
  uint16_t mLength=(recievedMessage[4]<<8)+recievedMessage[5];
  uint16_t dLength=(uint16_t)recievedMessage[8];
  if(mLength!=dLength+3)        //数据长度不一致
  {
    return;
  }
  
  FunctionCode fuctionCode=(FunctionCode)recievedMessage[7];
  
  if(fuctionCode!=currentServer->pReadCommand[cmdIndex][7])
  {
    return;
  }
  
  uint16_t startAddress=(uint16_t)currentServer->pReadCommand[cmdIndex][8];
  startAddress=(startAddress<<8)+(uint16_t)currentServer->pReadCommand[cmdIndex][9];
  uint16_t quantity=(uint16_t)currentServer->pReadCommand[cmdIndex][10];
  quantity=(quantity<<8)+(uint16_t)currentServer->pReadCommand[cmdIndex][11];
  
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
  TCPAccessedServerType *currentServer=client->pCurrentServer;

  if(currentServer->cmdOrder>=currentServer->cmdNumber)
  {
    currentServer->cmdOrder=0;
  }
  
  for(int i=0;i<12;i++)
  {
    currentServer->pReadCommand[currentServer->cmdOrder][i]=commandBytes[i];
  }
  
  currentServer->cmdOrder++;
}

/*接收到返回信息后，判断是否是发送命令列表中命令的返回信息，*/
/*若是则从已发送命令列表中删除命令，若不是则丢弃该条返回信息*/
static int FindCommandForRecievedMessage(TCPLocalClientType *client,uint8_t *recievedMessage)
{
  int cmdIndex=-1;
  TCPAccessedServerType *currentServer=client->pCurrentServer;
  
  for(int i=0;i<currentServer->cmdNumber;i++)
  {
    if((recievedMessage[0]==currentServer->pReadCommand[i][0])&&(recievedMessage[1]==currentServer->pReadCommand[i][1]))
    {
      cmdIndex=i;
      break;
    }
  }
  
  return cmdIndex;
}

/*处理读服务器状态量返回信息，读线圈状态位0x01功能码*/
static void HandleReadCoilStatusRespond(TCPLocalClientType *client,uint8_t *receivedMessage,uint16_t startAddress,uint16_t quantity)
{
  bool coilStatus[256];
  
  TransformClientReceivedData(receivedMessage+6,quantity,coilStatus,NULL);
  
  uint8_t serverAddress=client->pCurrentServer->ipAddress.ipSegment[3];
  
  client->pUpdateCoilStatus(serverAddress,startAddress,quantity,coilStatus);
}

/*处理读服务器状态量返回信息，读输入状态位0x02功能码*/
static void HandleReadInputStatusRespond(TCPLocalClientType *client,uint8_t *receivedMessage,uint16_t startAddress,uint16_t quantity)
{
  bool inputStatus[256];
  
  TransformClientReceivedData(receivedMessage+6,quantity,inputStatus,NULL);
  
  uint8_t serverAddress=client->pCurrentServer->ipAddress.ipSegment[3];
  
  client->pUpdateInputStatus(serverAddress,startAddress,quantity,inputStatus);
}

/*处理读服务器寄存器值的返回信息，读保持寄存器0x03功能码）*/
static void HandleReadHoldingRegisterRespond(TCPLocalClientType *client,uint8_t *receivedMessage,uint16_t startAddress,uint16_t quantity)
{
  uint16_t holdingRegister[125];
  
  TransformClientReceivedData(receivedMessage+6,quantity,NULL,holdingRegister);
  
  uint8_t serverAddress=client->pCurrentServer->ipAddress.ipSegment[3];
  
  client->pUpdateHoldingRegister(serverAddress,startAddress,quantity,holdingRegister);
}

/*处理读服务器寄存器值的返回信息，读输入寄存器0x04功能码*/
static void HandleReadInputRegisterRespond(TCPLocalClientType *client,uint8_t *receivedMessage,uint16_t startAddress,uint16_t quantity)
{
  uint16_t inputRegister[125];
  
  TransformClientReceivedData(receivedMessage+6,quantity,NULL,inputRegister);
  
  uint8_t serverAddress=client->pCurrentServer->ipAddress.ipSegment[3];
  
  client->pUpdateInputResgister(serverAddress,startAddress,quantity,inputRegister);
}

/*生成MBAP头数据,为了方便与RTU相同处理，将单元标识符（远程从站地址）放在生成命令时处理*/
/*此处MBAP头只有7个字节，最后单元标识符为默认值0x01*/
static uint16_t CreateMbapHeadPart(TCPLocalClientType *client,uint8_t * mbapHead)
{
  uint16_t index=0;
  //产生事务标志
  if(client->transaction==65535)
  {
    client->transaction=1;
  }
  else
  {
    client->transaction=client->transaction+1;
  }
  mbapHead[index++]=client->transaction>>8;//事务标识符
  mbapHead[index++]=client->transaction;//事务标识符
  mbapHead[index++]=0x00;//协议标识符，modbus标识符为0
  mbapHead[index++]=0x00;//协议标识符，modbus标识符为0
  mbapHead[index++]=0x00;//字节长度，此处尚不能确定，在后续更新
  mbapHead[index++]=0x01;//字节长度，此处尚不能确定，在后续更新
  mbapHead[index++]=0x01;//单元标识符默认值
  return index;
}

/*初始化TCP客户端对象*/
void InitializeTCPClientObject(TCPLocalClientType *client,
                               UpdateCoilStatusType pUpdateCoilStatus,
                               UpdateInputStatusType pUpdateInputStatus,
                               UpdateHoldingRegisterType pUpdateHoldingRegister,
                               UpdateInputResgisterType pUpdateInputResgister
                                 )
{
  client->transaction=0;

  client->pServerList=NULL;
  client->pCurrentServer=NULL;
  
  client->pUpdateCoilStatus=pUpdateCoilStatus!=NULL?pUpdateCoilStatus:UpdateCoilStatus;
  
  client->pUpdateInputStatus=pUpdateInputStatus!=NULL?pUpdateInputStatus:UpdateInputStatus;
  
  client->pUpdateHoldingRegister=(pUpdateHoldingRegister!=NULL)?pUpdateHoldingRegister:UpdateHoldingRegister;
  
  client->pUpdateInputResgister=(pUpdateInputResgister!=NULL)?pUpdateInputResgister:UpdateInputResgister;
}

/* 实例化TCP服务器对象 */
void InstantiateTCPServerObject(TCPAccessedServerType *server,          //要实例化的服务器对象
                                TCPLocalClientType *client,             //服务器所属本地客户端对象
                                uint8_t ipSegment1,                     //IP地址第1段
                                uint8_t ipSegment2,                     //IP地址第2段
                                uint8_t ipSegment3,                     //IP地址第3段
                                uint8_t ipSegment4,                     //IP地址第4段
                                uint16_t port,                          //端口，默认为502
                                uint16_t cmdNumber,                     //读命令的数量，最多127
                                uint8_t(*pReadCommand)[12],             //读命令列表
                                uint16_t writedCoilNumber,              //可写线圈量节点的数量
                                WritedCoilListNode *pCoilList,          //写线圈列表
                                uint16_t writedRegisterNumber,          //可写寄存器量节点的数量
                                WritedRegisterListNode *pRegisterList)  //写寄存器列表
{
    if ((server == NULL) || (client == NULL))
    {
        return;
    }

  server->ipAddress.ipSegment[0]=ipSegment1;
  server->ipAddress.ipSegment[1]=ipSegment2;
  server->ipAddress.ipSegment[2]=ipSegment3;
  server->ipAddress.ipSegment[3]=ipSegment4;
  
  server->port = port > 0 ? port : 502;

  server->flagPresetServer=0;
  server->cmdNumber = cmdNumber;
  server->cmdOrder = 0;
  server->pReadCommand = pReadCommand;
  
  server->writedCoilNumber=writedCoilNumber;
  server->pWritedCoilList=pCoilList;
  
  server->writedRegisterNumber=writedRegisterNumber;
  server->pWritedRegisterList=pRegisterList;
  
  server->pNextNode=NULL;
  
  /* 向TCP客户端添加TCP服务器列表节点 */
  AddTCPServerNode(client,server);
}

/* 向TCP客户端添加TCP服务器列表节点 */
static void AddTCPServerNode(TCPLocalClientType *client,TCPAccessedServerType *server)
{
  TCPAccessedServerType *currentNode=NULL;
  
  if((client==NULL)||(server==NULL))
  {
    return;
  }

  currentNode=client->pServerList;
  
  if(currentNode==NULL)
  {
    client->pServerList=server;
  }
  else if(server->ipAddress.ipSegment[3]<currentNode->ipAddress.ipSegment[3])
  {
    client->pServerList=server;
    server->pNextNode=currentNode;
  }
  else
  {
    while(currentNode->pNextNode!=NULL)
    {
      if((currentNode->ipAddress.ipSegment[3]<=server->ipAddress.ipSegment[3])&&(server->ipAddress.ipSegment[3]<currentNode->pNextNode->ipAddress.ipSegment[3]))
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
}

/* 使能或者失能写服务器操作标志位（修改服务器的写使能标志位） */
void ModifyWriteTCPServerEnableFlag(TCPLocalClientType *client,uint8_t ipAddress,bool en)
{
  TCPAccessedServerType *currentNode;
  currentNode=client->pServerList;
  
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

/* 获得服务器的写使能标志位的状态 */
bool GetWriteTCPServerEnableFlag(TCPLocalClientType *client,uint8_t ipAddress)
{
  bool status=false;
  
  TCPAccessedServerType *currentNode;
  currentNode=client->pServerList;
  
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
  currentNode=client->pServerList;
  
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