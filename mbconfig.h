/******************************************************************************/
/** 模块名称：Modbus通讯                                                     **/
/** 文件名称：mbconfig.h                                                     **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：用于配置Modbus协议栈使用的相关定义                             **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期           作者         说明                           **/
/**     V1.0.0  2015-07-18      尹家军     创建文件                          **/
/**     V1.0.1  2018-09-21      尹家军     添加Modbus ASCII代码              **/
/**     V1.1.0  2019-04-17      尹家军     修改主站代码结构，封装主站从站对象**/
/**                                                                          **/
/******************************************************************************/ 


#ifndef _MB_CONFIG_H
#define _MB_CONFIG_H

/*定义是否使能RTU主站功能，0为禁用，1为使能*/
#define MB_RTU_MASTER_ENABLED		(1)

/*定义是否使能RTU从站功能，0为禁用，1为使能*/
#define MB_RTU_SLAVE_ENABLED		(1)

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

#if MB_ACSII_MASTER_ENABLED > (0)
#include "mbasciimaster.h"
#endif

#if MB_ASCII_SLAVE_ENABLED > (0)
#include "mbasciislave.h"
#endif

#include "mbcommon.h"

/* 作为从站时，允许操作的变量范围 */
#if ((MB_RTU_SLAVE_ENABLED > (0))||(MB_TCP_SERVER_ENABLED > (0))||(MB_ASCII_SLAVE_ENABLED > (0)))

#define CoilStartAddress        0
#define CoilEndAddress          0

#define StatusStartAddress        0
#define StatusEndAddress          0

#define HoldingResterStartAddress       0
#define HoldingResterEndAddress         0

#define InputResterStartAddress       0
#define InputResterEndAddress         0

#endif

#endif
/*********** (C) COPYRIGHT 1999-2019 Moonan Technology *********END OF FILE****/