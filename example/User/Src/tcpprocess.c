/******************************************************************************/
/** 模块名称：不带操作系统LwIP以太网通讯                                     **/
/** 文件名称：tcpprocess.c                                                   **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：用于实现基于LwIP的TCP服务器和客户端功能                        **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2018-08-01          木南              创建文件               **/
/**                                                                          **/
/******************************************************************************/ 

#include "tcpprocess.h"
#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/tcp.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "lwip/ip_addr.h"
#include "netportdefine.h"

#include "mbconfig.h"

#define TCP_SERVER_PORT TCP_ECHO_SERVER_PORT
#define TCP_CLIENT_PORT TCP_ECHO_SERVER_PORT

uint8_t serverIP[4]={192,168,1,31};

/* TCP服务器接收回调函数 */
static err_t TCPServerAccept(void *arg, struct tcp_pcb *pcb, err_t err);

/* TCP服务器数据处理服务器回调函数 */
static err_t TCPServerCallback(void *arg, struct tcp_pcb *pcb, struct pbuf *tcp_recv_pbuf, err_t err);

/* TCP客户端连接到服务器回调函数 */
static err_t TCPClientConnected(void *arg, struct tcp_pcb *pcb, err_t err);

/* TCP客户端接收到数据后的数据处理回调函数 */
static err_t TCPClientCallback(void *arg, struct tcp_pcb *pcb, struct pbuf *tcp_recv_pbuf, err_t err);

/* TCP客户端连接服务器错误回调函数 */
static void TCPClientConnectError(void *arg, err_t err);

/* TCP服务器初始化 */
void Tcp_Server_Initialization(void)
{
  struct tcp_pcb *tcp_server_pcb;

  /* 为tcp服务器分配一个tcp_pcb结构体 */
  tcp_server_pcb = tcp_new();

  /* 绑定本地端号和IP地址 */
  tcp_bind(tcp_server_pcb, IP_ADDR_ANY, TCP_SERVER_PORT);

  /* 监听之前创建的结构体tcp_server_pcb */
  tcp_server_pcb = tcp_listen(tcp_server_pcb);

  /* 初始化结构体接收回调函数 */
  tcp_accept(tcp_server_pcb, TCPServerAccept);
}

/* TCP服务器接收回调函数，当客户端建立连接后本函数被调用 */
static err_t TCPServerAccept(void *arg, struct tcp_pcb *pcb, err_t err)
{
  /* 注册接收回调函数 */
  tcp_recv(pcb, TCPServerCallback);

  return ERR_OK;
}

/* TCP服务器数据处理服务器回调函数 */
static err_t TCPServerCallback(void *arg, struct tcp_pcb *pcb, struct pbuf *tcp_recv_pbuf, err_t err)
{
  //struct pbuf *tcp_send_pbuf;
  //char echoString[]="This is the client content echo:\r\n";
    
    struct pbuf tcp_send_pbuf;
    uint8_t respondBuffer[100];
    //tcp_send_pbuf.next=NULL;
    tcp_send_pbuf.payload=respondBuffer;

  if (tcp_recv_pbuf != NULL)
  {
    /* 更新接收窗口 */
    tcp_recved(pcb, tcp_recv_pbuf->tot_len);

    /* 将接收的数据拷贝给发送结构体 */
//    tcp_send_pbuf = tcp_recv_pbuf;
//    tcp_write(pcb,echoString, strlen(echoString), 1);
    tcp_send_pbuf.len=ParsingClientAccessCommand(tcp_recv_pbuf->payload,tcp_send_pbuf.payload);
    /* 将接收到的数据再转发出去 */
    tcp_write(pcb,&tcp_send_pbuf.payload, tcp_send_pbuf.len, 1);

    pbuf_free(tcp_recv_pbuf);
    tcp_close(pcb);
  }
  else if (err == ERR_OK)
  {
    return tcp_close(pcb);
  }

  return ERR_OK;
}

/* TCP客户端初始化 */
void Tcp_Client_Initialization(void)
{
  struct tcp_pcb *tcp_client_pcb;
  ip_addr_t ipaddr;

  /* 将目标服务器的IP写入一个结构体，为pc机本地连接IP地址 */
  IP4_ADDR(&ipaddr,serverIP[0],serverIP[1],serverIP[2],serverIP[3]);

  /* 为tcp客户端分配一个tcp_pcb结构体    */
  tcp_client_pcb = tcp_new();

  /* 绑定本地端号和IP地址 */
  tcp_bind(tcp_client_pcb, IP_ADDR_ANY, TCP_CLIENT_PORT);

  if (tcp_client_pcb != NULL)
  {
    /* 与目标服务器进行连接，参数包括了目标端口和目标IP */
    tcp_connect(tcp_client_pcb, &ipaddr, TCP_SERVER_PORT, TCPClientConnected);
    
    tcp_err(tcp_client_pcb, TCPClientConnectError);
  }
}

/* TCP客户端连接到服务器回调函数 */
static err_t TCPClientConnected(void *arg, struct tcp_pcb *pcb, err_t err)
{
  char clientString[]="This is a new client connection.\r\n";

  /* 配置接收回调函数 */
  tcp_recv(pcb, TCPClientCallback);

  /* 发送一个建立连接的问候字符串*/
  tcp_write(pcb,clientString, strlen(clientString),0);

  return ERR_OK;
}

/* TCP客户端接收到数据后的数据处理回调函数 */
static err_t TCPClientCallback(void *arg, struct tcp_pcb *pcb, struct pbuf *tcp_recv_pbuf, err_t err)
{
  struct pbuf *tcp_send_pbuf;
  char echoString[]="This is the server content echo:\r\n";

  if (tcp_recv_pbuf != NULL)
  {
    /* 更新接收窗口 */
    tcp_recved(pcb, tcp_recv_pbuf->tot_len);

    /* 将接收到的服务器内容回显*/
    tcp_write(pcb,echoString, strlen(echoString), 1);
    tcp_send_pbuf = tcp_recv_pbuf;
    tcp_write(pcb, tcp_send_pbuf->payload, tcp_send_pbuf->len, 1);

    pbuf_free(tcp_recv_pbuf);
  }
  else if (err == ERR_OK)
  {
    tcp_close(pcb);
    Tcp_Client_Initialization();

    return ERR_OK;
  }

  return ERR_OK;
}

/* TCP客户端连接服务器错误回调函数 */
static void TCPClientConnectError(void *arg, err_t err)
{
  /* 重新启动连接 */
  Tcp_Client_Initialization();
}

/*********** (C) COPYRIGHT 1999-2018 Moonan Technology *********END OF FILE****/
