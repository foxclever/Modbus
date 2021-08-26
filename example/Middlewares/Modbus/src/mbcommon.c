/******************************************************************************/
/** 模块名称：Modbus通讯                                                     **/
/** 文件名称：mbcommon.c                                                     **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：用于实现Modbus各种情况下的公用部分                             **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2015-07-18          木南              创建文件               **/
/**                                                                          **/
/******************************************************************************/ 

#include "mbcommon.h"

/*将接收到的写单个Coil值转化为布尔量，对应0x05功能码*/
bool CovertSingleCommandCoilToBoolStatus(uint16_t coilValue,bool value)
{
  bool state=value;
  if(coilValue==0x0000)
  {
    state=false;
  }
  if(coilValue==0xFF00)
  {
    state=true;
  }
  return state;
}

/*检验所写数据是否符合物理量要求范围并处理(单精度浮点数)*/
float CheckWriteFloatDataIsValid(float value,float range,float zero)
{
  if(value>=range)
  {
    return range;
  }
  else if(value<=zero)
  {
    return zero;
  }
  else
  {
    return value;
  }
}

/*检验所写数据是否符合物理量要求范围并处理(双精度浮点数)*/
double CheckWriteDoubleDataIsValid(double value,double range,double zero)
{
  if(value>=range)
  {
    return range;
  }
  else if(value<=zero)
  {
    return zero;
  }
  else
  {
    return value;
  }
}

/*检验所写数据是否符合物理量要求范围并处理(16位整数)*/
uint16_t CheckWriteInt16DataIsValid(uint16_t value,uint16_t range,uint16_t zero)
{
  if(value>=range)
  {
    return range;
  }
  else if(value<=zero)
  {
    return zero;
  }
  else
  {
    return value;
  }
}

/*检验所写数据是否符合物理量要求范围并处理(32位整数)*/
uint32_t CheckWriteInt32DataIsValid(uint32_t value,uint32_t range,uint32_t zero)
{
  if(value>=range)
  {
    return range;
  }
  else if(value<=zero)
  {
    return zero;
  }
  else
  {
    return value;
  }
}

/*获取想要读取的Coil量的值*/
__weak void GetCoilStatus(uint16_t startAddress,uint16_t quantity,bool *statusList)
{
  //如果需要Modbus TCP Server/RTU Slave应用中实现具体内容
}

/*获取想要读取的InputStatus量的值*/
__weak void GetInputStatus(uint16_t startAddress,uint16_t quantity,bool *statusValue)
{
  //如果需要Modbus TCP Server/RTU Slave应用中实现具体内容
}

/*获取想要读取的保持寄存器的值*/
__weak void GetHoldingRegister(uint16_t startAddress,uint16_t quantity,uint16_t *registerValue)
{
  //如果需要Modbus TCP Server/RTU Slave应用中实现具体内容
}

/*获取想要读取的输入寄存器的值*/
__weak void GetInputRegister(uint16_t startAddress,uint16_t quantity,uint16_t *registerValue)
{
  //如果需要Modbus TCP Server/RTU Slave应用中实现具体内容
}

/*设置单个线圈的值*/
__weak void SetSingleCoil(uint16_t coilAddress,bool coilValue)
{
  //如果需要Modbus TCP Server/RTU Slave应用中实现具体内容
}

/*设置单个寄存器的值*/
__weak void SetSingleRegister(uint16_t registerAddress,uint16_t registerValue)
{
  //如果需要Modbus TCP Server/RTU Slave应用中实现具体内容
}

/*设置多个线圈的值*/
__weak void SetMultipleCoil(uint16_t startAddress,uint16_t quantity,bool *statusValue)
{
  //如果需要Modbus TCP Server/RTU Slave应用中实现具体内容
}

/*设置多个寄存器的值*/
__weak void SetMultipleRegister(uint16_t startAddress,uint16_t quantity,uint16_t *registerValue)
{
  //如果需要Modbus TCP Server/RTU Slave应用中实现具体内容
}

/*更新读回来的线圈状态*/
__weak void UpdateCoilStatus(uint8_t salveAddress,uint16_t startAddress,uint16_t quantity,bool *stateValue)
{
  //在客户端（主站）应用中实现
}

/*更新读回来的输入状态值*/
__weak void UpdateInputStatus(uint8_t salveAddress,uint16_t startAddress,uint16_t quantity,bool *stateValue)
{
  //在客户端（主站）应用中实现
}

/*更新读回来的保持寄存器*/
__weak void UpdateHoldingRegister(uint8_t salveAddress,uint16_t startAddress,uint16_t quantity,uint16_t *registerValue)
{
  //在客户端（主站）应用中实现
}

/*更新读回来的输入寄存器*/
__weak void UpdateInputResgister(uint8_t salveAddress,uint16_t startAddress,uint16_t quantity,uint16_t *registerValue)
{
  //在客户端（主站）应用中实现
}

/*********** (C) COPYRIGHT 1999-2016 Moonan Technology *********END OF FILE****/