/******************************************************************************/
/** 模块名称：不带操作系统LwIP以太网通讯                                     **/
/** 文件名称：tftpprocess.c                                                  **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：用于实现基于LwIP的TFTP服务器功能                               **/

/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2018-08-01          木南              创建文件               **/
/**                                                                          **/
/******************************************************************************/ 

#include "tftpprocess.h"
#include "netportdefine.h"

/* TFTP错误码字符串 */
char *tftp_errorcode_string[] = {
                                  "not defined",
                                  "file not found",
                                  "access violation",
                                  "disk full",
                                  "illegal operation",
                                  "unknown transfer id",
                                  "file already exists",
                                  "no such user",
                                };

/* 发送TFTP消息 */
static err_t SendTftpMessage(struct udp_pcb *upcb,const ip_addr_t *to_ip, int to_port, char *buf, int buflen);
/* 使用获得的错误信息作为错误码在buf中构造一个错误信息 */
static int ConstructTftpErrorMessage(char *buf, tftp_errorcode err);
/* 构造并且传送数据包 */
static int SendTftpDataPacket(struct udp_pcb *upcb, const ip_addr_t *to, int to_port, int block,char *buf, int buflen);
/* 构造并向客户端发送一条错误消息 */
static int SendTftpErrorMessage(struct udp_pcb *upcb, const ip_addr_t *to, int to_port, tftp_errorcode err);
/* 从每一个来自addr:port的新请求创建一个新的端口来服务响应，并启动响应过程 */
static void ProcessTftpRequest(struct pbuf *pkt_buf, const ip_addr_t *addr, u16_t port);
/* TFTP服务器回调函数 */
static void TftpServerCallback(void *arg, struct udp_pcb *upcb, struct pbuf *p,const ip_addr_t *addr, u16_t port);


/* 发送TFTP消息 */
static err_t SendTftpMessage(struct udp_pcb *upcb,const ip_addr_t *to_ip, int to_port, char *buf, int buflen)
{
  err_t err;
  struct pbuf *pkt_buf = NULL; /* 将被发送的pbuf链 */

  /* 指定传输层 */
  pkt_buf = pbuf_alloc(PBUF_TRANSPORT, buflen, PBUF_RAM);

  if (!pkt_buf)
    return ERR_MEM;

  memcpy(pkt_buf->payload, buf, buflen);

  /* Sending packet by UDP protocol */
  err = udp_send(upcb, pkt_buf);

  /* free the buffer pbuf */
  pbuf_free(pkt_buf);

  return err;
}


/* 使用获得的错误信息作为错误码在buf中构造一个错误信息 */
static int ConstructTftpErrorMessage(char *buf, tftp_errorcode err)
{
  int errorlen;
  /* 在开头的2个字节设置操作码 */
  SetTftpOpCode(buf, TFTP_ERROR);
  /* 在紧接的2个字节设置错误码 */
  SetTftpErrorCode(buf, err);
  /* 在最后几个字节设置错误信息 */
  SetTftpErrorMessage(buf, tftp_errorcode_string[err]);
  /* 设置错误消息的长度 */
  errorlen = strlen(tftp_errorcode_string[err]);

  /* 返回消息长度 */
  return (4 + errorlen + 1);
}

/* 构造并向客户端发送一条错误消息 */
static int SendTftpErrorMessage(struct udp_pcb *upcb, const ip_addr_t *to, int to_port, tftp_errorcode err)
{
  char buf[512];
  int error_len;

  error_len = ConstructTftpErrorMessage(buf, err);

  return SendTftpMessage(upcb, to, to_port, buf, error_len);
}

/* 构造并且传送数据包 */
static int SendTftpDataPacket(struct udp_pcb *upcb, const ip_addr_t *to, int to_port, int block,char *buf, int buflen)
{
  /* 将开始的2个字节设置为功能码 */
  SetTftpOpCode(buf, TFTP_DATA);
  
  /* 将后续2个字节设置为块号 */
  SetTftpBlockNumber(buf, block);
  
  /* 在后续设置n各字节的数据 */

  /* 发送数据包 */
  return SendTftpMessage(upcb, to, to_port, buf, buflen + 4);
}

/*构造并发送确认包*/
int SendTftpAckPacket(struct udp_pcb *upcb,const ip_addr_t *to, int to_port, int block)
{
  /* 创建一个TFTP ACK包 */
  char packet[TFTP_ACK_PKT_LEN];

  /* 将开始的2个字节设置为功能码 */
  SetTftpOpCode(packet, TFTP_ACK);

  /* 制定ACK的块号 */
  SetTftpBlockNumber(packet, block);

  return SendTftpMessage(upcb, to, to_port, packet, TFTP_ACK_PKT_LEN);
}

