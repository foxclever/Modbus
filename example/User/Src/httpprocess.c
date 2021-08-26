/******************************************************************************/
/** 模块名称：不带操作系统LwIP以太网通讯                                     **/
/** 文件名称：httpprocess.c                                                  **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：用于实现基于LwIP的HTTP服务器和客户端功能                       **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2018-08-01          木南              创建文件               **/
/**                                                                          **/
/******************************************************************************/ 

#include "httpprocess.h"
#include "lwip/netif.h"
#include "lwip/ip.h"
#include "lwip/tcp.h"
#include "lwip/init.h"
#include "netif/etharp.h"
#include "lwip/udp.h"
#include "lwip/pbuf.h"
#include <stdio.h>	
#include <string.h>
#include "netportdefine.h"

#define TCP_HTTP_CLIENT_PORT TCP_HTTP_SERVER_PORT

uint8_t httpServerIP[4]={192,168,0,1};

const unsigned char htmlMessage[] = "	\
        <html>	\
        <head><title> A LwIP WebServer !!</title></head> \
            <center><p>This is a WebServer for testing!</center>\
	    <center><p>The WebServer based on LwIP v2.0.3!</center>\
	    </html>";

const unsigned char strHttpGet[]="GET https://www.cnblogs.com/foxclever/ HTTP/1.1\r\n"
                                    "Host:www.cnblogs.com:80\r\n\r\n";
              
              
/* HTTP接收回调函数，客户端建立连接后，本函数被调用 */
static err_t HttpServerAccept(void *arg, struct tcp_pcb *pcb, err_t err);
/* HTTP服务器信息处理回调函数 */
static err_t HttpServerCallback(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err);

/* HTTP客户端连接到服务器回调函数 */
static err_t HTTPClientConnected(void *arg, struct tcp_pcb *pcb, err_t err);

/* HTTP客户端接收到数据后的数据处理回调函数 */
static err_t HTTPClientCallback(void *arg, struct tcp_pcb *pcb, struct pbuf *tcp_recv_pbuf, err_t err);

/* HTTP客户端连接服务器错误回调函数 */
static void HTTPClientConnectError(void *arg, err_t err);


/* HTTP服务器信息处理回调函数 */
static err_t HttpServerCallback(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
  char *data = NULL;

  if (p != NULL) 
  {        
    /* 更新接收窗口 */
    tcp_recved(pcb, p->tot_len);
    data =  p->payload;
    
    /* 如果是http请求，返回html信息，否则无响应 */
    if(p->len >=3 && data[0] == 'G'&& data[1] == 'E'&& data[2] == 'T')
    {
      tcp_write(pcb, htmlMessage, sizeof(htmlMessage), 1);
    }
    else
    {

    }
    pbuf_free(p);
    tcp_close(pcb);
  } 
  else if (err == ERR_OK) 
  {
    return tcp_close(pcb);
  }
  return ERR_OK;
}

/* HTTP接收回调函数，客户端建立连接后，本函数被调用 */
static err_t HttpServerAccept(void *arg, struct tcp_pcb *pcb, err_t err)
{
  /*注册HTTP服务器回调函数*/
  tcp_recv(pcb, HttpServerCallback);
  
  return ERR_OK;
}

/* HTTP服务器初始化配置*/
 void Http_Server_Initialization(void)
{
  struct tcp_pcb *pcb = NULL;	            		
  
  /* 生成一个新的TCP控制块 */
  pcb = tcp_new();	                		 	

  /* 控制块邦定到本地IP和对应端口 */
  tcp_bind(pcb, IP_ADDR_ANY, TCP_HTTP_SERVER_PORT);       

  /* 服务器进入侦听状态 */
  pcb = tcp_listen(pcb);				

  /* 注册服务器accept回调函数 */	
  tcp_accept(pcb, HttpServerAccept);   
										
}

/* HTTP客户端初始化配置*/
 void Http_Client_Initialization(void)
{
  struct tcp_pcb *tcp_client_pcb;
  ip_addr_t ipaddr;

  /* 将目标服务器的IP写入一个结构体，为pc机本地连接IP地址 */
  IP4_ADDR(&ipaddr,httpServerIP[0],httpServerIP[1],httpServerIP[2],httpServerIP[3]);

  /* 为tcp客户端分配一个tcp_pcb结构体    */
  tcp_client_pcb = tcp_new();

  /* 绑定本地端号和IP地址 */
  tcp_bind(tcp_client_pcb, IP_ADDR_ANY, TCP_HTTP_CLIENT_PORT);

  if (tcp_client_pcb != NULL)
  {
    /* 与目标服务器进行连接，参数包括了目标端口和目标IP */
    tcp_connect(tcp_client_pcb, &ipaddr, TCP_HTTP_SERVER_PORT, HTTPClientConnected);
    
    tcp_err(tcp_client_pcb, HTTPClientConnectError);
  }
}


/* HTTP客户端连接到服务器回调函数 */
static err_t HTTPClientConnected(void *arg, struct tcp_pcb *pcb, err_t err)
{
  char clientString[]="GET https://www.cnblogs.com/foxclever/ HTTP/1.1\r\n"
                                    "Host:www.cnblogs.com:80\r\n\r\n";

  /* 配置接收回调函数 */
  tcp_recv(pcb, HTTPClientCallback);

  /* 发送一个建立连接的问候字符串*/
  tcp_write(pcb,clientString, strlen(clientString),0);

  return ERR_OK;
}

/* HTTP客户端接收到数据后的数据处理回调函数 */
static err_t HTTPClientCallback(void *arg, struct tcp_pcb *pcb, struct pbuf *tcp_recv_pbuf, err_t err)
{
  struct pbuf *tcp_send_pbuf;
  char echoString[]="GET https://www.cnblogs.com/foxclever/ HTTP/1.1\r\n"
                                    "Host:www.cnblogs.com:80\r\n\r\n";

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
    Http_Client_Initialization();

    return ERR_OK;
  }

  return ERR_OK;
}

/* HTTP客户端连接服务器错误回调函数 */
static void HTTPClientConnectError(void *arg, err_t err)
{
  /* 重新启动连接 */
  Http_Client_Initialization();
}


/*********** (C) COPYRIGHT 1999-2018 Moonan Technology *********END OF FILE****/