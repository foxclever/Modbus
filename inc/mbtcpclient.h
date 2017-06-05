/******************************************************************************/
/** 模块名称：Modbus通讯                                                     **/
/** 文件名称：mbtcpclient.c                                                  **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：用于声明Modbus TCP客户端的相关属性及方法                       **/
/**           1、Modbus TCP客户端用户应用的接口层                            **/
/**           2、实现服务器访问命令的生成并将其传回应用层                    **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2016-04-17          尹家军            创建文件               **/
/**                                                                          **/
/******************************************************************************/

#ifndef __mbtcpclient_h
#define __mbtcpclient_h

#include "mbtcp.h"
#include "mbcommon.h"

/*生成访问服务器的命令*/
uint16_t CreateAccessServerCommand(ObjAccessInfo objInfo,void *dataList,uint8_t *commandBytes);

/*解析收到的服务器相应信息*/
void ParsingServerRespondMessage(uint8_t *recievedMessage);

#endif
/*********** (C) COPYRIGHT 1999-2016 Moonan Technology *********END OF FILE****/