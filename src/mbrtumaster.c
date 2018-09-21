/******************************************************************************/
/** 模块名称：Modbus通讯                                                     **/
/** 文件名称：modbusrtumaster.c                                              **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：用于声明Modbus RTU主站相关属性及方法                           **/
/**           1、发送访问请求                                                **/
/**           2、解析返回信息                                                **/
/**           3、根据返回信息修改数据                                        **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2016-04-17          尹家军            创建文件               **/
/**     V1.5.0  2018-01-16          尹家军            修改指令存储及检索     **/
/**                                                                          **/
/******************************************************************************/

#include "mbrtumaster.h"

/*处理读从站状态量返回信息，读线圈状态位0x01功能码*/
static void HandleReadCoilStatusRespond(uint8_t *receivedMesasage,uint16_t startAddress,uint16_t quantity);
/*处理读从站状态量返回信息，读输入状态位0x02功能码*/
static void HandleReadInputStatusRespond(uint8_t *receivedMesasage,uint16_t startAddress,uint16_t quantity);
/*处理读从站寄存器值的返回信息，读保持寄存器0x03功能码）*/
static void HandleReadHoldingRegisterRespond(uint8_t *receivedMesasage,uint16_t startAddress,uint16_t quantity);
/*处理读从站寄存器值的返回信息，读输入寄存器0x04功能码*/
static void HandleReadInputRegisterRespond(uint8_t *receivedMesasage,uint16_t startAddress,uint16_t quantity);
/*判断接收到的信息是否是发送命令的返回信息*/
static bool CheckMessageAgreeWithCommand(uint8_t *recievedMessage,uint8_t *command);

void (*HandleSlaveRespond[])(uint8_t *,uint16_t,uint16_t)={HandleReadCoilStatusRespond,
                                                           HandleReadInputStatusRespond,
                                                           HandleReadHoldingRegisterRespond,
                                                           HandleReadInputRegisterRespond};

/*函数名：CreateAccessSlaveCommand，生成访问服务器的命令*/
/*参数：ObjAccessInfo objInfo,要生成访问命令的对象信息*/
/*      void *dataList,写的数据列表，寄存器为uint16_t类型，状态量为bool类型*/
/*      uint8_t *commandBytes,生成的命令列表*/
/*返回值：uint16_t，生成的命令的长度*/
uint16_t CreateAccessSlaveCommand(ObjAccessInfo objInfo,void *dataList,uint8_t *commandBytes)
{
  uint16_t commandLength=0;
  /*生成读服务器对象的命令，功能码0x01、0x02、0x03、0x04,命令长度8个字节*/
  if((objInfo.functionCode>=ReadCoilStatus)&&(objInfo.functionCode <= ReadInputRegister))
  {
    commandLength=SyntheticReadWriteSlaveCommand(objInfo,NULL,NULL,commandBytes);
  }

  /*生成预置服务器对象的命令，功能码0x05,0x0F,命令长度随发送数据而变*/
  if((objInfo.functionCode==WriteSingleCoil)||(objInfo.functionCode==WriteMultipleCoil))
  {
    bool *statusList=(bool*)dataList;
    commandLength=SyntheticReadWriteSlaveCommand(objInfo,statusList,NULL,commandBytes);
  }
  
  /*生成预置服务器对象的命令，功能码0x06,0x10,命令长度随发送数据而变*/
  if((objInfo.functionCode==WriteSingleRegister)||(objInfo.functionCode==WriteMultipleRegister))
  {
    uint16_t *registerList=(uint16_t*)dataList;
    commandLength=SyntheticReadWriteSlaveCommand(objInfo,NULL,registerList,commandBytes);
  }

  return commandLength;
}

