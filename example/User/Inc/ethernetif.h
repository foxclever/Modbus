/******************************************************************************/
/** 模块名称：不带操作系统LwIP以太网通讯                                     **/
/** 文件名称：ethernetif.h                                                   **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：用于实现LwIP所使用网卡操作的回调函数                           **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2018-08-01          木南              创建文件               **/
/**                                                                          **/
/******************************************************************************/ 
  

#ifndef __ETHERNETIF_H__
#define __ETHERNETIF_H__

#include "lwip/err.h"
#include "lwip/netif.h"

/* Within 'USER CODE' section, code will be kept by default at each generation */
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* Exported functions ------------------------------------------------------- */
err_t ethernetif_init(struct netif *netif);

void ethernetif_input(struct netif *netif);
void ethernetif_update_config(struct netif *netif);
void ethernetif_notify_conn_changed(struct netif *netif);


#endif

/*********** (C) COPYRIGHT 1999-2018 Moonan Technology *********END OF FILE****/
