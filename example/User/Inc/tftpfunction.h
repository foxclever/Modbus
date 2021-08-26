/******************************************************************************/
/** 模块名称：不带操作系统LwIP以太网通讯                                     **/
/** 文件名称：tftpfunction.h                                                 **/
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

#ifndef __TFTP_FUNCTION_H
#define __TFTP_FUNCTION_H

/* TFTP操作码定义 */
typedef enum {
  TFTP_RRQ = 1,
  TFTP_WRQ = 2,
  TFTP_DATA = 3,
  TFTP_ACK = 4,
  TFTP_ERROR = 5
} tftp_opcode;


/* TFTP错误码定义 */
typedef enum {
  TFTP_ERR_NOTDEFINED,
  TFTP_ERR_FILE_NOT_FOUND,
  TFTP_ERR_ACCESS_VIOLATION,
  TFTP_ERR_DISKFULL,
  TFTP_ERR_ILLEGALOP,
  TFTP_ERR_UKNOWN_TRANSFER_ID,
  TFTP_ERR_FILE_ALREADY_EXISTS,
  TFTP_ERR_NO_SUCH_USER,
} tftp_errorcode;

/* 从TFTP信息中解析操作码 */
tftp_opcode ExtractTftpOpcode(char *buf);

/* 从TFTP消息中提取块号 */
uint16_t ExtractTftpBlock(char *buf);

/* 从TFTP信息中解析文件名 */
void ExtractTftpFilename(char *fname, char *buf);

/* 设置操作码： RRQ / WRQ / DATA / ACK / ERROR */
void SetTftpOpCode(char *buffer, tftp_opcode opcode);

/* 设置错误码 */
void SetTftpErrorCode(char *buffer, tftp_errorcode errCode);

/* 设置错误消息 */
void SetTftpErrorMessage(char * buffer, char* errormsg);

/* 在ACK/DATA第二的字节设置块号 */
void SetTftpBlockNumber(char* packet, u16_t block);

/* 为DATA设置消息的最后字节 */
void SetTftpDataMessage(char* packet, char* buf, int buflen);

/* 检查收到的确认是否正确 */
uint32_t CheckTftpIsCorrectAck(char *buf, int block);

#endif

/*********** (C) COPYRIGHT 1999-2018 Moonan Technology *********END OF FILE****/