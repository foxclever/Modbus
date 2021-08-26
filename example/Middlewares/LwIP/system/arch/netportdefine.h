/******************************************************************************/
/** 模块名称：以太网通讯公用模块                                             **/
/** 文件名称：netportdefine.h                                                **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：用于TCP/IP协议族中，各个网络端口的宏                           **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2018-08-01          木南              创建文件               **/
/**                                                                          **/
/******************************************************************************/ 

#ifndef __NET_PORT_DEFINE_H
#define __NET_PORT_DEFINE_H

#define TCP_ECHO_SERVER_PORT            7       //TCP ECHO（回显）协议端口
#define UDP_ECHO_SERVER_PORT            7       //UDP ECHO（回显）协议端口

#define TCP_FTP_SERVER_DATA_PORT        20      //文件传输协议 - 默认数据端口
#define UDP_FTP_SERVER_DATA_PORT        20      //文件传输协议 - 默认数据端口
#define TCP_FTP_SERVER_CTRL_PORT	21	//文件传输协议 - 控制端口
#define TCP_FTP_SERVER_CTRL_PORT	21	//文件传输协议 - 控制端口

#define	TCP_TELNET_SERVER_PORT          23      //Telnet 终端仿真协议 - 未加密文本通信
#define	UDP_TELNET_SERVER_PORT          23	//Telnet 终端仿真协议 - 未加密文本通信

#define	TCP_DNS_SERVER_PORT             53	//DNS（域名服务系统）
#define	UDP_DNS_SERVER_PORT             53	//DNS（域名服务系统）

#define	UDP_TFTP_SERVER_PORT            69	//TFTP（小型文件传输协议）

#define	TCP_HTTP_SERVER_PORT            80	//HTTP（超文本传输协议）- 用于传输网页

#define	TCP_REMOTE_TELNET_SERVER_PORT   107	//远程Telnet协议

#define	UDP_NTP_SERVER_PORT             123	//NTP (网络时间协议) - 用于时间同步

#define	TCP_SNMP_SERVER_PORT            161	//SNMP (简单网络管理协议)
#define	UDP_SNMP_SERVER_PORT            161	//SNMP (简单网络管理协议)

#define	TCP_MODBUS_SERVER_PORT          502	//Modbus协议
#define	UDP_MODBUS_SERVER_PORT          502	//Modbus协议

#endif
/*********** (C) COPYRIGHT 1999-2018 Moonan Technology *********END OF FILE****/