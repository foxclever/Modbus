/******************************************************************************/
/** 模块名称：Modbus通讯                                                     **/
/** 文件名称：mbascii.c                                                      **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：用于实现Modbus ASCII总线协议栈ADU的封装                        **/
/**           声明和定义Modbus总线在串行链路上的实现的相关属性和方法         **/
/**           对串行链路ASCII方式进行具体描述                                **/
/**                                                                          **/
/* 一个典型的Modbus数据帧有如下部分组成：                                     */
/* <------------------- MODBUS串行链路应用数据单元（ADU） ----------------->  */
/*              <------ MODBUS简单协议数据单元（PDU） ------->                */
/*  +-----------+---------------+----------------------------+-------------+  */
/*  |  地址域   |    功能码     | 数据域                     | CRC/LRC     |  */
/*  +-----------+---------------+----------------------------+-------------+  */
/*  对于ASCII编码则要在前面加上起始符“：”(0x3A)以及结束符“回车符”(0x0D)   */
/*  和“换行符”(0x0A)                                                       **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2018-09-11          木南              创建文件               **/
/**                                                                          **/
/******************************************************************************/

#ifndef __mbascii_h
#define __mbascii_h

#include "stdint.h"
#include "mbpdu.h"

/*生成读写从站的命令，应用于主站，含校验及起始结束符*/
uint16_t SyntheticReadWriteAsciiSlaveCommand(ObjAccessInfo slaveInfo, bool *statusList, uint16_t *registerList, uint8_t *commandBytes);

/*生成应答主站的响应，应用于从站*/
uint16_t SyntheticAsciiSlaveAccessRespond(uint8_t *receivedMessage, bool *statusList, uint16_t *registerList, uint8_t *respondBytes);

/*接收到的ASCII消息转换为16进制*/
bool CovertAsciiMessageToHex(uint8_t *aMsg, uint8_t *hMsg, uint16_t aLen);

/*判断ASCII数据信息是否正确*/
bool CheckASCIIMessageIntegrity(uint8_t *usMsg, uint16_t usLength);

#endif
/*********** (C) COPYRIGHT 1999-2018 Moonan Technology *********END OF FILE****/