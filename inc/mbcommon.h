/******************************************************************************/
/** 模块名称：Modbus通讯                                                     **/
/** 文件名称：mbcommon.h                                                     **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：用于实现Modbus各种情况下的公用部分                             **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2015-07-18          尹家军            创建文件               **/
/**                                                                          **/
/******************************************************************************/ 

#include "stdbool.h"
#include "stdint.h"

/*将接收到的写单个Coil值转化为布尔量，对应0x05功能码*/
bool CovertSingleCommandCoilToBoolStatus(uint16_t coilValue,bool value);

/*获取想要读取的Coil量的值*/
void GetCoilStatus(uint16_t startAddress,uint16_t quantity,bool *statusList);

/*获取想要读取的InputStatus量的值*/
void GetInputStatus(uint16_t startAddress,uint16_t quantity,bool *statusValue);

/*获取想要读取的保持寄存器的值*/
void GetHoldingRegister(uint16_t startAddress,uint16_t quantity,uint16_t *registerValue);

/*获取想要读取的输入寄存器的值*/
void GetInputRegister(uint16_t startAddress,uint16_t quantity,uint16_t *registerValue);

/*设置单个线圈的值*/
void SetSingleCoil(uint16_t coilAddress,bool coilValue);

/*设置单个寄存器的值*/
void SetSingleRegister(uint16_t registerAddress,uint16_t registerValue);

/*设置多个线圈的值*/
void SetMultipleCoil(uint16_t startAddress,uint16_t quantity,bool *statusValue);

/*设置多个寄存器的值*/
void SetMultipleRegister(uint16_t startAddress,uint16_t quantity,uint16_t *registerValue);

/*更新读回来的线圈状态*/
void UpdateCoilStatus(uint16_t startAddress,uint16_t quantity,bool *stateValue);

/*更新读回来的输入状态值*/
void UpdateInputStatus(uint16_t startAddress,uint16_t quantity,bool *stateValue);

/*更新读回来的保持寄存器*/
void UpdateHoldingRegister(uint16_t startAddress,uint16_t quantity,uint16_t *registerValue);

/*更新读回来的输入寄存器*/
void UpdateInputResgister(uint16_t startAddress,uint16_t quantity,uint16_t *registerValue);

/*********** (C) COPYRIGHT 1999-2016 Moonan Technology *********END OF FILE****/