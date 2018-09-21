/******************************************************************************/
/** 模块名称：Modbus通讯                                                     **/
/** 文件名称：mbasciimaster.h                                                **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：用于声明Modbus ASCII主站相关属性及方法                         **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2018-09-11          尹家军            创建文件               **/
/**                                                                          **/
/******************************************************************************/

#ifndef __modbusasciimaster_h
#define __modbusasciimaster_h

#include "mbascii.h"
#include "mbcommon.h"

/*生成访问服务器的命令*/
uint16_t CreateAccessAsciiSlaveCommand(ObjAccessInfo objInfo,void *dataList,uint8_t *commandBytes);

/*解析收到的服务器相应信息*/
void ParsingAsciiSlaveRespondMessage(uint8_t *recievedMessage, uint8_t *command,uint16_t rxLength);

/*接收到返回信息后，判断是否是发送命令列表中命令的返回信息*/
int FindAsciiCommandForRecievedMessage(uint8_t *recievedMessage,uint8_t (*commandList)[17],uint16_t commandNumber);

#endif
/*********** (C) COPYRIGHT 1999-2018 Moonan Technology *********END OF FILE****/