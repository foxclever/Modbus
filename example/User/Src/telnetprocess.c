/******************************************************************************/
/** 模块名称：不带操作系统LwIP以太网通讯                                     **/
/** 文件名称：telnetprocess.c                                                **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：用于实现基于LwIP的Telnet服务器功能                             **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2018-08-01          木南              创建文件               **/
/**                                                                          **/
/******************************************************************************/ 

#include "telnetprocess.h"
#include "lwip/tcp.h"
#include <string.h>
#include <stdio.h>

#include "netportdefine.h"

#define MAX_MSG_SIZE 30         //命令的最大长度
#define LOGIN_INFO "Please input Password to login:"    //客户端登录提示信息
#define PASSWORD "111111"       //默认密码

extern unsigned int sys_now(void);
char *command[] = {"hello","date","time","version","quit","help"};
enum TELNET_STATE 
{
  TELNET_SETUP,
  TELNET_CONNECTED,
};

typedef struct 
{
  int state;
  u16_t client_port;
  u16_t bytes_len;
  char bytes[MAX_MSG_SIZE];
}telnet_conn_arg;

/* TELNET接收回调函数，客户端建立连接后，本函数被调用 */
static err_t TelnetServerAccept(void *arg, struct tcp_pcb *pcb, err_t err);
/* TELNET连接错误回调函数，连接故障时调用本函数 */
static void TelnetServeConnectError(void *arg, err_t err);
/* TELNET服务器信息处理回调函数，在有消息需要处理时，调用此函数 */
static err_t TelnetServerCallback(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err);
/* TELNET命令输入处理*/
static int TelnetCommandInput(struct tcp_pcb *pcb, telnet_conn_arg * conn_args, struct pbuf *p);
/* TELNET命令输入处理*/
static int TelnetCommandInput(struct tcp_pcb *pcb, telnet_conn_arg * conn_args, struct pbuf *p);

/* 服务器主动关闭TELNET连接*/
static void ServerCloseTelnetConnection(struct tcp_pcb *pcb)
{
  if(NULL != (void *)pcb->callback_arg)
  {
    mem_free((void *)pcb->callback_arg);
    tcp_arg(pcb, NULL);
  }

  tcp_close(pcb);
}

/* TELNET命令解析*/
static int TelnetCommandParse(struct tcp_pcb *pcb, char *req)
{
  char res_buffer[100] = {0,};
  int strlen = 0;
  int close_flag = 0;
	
  if(strcmp(req, command[0]) == 0)              //hello命令
  {
    strlen = sprintf(res_buffer, "Hello! This is an LwIP-based Telnet Server..\r\n");
  }
  else if(strcmp(req, command[1]) == 0)         //date命令
  {
    strlen = sprintf(res_buffer, "The current date is August 1, 2018..\r\n");
  }
  else if(strcmp(req, command[2]) == 0)         //time命令
  {
    strlen = sprintf(res_buffer, "The current system time is [%u]..\r\n", sys_now());
  }
  else if(strcmp(req, command[3]) == 0)         //version命令
  {
    strlen = sprintf(res_buffer, "The current version is V1.0.1..\r\n");
  }
  else if(strcmp(req, command[4]) == 0)         //quit命令
  {
    strlen = sprintf(res_buffer, "The Connection will shutdown..\r\n");
    close_flag = 1;
  }
  else if(strcmp(req, command[5]) == 0)         //help命令
  {
    strlen = sprintf(res_buffer,"Suppprted Command:date  hello  systick  version  help  quit..\r\n");
  }
  else          //未定义命令
  {
    strlen = sprintf(res_buffer, "This command is not supported by the system..\r\n");
  }

  /* 对命令进行回复，并输出提示符*/
  tcp_write(pcb, res_buffer, strlen, TCP_WRITE_FLAG_COPY);
  strlen = sprintf(res_buffer, "LwIP Telnet>");
  tcp_write(pcb, res_buffer, strlen, TCP_WRITE_FLAG_COPY);

  return close_flag;
}

/* TELNET命令输入处理*/
static int TelnetCommandInput(struct tcp_pcb *pcb, telnet_conn_arg * conn_args, struct pbuf *p)
{
  int strlen = 0;
  char buf[20];
  u16_t len=p->len;
  u8_t * datab = (unsigned char *)p->payload;

  if((len == 2) && (*datab == 0x0d) && (*(datab+1) == 0x0a))    //接收到回车符，为完整命令
  {	
    conn_args->bytes[conn_args->bytes_len] = 0x00;
    return 1;
  }
  else if((len == 1) && (*datab >= 0x20) && (*datab <= 0x7e))   //收到普通字符
  {
    conn_args->bytes[conn_args->bytes_len] = *datab;
    
    if(conn_args->bytes_len < (MAX_MSG_SIZE-1))
    {
      conn_args->bytes_len++;
    }
		
  }
  else if((len == 1) && (*datab == 0x08) && (conn_args->bytes_len > 0)) //收到退格符，且前面有输入的字符
  {
    conn_args->bytes_len--;
    strlen = sprintf(buf," \b \b");
    tcp_write(pcb, buf, strlen, TCP_WRITE_FLAG_COPY);
  }
  else if((len == 1) && (*datab == 0x08))       //收到退格符，且前面无输入的字符
  {
    conn_args->bytes_len = 0;
    strlen = sprintf(buf,">");
    tcp_write(pcb, buf, strlen, TCP_WRITE_FLAG_COPY);
  }
  return 0;
}

