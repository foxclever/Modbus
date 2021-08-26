/******************************************************************************/
/** 模块名称：不带操作系统LwIP以太网通讯                                     **/
/** 文件名称：tftpfunction.c                                                 **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：用于实现基于LwIP的TFTP服务器操作函数，根据TFTP标准文件         **/
/**           RFC1350实现。TFTP包括5种功能码和4种报文格式。                  **/
/**           五中功能码如下：                                               **/
/**           +-------+--------------------+                                 **/
/**           | Opcode|     operation      |                                 **/
/**           +-------+--------------------+                                 **/
/**           |   1   |Read request (RRQ)  |                                 **/
/**           +-------+--------------------+                                 **/
/**           |   2   |Write request (WRQ) |                                 **/
/**           +-------+--------------------+                                 **/
/**           |   3   |Data (DATA)         |                                 **/
/**           +-------+--------------------+                                 **/
/**           |   4   |Acknowledgment (ACK)|                                 **/
/**           +-------+--------------------+                                 **/
/**           |   5   |Error (ERROR)       |                                 **/
/**           +-------+--------------------+                                 **/
/**           读请求/写请求包格式：                                          **/
/**           +--------+----------+------+-------+------+                    **/
/**           |2 bytes |  string  |1 byte|string |1 byte|                    **/
/**           +--------+----------+------+-------+------+                    **/
/**           | Opcode | Filename |   0  | Mode  |   0  |                    **/
/**           +--------+----------+------+-------+------+                    **/
/**           数据包格式：                                                   **/
/**           +--------+----------+---------+                                **/
/**           |2 bytes | 2 bytes  | n bytes |                                **/
/**           +--------+----------+---------+                                **/
/**           | Opcode |  Block # |   Data  |                                **/
/**           +--------+----------+---------+                                **/
/**           确认包格式：                                                   **/
/**           +--------+---------+                                           **/
/**           | 2 bytes| 2 bytes |                                           **/
/**           +------------------+                                           **/
/**           | Opcode | Block # |                                           **/
/**           +--------+---------+                                           **/
/**           错误包格式：                                                   **/
/**           +--------+----------+--------+------+                          **/
/**           |2 bytes |2 bytes   | string |1 byte|                          **/
/**           +--------+----------+--------+------+                          **/
/**           | Opcode | ErrorCode| ErrMsg |   0  |                          **/
/**           +--------+----------+--------+------+                          **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2018-08-01          木南              创建文件               **/
/**                                                                          **/
/******************************************************************************/ 

#include <string.h>
#include "lwip/inet.h"
#include "tftpfunction.h"


/* 从TFTP信息中解析操作码 */
tftp_opcode ExtractTftpOpcode(char *buf)
{
  return (tftp_opcode)(buf[1]);
}

/* 从TFTP消息中提取块号 */
uint16_t ExtractTftpBlock(char *buf)
{
  u16_t *b = (u16_t*)buf;
  return ntohs(b[1]);
}

/* 从TFTP信息中解析文件名 */
void ExtractTftpFilename(char *fname, char *buf)
{
  strncpy(fname, buf + 2, 30);
}

/* 设置操作码： RRQ / WRQ / DATA / ACK / ERROR */
void SetTftpOpCode(char *buffer, tftp_opcode opcode)
{

  buffer[0] = 0;
  buffer[1] = (u8_t)opcode;
}

/* 设置错误码 */
void SetTftpErrorCode(char *buffer, tftp_errorcode errCode)
{

  buffer[2] = 0;
  buffer[3] = (u8_t)errCode;
}

/* 设置错误消息 */
void SetTftpErrorMessage(char * buffer, char* errormsg)
{
  strcpy(buffer + 4, errormsg);
}

/* 在ACK/DATA第二的字节设置块号 */
void SetTftpBlockNumber(char* packet, u16_t block)
{
  u16_t *p = (u16_t *)packet;
  p[1] = htons(block);
}

/* 为DATA设置消息的最后字节 */
void SetTftpDataMessage(char* packet, char* buf, int buflen)
{
  memcpy(packet + 4, buf, buflen);
}

/* 检查收到的确认是否正确 */
uint32_t CheckTftpIsCorrectAck(char *buf, int block)
{
  /* 判断是否为数据确认包 */
  if (ExtractTftpOpcode(buf) != TFTP_ACK)
    return 0;

  /* 比较块数量 */
  if (block != ExtractTftpBlock(buf))
    return 0;

  return 1;
}

/*********** (C) COPYRIGHT 1999-2018 Moonan Technology *********END OF FILE****/