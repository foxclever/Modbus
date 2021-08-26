/******************************************************************************/
/** 模块名称：不带操作系统LwIP以太网通讯                                     **/
/** 文件名称：tftpprocess.h                                                  **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：用于实现基于LwIP的TFTP服务器功能                               **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2018-08-01          木南              创建文件               **/
/**                                                                          **/
/******************************************************************************/ 

#ifndef __TFTP_PROCESS_H
#define __TFTP_PROCESS_H

#include "lwip/mem.h"
#include "lwip/udp.h"
#include "tftpfunction.h"
#include <stdio.h>
#include <string.h>

#define MFS_MODE_READ 0
#define MFS_MODE_WRITE 1

#define TFTP_OPCODE_LEN         2
#define TFTP_BLKNUM_LEN         2
#define TFTP_ERRCODE_LEN        2
#define TFTP_DATA_LEN_MAX       512
#define TFTP_DATA_PKT_HDR_LEN   (TFTP_OPCODE_LEN + TFTP_BLKNUM_LEN)
#define TFTP_ERR_PKT_HDR_LEN    (TFTP_OPCODE_LEN + TFTP_ERRCODE_LEN)
#define TFTP_ACK_PKT_LEN        (TFTP_OPCODE_LEN + TFTP_BLKNUM_LEN)
#define TFTP_DATA_PKT_LEN_MAX   (TFTP_DATA_PKT_HDR_LEN + TFTP_DATA_LEN_MAX)
#define TFTP_MAX_RETRIES        3
#define TFTP_TIMEOUT_INTERVAL   5

typedef struct
{
  int op;    /* RRQ/WRQ */

  /* last block read */
  char data[TFTP_DATA_PKT_LEN_MAX];
  int  data_len;
  
  u16_t remote_port;

  /* next block number */
  u16_t block;

  /* total number of bytes transferred */
  u32_t tot_bytes;
}tftp_connection_args;

/* 初始化TFTP服务器 */
void Tftp_Server_Initialization(void);

int tftp_process_write(struct udp_pcb *upcb2,const ip_addr_t *to, int to_port, char* FileName);
int tftp_process_read(struct udp_pcb *upcb2, const ip_addr_t *to, int to_port, char* FileName);

#endif  /* __TFTP_PROCESS_H */

/*********** (C) COPYRIGHT 1999-2018 Moonan Technology *********END OF FILE****/