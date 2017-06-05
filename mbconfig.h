/******************************************************************************/
/** 模块名称：Modbus通讯                                                     **/
/** 文件名称：mbconfig.h                                                     **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：用于配置Modbus协议栈使用的相关定义                             **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2015-07-18          尹家军            创建文件               **/
/**                                                                          **/
/******************************************************************************/ 


#ifndef _MB_CONFIG_H
#define _MB_CONFIG_H

/*定义是否使能RTU主站功能，0为禁用，1为使能*/
#define MB_RTU_MASTER_ENABLED		(0)

/*定义是否使能RTU从站功能，0为禁用，1为使能*/
#define MB_RTU_SLAVE_ENABLED		(0)

/*定义是否使能ASCII主站功能，0为禁用，1为使能*/
#define MB_ACSII_MASTER_ENABLED		(0)

/*定义是否使能ASCII从站功能，0为禁用，1为使能*/
#define MB_ASCII_SLAVE_ENABLED		(0)

/*定义是否使能TCP服务器功能，0为禁用，1为使能*/
#define MB_TCP_SERVER_ENABLED		(0)

/*定义是否使能TCP客户端功能，0为禁用，1为使能*/
#define MB_TCP_CLIENT_ENABLED		(0)

#if MB_RTU_MASTER_ENABLED > (0)
#include "mbrtumaster.h"
#endif

#if MB_RTU_SLAVE_ENABLED > (0)
#include "mbrtuslave.h"
#endif

#if MB_TCP_CLIENT_ENABLED > (0)
#include "mbtcpclient.h"
#endif

#if MB_TCP_SERVER_ENABLED > (0)
#include "mbtcpserver.h"
#endif

#include "mbcommon.h"

#endif
/*********** (C) COPYRIGHT 1999-2016 Moonan Technology *********END OF FILE****/