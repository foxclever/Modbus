/******************************************************************************/
/** 模块名称：Modbus通讯                                                     **/
/** 文件名称：modbusrtumaster.h                                              **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：用于声明Modbus RTU主站相关属性及方法                           **/
/**           1、发送访问请求                                                **/
/**           2、解析返回信息                                                **/
/**           3、根据返回信息修改数据                                        **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2016-04-17          尹家军            创建文件               **/
/**                                                                          **/
/******************************************************************************/

#ifndef __mbrtumaster_h
#define __mbrtumaster_h

#include "mbrtu.h"
#include "mbcommon.h"

/*生成访问服务器的命令*/
uint16_t CreateAccessSlaveCommand(ObjAccessInfo objInfo,void *dataList,uint8_t *commandBytes);

/*解析收到的服务器相应信息*/
void ParsingSlaveRespondMessage(uint8_t *recievedMessage,uint8_t *command);

/*接收到返回信息后，判断是否是发送命令列表中命令的返回信息*/
int FindCommandForRecievedMessage(uint8_t *recievedMessage,uint8_t (*commandList)[8],uint16_t commandNumber);

#endif
/*********** (C) COPYRIGHT 1999-2016 Moonan Technology *********END OF FILE****/