/* 关闭文件传送、断开连接并删除控制块 */
void CleanTftpConnection(struct udp_pcb *upcb, tftp_connection_args *args)
{
  /* 释放args结构体 */
  mem_free(args);

  /* 断开控制块连接 */
  udp_disconnect(upcb);

  /* 删除控制块 */
  udp_remove(upcb);

}

/* 发送下一个块 */
void SendNextBlock(struct udp_pcb *upcb, tftp_connection_args *args,const ip_addr_t *to_ip, u16_t to_port)
{
  /* 从文件中读取512字节，并将其放入到"args->data" */
  int total_block = args->tot_bytes/TFTP_DATA_LEN_MAX;
  total_block +=1;

  /* 块号错误，返回 */
  if(total_block < 1 || args->block > total_block )
  {
    return;
  }

  args->data_len = TFTP_DATA_LEN_MAX;
  //判断是否为最后一块
  if(total_block == args->block)
  {
    if(args->tot_bytes%TFTP_DATA_LEN_MAX == 0)
    {
      args->data_len = 0;
    }
    else
    {
      args->data_len = args->tot_bytes - (total_block - 1)*TFTP_DATA_LEN_MAX;
    }
  }
  
  memset(args->data + TFTP_DATA_PKT_HDR_LEN, ('a'-1) + args->block%26 , args->data_len);

  /* 发送数据包 */
  SendTftpDataPacket(upcb, to_ip, to_port, args->block, args->data, args->data_len);

}

/* 读请求处理回调函数，在收到包后，向服务器发送ACK，调用本函数处理 */
void RrqReceiveCallback(void *_args, struct udp_pcb *upcb, struct pbuf *p,const ip_addr_t *addr, u16_t port)
{
  /* 获取连接状态 */
  tftp_connection_args *args = (tftp_connection_args *)_args;
  if(port != args->remote_port)
  {
    /* 清除连接 */
    CleanTftpConnection(upcb, args);

    pbuf_free(p);
    
    return;
  }

  if (CheckTftpIsCorrectAck(p->payload, args->block))
  {
    /* 增加块号 */
    args->block++;
  }
  else
  {
    /* 验证出错，块号不变，重新发送 */
  }

  /* 如果最后一次读取返回的字节少于所请求的字节数，则文件发送完成 */
  if (args->data_len < TFTP_DATA_LEN_MAX)
  {
    /* 清除连接 */
    CleanTftpConnection(upcb, args);

    pbuf_free(p);

    return;
  }

  /* 如果整个文件还没有发送完，则继续 */
  SendNextBlock(upcb, args, addr, port);

  pbuf_free(p);

}

/* TFTP读请求处理*/
int TftpReadProcess(struct udp_pcb *upcb, const ip_addr_t *to, int to_port, char* FileName)
{
  tftp_connection_args *args = NULL;

  /* 这个函数在回调函数中被调用，因此中断被禁用，因此我们可以使用常规的malloc */
  args = mem_malloc(sizeof(tftp_connection_args));
  
  if (!args)
  {
    /* 内存分配失败 */
    SendTftpErrorMessage(upcb, to, to_port, TFTP_ERR_NOTDEFINED);

    CleanTftpConnection(upcb, args);

    return 0;
  }

  /* i初始化连接结构体  */
  args->op = TFTP_RRQ;
  args->remote_port = to_port;
  args->block = 1; /* 块号从1开始 */
  args->tot_bytes = 10*1024*1024;


  /* 注册回调函数 */
  udp_recv(upcb, RrqReceiveCallback, args);

  /* 通过发送第一个块来建立连接，后续块在收到ACK后发送*/
   SendNextBlock(upcb, args, to, to_port);

  return 1;
}

