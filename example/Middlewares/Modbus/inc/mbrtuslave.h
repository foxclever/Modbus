/******************************************************************************/
/** 模块名称：Modbus通讯                                                     **/
/** 文件名称：modbusrtuslave.h                                               **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：用于声明Modbus RTU从站相关属性及方法                           **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2016-04-17          木南              创建文件               **/
/**                                                                          **/
/******************************************************************************/ 

#ifndef __mbrtuslave_h
#define __mbrtuslave_h

#include "mbrtu.h"
#include "mbcommon.h"


/*解析接收到的信息，并返回合成的回复信息和信息的字节长度，通过回调函数*/
uint16_t ParsingMasterAccessCommand(uint8_t *receivedMesasage,uint8_t *respondBytes,uint16_t rxLength,uint8_t StationAddress);

#endif
/*********** (C) COPYRIGHT 1999-2016 Moonan Technology *********END OF FILE****/