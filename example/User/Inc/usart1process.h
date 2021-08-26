/******************************************************************************/
/** 模块名称：Modbus本地从站实例模块                                         **/
/** 文件名称：mcudsprocess.h                                                 **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：Modbus本地从站实例模块上位显示串行接口软件，实现对上位机的通   **/
/**           讯。基于USART1端口，采用RS232，实现Modbus RTU从站。            **/
/**           PA9       USART1_TX       USART1串行发送                       **/
/**           PA10      USART1_RX       USART1串行接收                       **/
/**           PD8       RS232_INT       RS232中断信号                        **/
/**           基于STM32F407VGT6硬件平台，软件库采用HAL FW_F4 V1.26.0库       **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2021-03-29          木南              创建文件               **/
/**                                                                          **/
/******************************************************************************/ 

#ifndef __MCUDS_PROCESS_H
#define __MCUDS_PROCESS_H

#include "noos_config.h"
#include "mbconfig.h"

#ifdef __cplusplus
extern "C" {
#endif


/*设备对外通讯数据处理*/
void LocalSlaveProcess(void);

/*设备对外通讯数据处理*/
void LocalSlaveConfiguration(void);

#ifdef __cplusplus
}
#endif

#endif  /* __MCUDS_PROCESS_H */

/*********** (C) COPYRIGHT 1999-2021 Moonan Technology *********END OF FILE****/