/* TELNET服务器信息处理回调函数，在有消息需要处理时，调用此函数 */
static err_t TelnetServerCallback(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
  telnet_conn_arg *conn_args = (telnet_conn_arg *)arg;
  char sndbuf[50];
  int strlen = 0;
  int ret = 0;
  
  if(NULL == conn_args || pcb->remote_port != conn_args->client_port)
  {
    if(p!= NULL)
    {
      pbuf_free(p);
    }
    return ERR_ARG;
  }
  
  if (p != NULL) 
  {        
    /* 更新接收窗口 */
    tcp_recved(pcb, p->tot_len);

    ret = TelnetCommandInput(pcb, conn_args, p);

    if(ret == 1)//是完整命令
    {
      switch(conn_args->state)
      {
      case TELNET_SETUP:
        {
          if(strcmp(conn_args->bytes,PASSWORD) == 0)//密码正确
          {
            strlen = sprintf(sndbuf,"##Hello! This is an LwIP-based Telnet Server##\r\n");
            tcp_write(pcb, sndbuf, strlen,TCP_WRITE_FLAG_COPY); 
            strlen = sprintf(sndbuf,"##Created by Moonan...                      ##\r\n");
            tcp_write(pcb, sndbuf, strlen,TCP_WRITE_FLAG_COPY);
            strlen = sprintf(sndbuf,"##Enter help for help.  Enter quit for quit.##\r\n");
            tcp_write(pcb, sndbuf, strlen,TCP_WRITE_FLAG_COPY);
            strlen = sprintf(sndbuf,"LwIP Telnet>");
            tcp_write(pcb,sndbuf,strlen, 1);

            conn_args->state = TELNET_CONNECTED;//转换状态
          }
          else//密码错误，提示重新登录
          {
            strlen = sprintf(sndbuf,"##PASSWORD ERROR! Try again:##\r\n");
            tcp_write(pcb, sndbuf, strlen,TCP_WRITE_FLAG_COPY);
          }
          memset(conn_args->bytes, 0, MAX_MSG_SIZE);
          conn_args->bytes_len = 0;
          break;
        }
      case TELNET_CONNECTED:
        {
          if(TelnetCommandParse(pcb, conn_args->bytes) == 0)
          {
            memset(conn_args->bytes, 0, MAX_MSG_SIZE);
            conn_args->bytes_len = 0;
          }
          else
          {
            /* 服务器关闭连接 */
            ServerCloseTelnetConnection(pcb);
          }
          break;
        }
      default:
        {
          break;
        }
      }
    }
    pbuf_free(p);
  }  
  else if (err == ERR_OK) 
  {
    /* 服务器关闭连接 */
    ServerCloseTelnetConnection(pcb);
  }
  
  return ERR_OK;

}

/* TELNET连接错误回调函数，连接故障时调用本函数 */
static void TelnetServeConnectError(void *arg, err_t err)
{
  Telnet_Server_Initialization();
}

/* TELNET接收回调函数，客户端建立连接后，本函数被调用 */
static err_t TelnetServerAccept(void *arg, struct tcp_pcb *pcb, err_t err)
{     
  u32_t remote_ip;
  char linkInfo [100];
  u8_t iptab[4];
  telnet_conn_arg *conn_arg = NULL;
  remote_ip = pcb->remote_ip.addr;

  iptab[0] = (u8_t)(remote_ip >> 24);
  iptab[1] = (u8_t)(remote_ip >> 16);
  iptab[2] = (u8_t)(remote_ip >> 8);
  iptab[3] = (u8_t)(remote_ip);

  //生成登录提示信息
  sprintf(linkInfo, "Welcome to Telnet! your IP:Port --> [%d.%d.%d.%d:%d]\r\n", \
  	              iptab[3], iptab[2], iptab[1], iptab[0], pcb->remote_port);	

  conn_arg = mem_calloc(sizeof(telnet_conn_arg), 1);
  if(!conn_arg)
  {
    return ERR_MEM;
  }

  conn_arg->state = TELNET_SETUP;
  conn_arg->client_port = pcb->remote_port;
  conn_arg->bytes_len = 0;
  memset(conn_arg->bytes, 0, MAX_MSG_SIZE);
  
  tcp_arg(pcb, conn_arg);
  
  /* 注册Telnet服务器连接错误回调函数 */
  tcp_err(pcb, TelnetServeConnectError);
  /* 注册Telnet服务器消息处理回调函数*/
  tcp_recv(pcb, TelnetServerCallback);
  
  /* 连接成功，发送登录提示信息 */  
  tcp_write(pcb, linkInfo, strlen(linkInfo), 1);
  tcp_write(pcb, LOGIN_INFO, strlen(LOGIN_INFO), 1); 
  
  return ERR_OK;
}

/* TELNET服务器初始化配置*/
void Telnet_Server_Initialization(void)
{
  struct tcp_pcb *pcb;	            		
  
  /* 生成一个新的TCP控制块 */
  pcb = tcp_new();	                		 	

  /* 控制块邦定到本地IP和对应端口 */
  tcp_bind(pcb, IP_ADDR_ANY, TCP_TELNET_SERVER_PORT);       


  /* 服务器进入侦听状态 */
  pcb = tcp_listen(pcb);				

  /* 注册服务器accept回调函数 */	
  tcp_accept(pcb, TelnetServerAccept); 						
}

/*********** (C) COPYRIGHT 1999-2018 Moonan Technology *********END OF FILE****/