/*解析收到的服务器相应信息*/
/*uint8_t *recievedMessage,接收到的消息列表*/
/*uint8_t *command,发送的读操作命令，若为NULL则在命令列表中查找*/
void ParsingSlaveRespondMessage(uint8_t *recievedMessage,uint8_t *command)
{
  /*如果不是读操作的反回信息不需要处理*/
  if(recievedMessage[1]>0x04)
  {
    return;
  }

  if(command==NULL)
  {
    return;
  }
  
  FunctionCode fuctionCode=(FunctionCode)recievedMessage[1];
  uint16_t startAddress=(uint16_t)command[2];
  startAddress=(startAddress<<8)+(uint16_t)command[3];
  uint16_t quantity=(uint16_t)command[4];
  quantity=(quantity<<8)+(uint16_t)command[5];
  
  if((fuctionCode>=ReadCoilStatus)&&(fuctionCode<=ReadInputRegister))
  {
    HandleSlaveRespond[fuctionCode-1](recievedMessage,startAddress,quantity);
  }
}

/*接收到返回信息后，判断是否是发送命令列表中命令的返回信息，*/
/*若是则从已发送命令列表中删除命令，若不是则丢弃该条返回信息*/
int FindCommandForRecievedMessage(uint8_t *recievedMessage,uint8_t (*commandList)[8],uint16_t commandNumber)
{
  int cmdIndex=-1;
  
    for(int i=0;i<commandNumber;i++)
    {
      if(CheckMessageAgreeWithCommand(recievedMessage,commandList[i])==true)
      {
        cmdIndex=i;
        break;
      }
    }
  return cmdIndex;
}
        
/*判断接收到的信息是否是发送命令的返回信息*/
static bool CheckMessageAgreeWithCommand(uint8_t *recievedMessage,uint8_t *command)
{
  bool aw=false;
  
  if((recievedMessage[0]==command[0])&&(recievedMessage[1]==command[1]))
  {
    uint16_t quantity=(uint16_t)command[4];
    quantity=(quantity<<8)+(uint16_t)command[5];
    uint8_t bytescount=0;
    if((recievedMessage[1]==ReadCoilStatus)||(recievedMessage[1]==ReadInputStatus))
    {
      bytescount=(uint8_t)((quantity-1)/8+1);
    }
    
    if((recievedMessage[1]==ReadHoldingRegister)||(recievedMessage[1]==ReadInputRegister))
    {
      bytescount=quantity*2;
    }
    
    if(recievedMessage[2]==bytescount)
    {
      aw=true;
    }
  }
  
  return aw;
}
/*处理读从站状态量返回信息，读线圈状态位0x012功能码*/
static void HandleReadCoilStatusRespond(uint8_t *receivedMessage,uint16_t startAddress,uint16_t quantity)
{
  bool coilStatus[256];
  
  TransformClientReceivedData(receivedMessage,quantity,coilStatus,NULL);
  
  UpdateCoilStatus(startAddress,quantity,coilStatus);
}

/*处理读从站状态量返回信息，读输入状态位0x02功能码*/
static void HandleReadInputStatusRespond(uint8_t *receivedMessage,uint16_t startAddress,uint16_t quantity)
{
  bool inputStatus[256];
  
  TransformClientReceivedData(receivedMessage,quantity,inputStatus,NULL);
  
  UpdateInputStatus(startAddress,quantity,inputStatus);
}

/*处理读从站寄存器值的返回信息，读保持寄存器0x03功能码）*/
static void HandleReadHoldingRegisterRespond(uint8_t *receivedMessage,uint16_t startAddress,uint16_t quantity)
{
  uint16_t holdingRegister[125];
  
  TransformClientReceivedData(receivedMessage,quantity,NULL,holdingRegister);
  
  UpdateHoldingRegister(startAddress,quantity,holdingRegister);
}

/*处理读从站寄存器值的返回信息，读输入寄存器0x04功能码*/
static void HandleReadInputRegisterRespond(uint8_t *receivedMessage,uint16_t startAddress,uint16_t quantity)
{
  uint16_t inputRegister[125];
  
  TransformClientReceivedData(receivedMessage,quantity,NULL,inputRegister);
  
  UpdateInputResgister(startAddress,quantity,inputRegister);
}

/*********** (C) COPYRIGHT 1999-2016 Moonan Technology *********END OF FILE****/