/******************************************************************************/
/** 模块名称：Modbus本地主机实例模块                                         **/
/** 文件名称：mcutsprocess.c                                                 **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：Modbus本地主机实例模块测试端口串行接口软件，实现对下位机的通   **/
/**           讯。基于USART3端口，采用RS485，实现Modbus RTU主站。            **/
/**           PB10      USART3_TX       USART3串行发送                       **/
/**           PB11      USART3_RX       USART3串行接收                       **/
/**           PB7       RS485_CTL3      RS485收发控制                        **/
/**           基于STM32F407ZGT6硬件平台，软件库采用HAL FW_F4 V1.26.0库       **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2021-03-29          尹家军            创建文件               **/
/**                                                                          **/
/******************************************************************************/ 

#ifndef __MCUTS_PROCESS_H
#define __MCUTS_PROCESS_H

#include "noos_config.h"
#include "mbconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

/*设备本地主机通讯处理*/
void LocalMasterProcess(void);

/*对本地主机端口配置*/
void LocalMasterConfiguration(void);

#ifdef __cplusplus
}
#endif

#endif  /* __MCUTS_PROCESS_H */

/*********** (C) COPYRIGHT 1999-2021 Moonan Technology *********END OF FILE****/
