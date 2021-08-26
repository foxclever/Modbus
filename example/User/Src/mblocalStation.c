/******************************************************************************/
/** 模块名称：Modbus本地从站模块                                             **/
/** 文件名称：mcmbsprocess.c                                                 **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：Modbus本地从站模块软件，实现面向Modbus Slave(Server)的协议栈   **/
/**           回调函数，包括RTU和TCP应用                                     **/
/**           基于STM32F407ZGT6硬件平台，软件库采用HAL FW_F4 V1.26.0库       **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2021-03-29          尹家军            创建文件               **/
/**                                                                          **/
/******************************************************************************/ 

#include "mbconfig.h"
#include "noos_config.h"

AnalogParaTypeDef aPara;
DigitalParaTypeDef dPara;

/*获取想要读取的Coil量的值*/
void GetCoilStatus(uint16_t startAddress,uint16_t quantity,bool *statusList)
{
  uint16_t start;
  uint16_t count;
  /*先判断地址是否处于合法范围*/
  start=(startAddress>CoilStartAddress)?((startAddress<=CoilEndAddress)?startAddress:CoilEndAddress):CoilStartAddress;
  count=((start+quantity-1)<=CoilEndAddress)?quantity:(CoilEndAddress-start);
  
  for(int i=0;i<count;i++)
  {
    statusList[i]=dPara.coil[start+i];
  }
}

/*获取想要读取的保持寄存器的值*/
void GetHoldingRegister(uint16_t startAddress,uint16_t quantity,uint16_t *registerValue)
{
  uint16_t start;
  uint16_t count;
  /*先判断地址是否处于合法范围*/
  start=(startAddress>HoldingRegisterStartAddress)?((startAddress<=HoldingRegisterEndAddress)?startAddress:HoldingRegisterEndAddress):HoldingRegisterStartAddress;
  count=((start+quantity-1)<=HoldingRegisterEndAddress)?quantity:(HoldingRegisterEndAddress-start);
  
  for(int i=0;i<count;i++)
  {
    registerValue[i]=aPara.holdingRegister[start+i];
  }
}

/*设置单个线圈的值*/
void SetSingleCoil(uint16_t coilAddress,bool coilValue)
{
  /*先判断地址是否处于合法范围*/
    bool noError=(bool)(((4<=coilAddress)&&(coilAddress<=7))
                      ||((12<=coilAddress)&&(coilAddress<=15))
                          ||((20<=coilAddress)&&(coilAddress<=23))
                              ||((28<=coilAddress)&&(coilAddress<=31))
                                  ||((36<=coilAddress)&&(coilAddress<=39)));
    
    
  if(noError)
  {
    dPara.coil[coilAddress]=coilValue;
  }
}

/*设置多个线圈的值*/
void SetMultipleCoil(uint16_t startAddress,uint16_t quantity,bool *statusValue)
{
  uint16_t endAddress=startAddress+quantity-1;
  
  bool noError=(bool)(((4<=startAddress)&&(startAddress<=7)&&(4<=endAddress)&&(endAddress<=7))
                      ||((12<=startAddress)&&(startAddress<=15)&&(12<=endAddress)&&(endAddress<=15))
                      ||((20<=startAddress)&&(startAddress<=23)&&(20<=endAddress)&&(endAddress<=23))
                          ||((28<=startAddress)&&(startAddress<=31)&&(28<=endAddress)&&(endAddress<=31))
                              ||((36<=startAddress)&&(startAddress<=39)&&(36<=endAddress)&&(endAddress<=39)));
  
  if(noError)
  {
    for(int i=0;i<quantity;i++)
    {
      dPara.coil[i+startAddress]=statusValue[i];
    }
  }
}

/*设置单个寄存器的值*/
void SetSingleRegister(uint16_t registerAddress,uint16_t registerValue)
{
  bool noError=(bool)(((7<=registerAddress)&&(registerAddress<=7))
                      ||((13<=registerAddress)&&(registerAddress<=15))
                          ||((21<=registerAddress)&&(registerAddress<=23))
                              ||((29<=registerAddress)&&(registerAddress<=31))
                                  ||((37<=registerAddress)&&(registerAddress<=39)));
  if(noError)
  {
    aPara.holdingRegister[registerAddress]=registerValue;
  }
  
}


/*设置多个寄存器的值*/
void SetMultipleRegister(uint16_t startAddress,uint16_t quantity,uint16_t *registerValue)
{
  uint16_t endAddress=startAddress+quantity-1;
  
  bool noError=(bool)(((4<=startAddress)&&(startAddress<=5)&&(4<=endAddress)&&(endAddress<=5))
                      ||((7<=startAddress)&&(startAddress<=7)&&(7<=endAddress)&&(endAddress<=7))
                      ||((13<=startAddress)&&(startAddress<=15)&&(13<=endAddress)&&(endAddress<=15))
                      ||((21<=startAddress)&&(startAddress<=23)&&(21<=endAddress)&&(endAddress<=23))
                          ||((29<=startAddress)&&(startAddress<=31)&&(29<=endAddress)&&(endAddress<=31))
                              ||((37<=startAddress)&&(startAddress<=39)&&(37<=endAddress)&&(endAddress<=39)));
  if(noError)
  {
    for(int i=0;i<quantity;i++)
    {
      aPara.holdingRegister[startAddress+i]=registerValue[i];
    }
  }

}


                         
/*********** (C) COPYRIGHT 1999-2021 Moonan Technology *********END OF FILE****/
