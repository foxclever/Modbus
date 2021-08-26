/******************************************************************************/
/** 模块名称：不带操作系统LwIP以太网通讯                                     **/
/** 文件名称：main.h                                                         **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：用于实现LwIP以太网通讯主控程序                                 **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2018-08-01          木南              创建文件               **/
/**                                                                          **/
/******************************************************************************/ 

#ifndef __MAIN_H__
#define __MAIN_H__

#include "stm32f4xx_hal.h"
#include "lwipprocess.h"
#include "noos_config.h"

#if (UDP_SERVER_ENABLE>(0)||UDP_CLIENT_ENABLE>(0))
#include "udpprocess.h"
#endif

#if TFTP_SERVER_ENABLE>(0)
#include "tftpprocess.h"
#endif

#if (TCP_SERVER_ENABLE>(0)||TCP_CLIENT_ENABLE>(0))
#include "tcpprocess.h"
#endif

#if HTTP_SERVER_ENABLE>(0)
#include "httpprocess.h"
#endif

#if TELNET_SERVER_ENABLE>(0)
#include "telnetprocess.h"
#endif

#include "usart1process.h"

#include "usart3process.h"

/* 系统时钟初始化配置*/
void SystemClock_Config(void);

/* GPIO初始化配置 */
 void GPIO_Init_Configuration(void);



#endif /* __MAIN_H__ */

/*********** (C) COPYRIGHT 1999-2018 Moonan Technology *********END OF FILE****/
