/******************************************************************************/
/** 模块名称：Modbus通讯                                                     **/
/** 文件名称：mbtcpclient.c                                                  **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：用于声明Modbus TCP客户端的相关属性及方法                       **/
/**           1、Modbus TCP客户端用户应用的接口层                            **/
/**           2、实现服务器访问命令的生成并将其传回应用层                    **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2016-04-17          木南              创建文件               **/
/**     V1.1.0  2020-09-01          木南              调整部分结构定义       **/
/**                                                                          **/
/******************************************************************************/

#ifndef __mbtcpclient_h
#define __mbtcpclient_h

#include "mbtcp.h"
#include "mbcommon.h"

/* 定义可被写的线圈量数据对象类型 */
typedef struct WritedCoilListType {
  uint16_t coilAddress;         //可写线圈量的地址
  uint8_t value;                //待写的值
  uint8_t writedStatus:1;       //写线圈量状态，1需要写，0不需要
  uint8_t conStatus:1;          //是否后接连续地址，1连续，0不连续
}WritedCoilListNode;

/* 定义可被写保持寄存器数据对象类型 */
typedef struct WritedRegisterListType{
  uint16_t registerAddress;     //可写保持寄存器地址
  uint16_t value;               //待写的值
  uint16_t writedStatus:1;      //写寄存器量状态，1需要写，0不需要
  uint16_t conStatus:1;          //是否后接连续地址，1连续，0不连续
  uint16_t dataType:14;         //数据包含几个寄存器，1个，2个，4个，0表示与前一寄存器同属一个变量
}WritedRegisterListNode;

/* 定义被访问TCP服务器对象类型 */
typedef struct AccessedTCPServerType{
  union {
    uint32_t ipNumber;
    uint8_t ipSegment[4];
  }ipAddress;                           //服务器的IP地址
  uint16_t port;                        //写服务器请求标志

  uint16_t flagPresetServer:2;          //写服务器请求标志
  uint16_t cmdNumber:7;                 //读服务器命令的数量
  uint16_t cmdOrder:7;                  //当前命令在命令列表中的位置
  uint8_t (*pReadCommand)[12];          //读命令列表，存储读操作命令

  uint16_t writedCoilNumber;                  //可写线圈量节点的数量
  uint16_t writedRegisterNumber;              //可写寄存器量节点的数量
  WritedCoilListNode *pWritedCoilList;        //可写线圈量节点指针  
  WritedRegisterListNode *pWritedRegisterList;  //可写寄存器量节点指针
  struct AccessedTCPServerType *pNextNode;      //下一个TCP服务器节点
}TCPAccessedServerType;

/* 定义本地TCP客户端对象类型 */
typedef struct LocalTCPClientType{
  uint16_t transaction;                         //事务标识符
  TCPAccessedServerType *pServerList;           //服务器节点列表
  TCPAccessedServerType *pCurrentServer;        //当前连接的服务器节点
  UpdateCoilStatusType pUpdateCoilStatus;               //更新线圈量函数
  UpdateInputStatusType pUpdateInputStatus;             //更新输入状态量函数
  UpdateHoldingRegisterType pUpdateHoldingRegister;     //更新保持寄存器量函数
  UpdateInputResgisterType pUpdateInputResgister;       //更新输入寄存器量函数
}TCPLocalClientType;

/*生成访问服务器的命令*/
uint16_t CreateAccessServerCommand(TCPLocalClientType *client,ObjAccessInfo objInfo,void *dataList,uint8_t *commandBytes);

/*解析收到的服务器相应信息*/
void ParsingServerRespondMessage(TCPLocalClientType *client,uint8_t *recievedMessage);

/*发送命令后，向已发送命令列表中添加命令*/
void AddCommandBytesToList(TCPLocalClientType *client,uint8_t *commandBytes);

/* 使能或者失能写服务器操作标志位（修改从站的写使能标志位） */
void ModifyWriteTCPServerEnableFlag(TCPLocalClientType *client,uint8_t ipAddress,bool en);

/* 获得服务器的写使能标志位的状态 */
bool GetWriteTCPServerEnableFlag(TCPLocalClientType *client,uint8_t ipAddress);

/* 判断当前是否有写操作使能，返回True则没有写操作 */
bool CheckWriteTCPServerNone(TCPLocalClientType *client);

/* 实例化TCP服务器对象 */
void InstantiateTCPServerObject(TCPAccessedServerType *server,          //要实例化的服务器对象
                                TCPLocalClientType *client,             //服务器所属本地客户端对象
                                uint8_t ipSegment1,                     //IP地址第1段
                                uint8_t ipSegment2,                     //IP地址第2段
                                uint8_t ipSegment3,                     //IP地址第3段
                                uint8_t ipSegment4,                     //IP地址第4段
                                uint16_t port,                          //端口，默认为502
                                uint16_t cmdNumber,                     //读命令的数量，最多127
                                uint8_t(*pReadCommand)[12],             //读命令列表
                                uint16_t writedCoilNumber,              //可写线圈量节点的数量
                                WritedCoilListNode *pCoilList,          //写线圈列表
                                uint16_t writedRegisterNumber,         //可写寄存器量节点的数量
                                WritedRegisterListNode *pRegisterList); //写寄存器列表



/*初始化TCP客户端对象*/
void InitializeTCPClientObject(TCPLocalClientType *client,      //要初始化的客户端对象
                               UpdateCoilStatusType pUpdateCoilStatus,
                               UpdateInputStatusType pUpdateInputStatus,
                               UpdateHoldingRegisterType pUpdateHoldingRegister,
                               UpdateInputResgisterType pUpdateInputResgister
                               );




#endif
/*********** (C) COPYRIGHT 1999-2016 Moonan Technology *********END OF FILE****/