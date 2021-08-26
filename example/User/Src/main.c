/******************************************************************************/
/** 模块名称：不带操作系统LwIP以太网通讯                                     **/
/** 文件名称：main.c                                                         **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：用于实现LwIP以太网通讯主控程序                                 **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2018-08-01          木南              创建文件               **/
/**                                                                          **/
/******************************************************************************/ 

#include "main.h"

/* 不带操作系统LwIP以太网通讯主控程序 */
int main(void)
{
  /* 复位全部外设，初始化Flash接口及嘀嗒定时器 */
  HAL_Init();

  /* 配置系统时钟 */
  SystemClock_Config();

  /* 初始化外设配置 */
  GPIO_Init_Configuration();

  /* 初始化网络配置 */
  LWIP_Init_Configuration();

#if UDP_SERVER_ENABLE > (0)
  /* UDP服务器初始化配置 */
  UDP_Server_Initialization();
#endif

#if UDP_CLIENT_ENABLE > (0)
  /* UDP客户端初始化配置 */
  UDP_Client_Initialization();
#endif
  
#if TFTP_SERVER_ENABLE > (0)
  /* 初始化TFTP服务器 */
  Tftp_Server_Initialization();
#endif
  
#if TCP_SERVER_ENABLE > (0)
  /* TCP服务器初始化配置 */
  Tcp_Server_Initialization();
#endif

#if TCP_CLIENT_ENABLE > (0)
  /* TCP客户端初始化配置 */
  Tcp_Client_Initialization();
#endif
  
#if HTTP_SERVER_ENABLE > (0)
  /* 初始化HTTP服务器 */
  Http_Server_Initialization();
#endif
  
#if HTTP_CLIENT_ENABLE > (0)
  /* 初始化HTTP客户端 */
  Http_Client_Initialization();
#endif
  
#if TELNET_SERVER_ENABLE > (0)
  /* 初始化TELNET服务器 */
  Telnet_Server_Initialization();
#endif
  
  
  /*设备对外通讯数据处理*/
    LocalSlaveConfiguration();
    
    /*对本地主机端口配置*/
    LocalMasterConfiguration();
  
  while (1)
  {
    /*设备对外通讯数据处理*/
    LocalSlaveProcess();
    
    /*设备本地主机通讯处理*/
    LocalMasterProcess();
    
  /* 网络处理 */
    EthernetProcess();
  }

}

/*********** (C) COPYRIGHT 1999-2018 Moonan Technology *********END OF FILE****/