/*写请求处理回调函数，客户端收到ACK后，发送数据包，服务器调用本函数处理 */
void WrqReceiveCallback(void *_args, struct udp_pcb *upcb, struct pbuf *pkt_buf, const ip_addr_t *addr, u16_t port)
{
  tftp_connection_args *args = (tftp_connection_args *)_args;

  u16_t next_block = 0;
  
  if (port != args->remote_port || pkt_buf->len != pkt_buf->tot_len)
  {
    CleanTftpConnection(upcb, args);
    pbuf_free(pkt_buf);
    return;
  }

  next_block = args->block + 1;
  
  /* 判断数据包是否有有效的数据 */
  if ((pkt_buf->len > TFTP_DATA_PKT_HDR_LEN)&&(ExtractTftpBlock(pkt_buf->payload) == next_block))
  {
    /* 更新我们的块号，使之与收到的块号匹配 */
    args->block++;
    /* 更新总字节数 */
    (args->tot_bytes) += (pkt_buf->len - TFTP_DATA_PKT_HDR_LEN);
  }
  else if (ExtractTftpBlock(pkt_buf->payload) == next_block)
  {
    /* 更新我们的块号，使之与收到的块号匹配 */
    args->block++;
  }
  else
  {

  }

  /* 向客户端发送ACK */
  SendTftpAckPacket(upcb, addr, port, args->block);

  /* 判断是否发送完毕，如果数据长度小于512则发送完毕 */
  if (pkt_buf->len < TFTP_DATA_PKT_LEN_MAX)
  {
    CleanTftpConnection(upcb, args);
    pbuf_free(pkt_buf);
  }
  else
  {
    pbuf_free(pkt_buf);
    return;
  }

}

/* TFTP写请求处理 */
int TftpWriteProcess(struct udp_pcb *upcb, const ip_addr_t *to, int to_port, char *FileName)
{
  tftp_connection_args *args = NULL;

  /* 这个函数在回调函数中被调用，因此中断被禁用，因此我们可以使用常规的malloc */
  args = mem_malloc(sizeof(tftp_connection_args));
  
  if (!args)
  {
    SendTftpErrorMessage(upcb, to, to_port, TFTP_ERR_NOTDEFINED);

    CleanTftpConnection(upcb, args);

    return 0;
  }

  args->op = TFTP_WRQ;
  args->remote_port = to_port;
  args->block = 0;      //WRQ响应的块号为0
  args->tot_bytes = 0;

  /* 为控制块注册回调函数 */
  udp_recv(upcb, WrqReceiveCallback, args);

  /* 通过发送第一个ack来发起写事务 */
  SendTftpAckPacket(upcb, to, to_port, args->block);

  return 0;
}

/* 从每一个来自addr:port的新请求创建一个新的端口来服务响应，并启动响应过程 */
static void ProcessTftpRequest(struct pbuf *pkt_buf, const ip_addr_t *addr, u16_t port)
{
  tftp_opcode op = ExtractTftpOpcode(pkt_buf->payload);
  char FileName[50] = {0};
  struct udp_pcb *upcb = NULL;
  err_t err;
  
  /* 生成新的UDP PCB控制块 */
  upcb = udp_new();
  if (!upcb)
  {
    return;
  }
  
  /* 连接 */
  err = udp_connect(upcb, addr, port);
  if (err != ERR_OK)
  {  
    return;
  }
  
  ExtractTftpFilename(FileName, pkt_buf->payload);

  switch (op)
  {
  case TFTP_RRQ:
    {

      TftpReadProcess(upcb, addr, port, FileName);
      break;
    }
  case TFTP_WRQ:
    {
      /* 启动TFTP写模式 */
      TftpWriteProcess(upcb, addr, port, FileName);
      break;
    }
  default:
    {
      /* 异常，发送错误消息 */
      SendTftpErrorMessage(upcb, addr, port, TFTP_ERR_ACCESS_VIOLATION);

      udp_remove(upcb);

      break;
    }
  }
}

/* TFTP服务器回调函数 */
static void TftpServerCallback(void *arg, struct udp_pcb *upcb, struct pbuf *p,const ip_addr_t *addr, u16_t port)
{
  /* 处理新的连接请求 */
  ProcessTftpRequest(p, addr, port);

  pbuf_free(p);
}

/* 初始化TFTP服务器 */
void Tftp_Server_Initialization(void)
{
  err_t err;
  struct udp_pcb *tftp_server_pcb = NULL;

  /* 生成新的 UDP PCB控制块 */
  tftp_server_pcb = udp_new();
  
  /* 判断UDP控制块是否正确生成 */
  if (NULL == tftp_server_pcb)
  {  
    return;
  }

  /* 绑定PCB控制块到指定端口 */
  err = udp_bind(tftp_server_pcb, IP_ADDR_ANY, UDP_TFTP_SERVER_PORT);
  
  if (err != ERR_OK)
  {
    udp_remove(tftp_server_pcb);
    return;
  }

  /* 注册TFTP服务器处理函数 */
  udp_recv(tftp_server_pcb, TftpServerCallback, NULL);
}

/*********** (C) COPYRIGHT 1999-2018 Moonan Technology *********END OF FILE****/