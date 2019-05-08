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
/**     V1.0.0  2016-04-17          尹家军            创建文件               **/
/**                                                                          **/
/******************************************************************************/

#ifndef __mbtcpclient_h
#define __mbtcpclient_h

#include "mbtcp.h"
#include "mbcommon.h"

/* 定义可被写的线圈量数据对象类型 */
typedef struct WritedCoilListType {
  uint16_t coilAddress;                 //可写线圈量的地址
  uint8_t writedStatus;                 //写线圈量状态，1需要写，0不需要
  uint8_t value;                        //待写的值
//  struct WritedCoilListType *pNext;     //下一个节点指针
}WritedCoilListNode;

/* 定义可被写保持寄存器数据对象类型 */
typedef struct WritedRegisterListType{
  uint16_t coilAddress;                 //可写保持寄存器地址
  uint16_t writedStatus;                //写寄存器量状态，1需要写，0不需要
  uint16_t value;                       //待写的值
//  struct WritedRegisterListType *pNext; //下一个节点指针
}WritedRegisterListNode;

/* 定义可被写的线圈量数据链表头类型 */
typedef struct WritedCoilListHeadType{
  WritedCoilListNode *pWritedCoilNode;  //可写线圈量节点指针
  uint32_t writedCoilNumber;            //可写线圈量节点的数量
}WritedCoilListHeadNode;

/* 定义可被写保持寄存器数据链表头对象 */
typedef struct WritedRegisterListHeadType{
  WritedRegisterListNode *pWritedRegisterNode;  //可写寄存器量节点指针
  uint32_t writedRegisterNumber;                //可写寄存器量节点的数量
}WritedRegisterListHeadNode;

/* 定义被访问TCP服务器对象类型 */
typedef struct AccessedTCPServerType{
  union {
    uint32_t ipNumber;
    uint8_t ipSegment[4];
  }ipAddress;                                           //服务器的IP地址
  uint32_t flagPresetServer;                            //写服务器请求标志
  WritedCoilListHeadNode pWritedCoilHeadNode;          //可写的线圈量列表
  WritedRegisterListHeadNode pWritedRegisterHeadNode;  //可写的保持寄存器列表
  struct AccessedTCPServerType *pNextNode;              //下一个TCP服务器节点
}TCPAccessedServerType;

/* 定义服务器链表头类型 */
typedef struct ServerListHeadType {
  TCPAccessedServerType *pServerNode;           //服务器节点指针
  uint32_t serverNumber;                        //服务器的数量
}ServerListHeadNode;

/* 定义本地TCP客户端对象类型 */
typedef struct LocalTCPClientType{
  uint32_t transaction;                                 //事务标识符
  uint16_t cmdNumber;                                  //读服务器命令的数量
  uint16_t cmdOrder;                                   //当前从站在从站列表中的位置
  uint8_t (*pReadCommand)[12];                         //读命令列表
  ServerListHeadNode ServerHeadNode;                    //Server对象链表的头节点
  UpdateCoilStatusType pUpdateCoilStatus;               //更新线圈量函数
  UpdateInputStatusType pUpdateInputStatus;             //更新输入状态量函数
  UpdateHoldingRegisterType pUpdateHoldingRegister;     //更新保持寄存器量函数
  UpdateInputResgisterType pUpdateInputResgister;       //更新输入寄存器量函数
}TCPLocalClientType;

/*生成访问服务器的命令*/
uint16_t CreateAccessServerCommand(ObjAccessInfo objInfo,void *dataList,uint8_t *commandBytes);

/*解析收到的服务器相应信息*/
void ParsingServerRespondMessage(TCPLocalClientType *client,uint8_t *recievedMessage);

/*发送命令后，向已发送命令列表中添加命令*/
void AddCommandBytesToList(TCPLocalClientType *client,uint8_t *commandBytes);

/* 使能或者失能写从站操作标志位（修改从站的写使能标志位） */
void ModifyWriteTCPServerEnableFlag(TCPLocalClientType *client,uint8_t ipAddress,bool en);

/* 获得从站的写使能标志位的状态 */
bool GetWriteTCPServerEnableFlag(TCPLocalClientType *client,uint8_t ipAddress);

/* 实例化TCP服务器对象 */
void InstantiateTCPServerObject(TCPAccessedServerType *server,
                                uint8_t ipSegment1,uint8_t ipSegment2,
                                uint8_t ipSegment3,uint8_t ipSegment4);

/* 向TCP客户端添加TCP服务器列表节点 */
void AddTCPServerNode(TCPLocalClientType *client,TCPAccessedServerType *server);

/*初始化TCP客户端对象*/
void InitializeTCPClientObject(TCPLocalClientType *client,
                               uint16_t cmdNumber,
                               uint8_t (*pReadCommand)[12],
                               UpdateCoilStatusType pUpdateCoilStatus,
                               UpdateInputStatusType pUpdateInputStatus,
                               UpdateHoldingRegisterType pUpdateHoldingRegister,
                               UpdateInputResgisterType pUpdateInputResgister
                               );

/* 判断当前是否有写操作使能 */
bool CheckWriteTCPServerNone(TCPLocalClientType *client);


#endif
/*********** (C) COPYRIGHT 1999-2016 Moonan Technology *********END OF FILE****/