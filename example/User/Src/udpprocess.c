/******************************************************************************/
/** 模块名称：不带操作系统LwIP以太网通讯                                     **/
/** 文件名称：udpprocess.c                                                   **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：用于实现基于LwIP的UDP服务器和客户端功能                        **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2018-08-01          木南              创建文件               **/
/**                                                                          **/
/******************************************************************************/ 

#include "lwip/netif.h"
#include "lwip/ip.h"
#include "lwip/tcp.h"
#include "lwip/init.h"
#include "netif/etharp.h"
#include "lwip/udp.h"
#include "lwip/pbuf.h"
#include "string.h"
#include "udpprocess.h"

#include "netportdefine.h"

uint8_t udpServerIP[4]={192,168,0,1};

/* 定义UDP服务器数据处理回调函数 */
static void UDPServerCallback(void *arg,struct udp_pcb *upcb,struct pbuf *p,const ip_addr_t *addr,u16_t port);

/* 定义UDP客户端数据处理回调函数 */
static void UDPClientCallback(void *arg,struct udp_pcb *upcb,struct pbuf *p,const ip_addr_t *addr,u16_t port);

/* 客户端数据发送函数 */
void UdpClientSendPacket(struct udp_pcb *upcb,char* data);


/* 定义UDP服务器数据处理回调函数 */
static void UDPServerCallback(void *arg,struct udp_pcb *upcb,struct pbuf *revBuf,const ip_addr_t *addr,u16_t port)
{
  struct pbuf *sendBuf = NULL;
  const char* reply = "This is reply!\n";

  pbuf_free(revBuf);
   
  sendBuf = pbuf_alloc(PBUF_TRANSPORT, strlen(reply)+1, PBUF_RAM);
  if(!sendBuf)
  {
    return;
  }

  memset(sendBuf->payload,0,sendBuf->len);
  memcpy(sendBuf->payload, reply, strlen(reply));
  udp_sendto(upcb, sendBuf, addr, port);
  pbuf_free(sendBuf);
}

/* UDP初始化配置 */
void UDP_Server_Initialization(void)
{
  static char * recv_arg="We recieved a UDP data\n";
  struct udp_pcb *upcb;

  /* 生成一个新的UDP控制块 */
  upcb = udp_new();
   
  /* 绑定upcb块到任意IP地址及指定端口*/
  udp_bind(upcb, IP_ADDR_ANY, UDP_ECHO_SERVER_PORT);
 
  /* 为upcb指定数据处理回调函数 */
  //udp_recv(upcb, UDPServerCallback, NULL);
  udp_recv(upcb,UDPServerCallback,(void *)recv_arg);
}

/* UDP客户端初始化配置 */
void UDP_Client_Initialization(void)
{
  ip_addr_t DestIPaddr;
  err_t err;
  struct udp_pcb *upcb;
  char data[]="This is a Client.";

  /* 设置服务器端的IP地址 */
  IP4_ADDR( &DestIPaddr,udpServerIP[0],udpServerIP[1],udpServerIP[2],udpServerIP[3]);

  /* 创建一个新的UDP控制块 */
  upcb = udp_new();

  if (upcb!=NULL)
  {
    /* 服务器端地址、端口配置 */
    err= udp_connect(upcb, &DestIPaddr, UDP_ECHO_SERVER_PORT);

    if (err == ERR_OK)
    {
      /* 注册回调函数 */
      udp_recv(upcb, UDPClientCallback,(void *)data);
      /**数据发送，第一次连接时客户端发送数据至服务器端，发送函数中会遍历查找源IP地址的配置，如果源IP地址未配置，则数据发送失败。该处出现的问题在后面总结中提到了**/
      UdpClientSendPacket(upcb,data);   
    }
  }
}

/* 定义UDP客户端数据处理回调函数 */
static void UDPClientCallback(void *arg,struct udp_pcb *upcb,struct pbuf *p,const ip_addr_t *addr,u16_t port)
{
  udp_send(upcb, p);     //数据回显 

  pbuf_free(p);
}

/* 客户端数据发送函数 */
void UdpClientSendPacket(struct udp_pcb *upcb,char* data)
{
  struct pbuf *p;

  /* 分配内存空间 */
  p = pbuf_alloc(PBUF_TRANSPORT,strlen((char*)data), PBUF_POOL);

  if (p != NULL)
  {

    /* 复制数据到pbuf */
    pbuf_take(p, (char*)data, strlen((char*)data));

    /* 发送数据 */
    udp_send(upcb, p);     //发送数据

    /* 释放pbuf */
    pbuf_free(p);
  }

}


/*********** (C) COPYRIGHT 1999-2018 Moonan Technology *********END OF FILE****/