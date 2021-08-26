/******************************************************************************/
/** 模块名称：Modbus通讯                                                     **/
/** 文件名称：mbrtu.h                                                        **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：用于实现Modbus RTU总线协议栈ADU的封装                          **/
/**           声明和定义Modbus总线在串行链路上的实现的相关属性和方法         **/
/**           对串行链路RTU方式进行具体描述                                  **/
/**                                                                          **/
/* 一个典型的Modbus数据帧有如下部分组成：                                     */
/* <------------------- MODBUS串行链路应用数据单元（ADU） ----------------->  */
/*              <------ MODBUS简单协议数据单元（PDU） ------->                */
/*  +-----------+---------------+----------------------------+-------------+  */
/*  |  地址域   |    功能码     | 数据域                     | CRC/LRC     |  */
/*  +-----------+---------------+----------------------------+-------------+  */
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2016-04-17          木南              创建文件               **/
/**                                                                          **/
/******************************************************************************/ 

#ifndef __mbrtu_h
#define __mbrtu_h

#include "mbpdu.h"

/*通过CRC校验校验接收的信息是否正确*/
bool CheckRTUMessageIntegrity (uint8_t *message,uint8_t length);

/*生成读写从站数据对象的命令,命令长度包括2个校验字节*/
uint16_t SyntheticReadWriteSlaveCommand(ObjAccessInfo slaveInfo,bool *statusList,uint16_t *registerList,uint8_t *commandBytes);

/*生成从站应答主站的响应*/
uint16_t SyntheticSlaveAccessRespond(uint8_t *receivedMesasage,bool *statusList,uint16_t *registerList,uint8_t *respondBytes);

#endif
/*********** (C) COPYRIGHT 1999-2016 Moonan Technology *********END OF FILE****/