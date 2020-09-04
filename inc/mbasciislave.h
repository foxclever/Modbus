/******************************************************************************/
/** 模块名称：Modbus通讯                                                     **/
/** 文件名称：mbasciislave.h                                                 **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：用于声明Modbus ASCII从站相关属性及方法                         **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2018-09-11          木南              创建文件               **/
/**                                                                          **/
/******************************************************************************/

#ifndef __modbusasciislave_h
#define __modbusasciislave_h

#include "mbascii.h"
#include "mbcommon.h"

/*解析接收到的信息，并返回合成的回复信息和信息的字节长度，通过回调函数*/
uint16_t ParsingAsciiMasterAccessCommand(uint8_t *receivedMessage, uint8_t *respondBytes, uint16_t rxLength, uint8_t StationAddress);

#endif
/*********** (C) COPYRIGHT 1999-2018 Moonan Technology *********END OF FILE****/