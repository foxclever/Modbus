/******************************************************************************/
/** 模块名称：Modbus通讯                                                     **/
/** 文件名称：mbasciimaster.h                                                **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：用于声明Modbus ASCII主站相关属性及方法                         **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2018-09-11          木南              创建文件               **/
/**                                                                          **/
/******************************************************************************/

#ifndef __modbusasciimaster_h
#define __modbusasciimaster_h

#include "mbascii.h"
#include "mbcommon.h"

/* 定义被访问ASCII从站对象类型 */
typedef  struct AccessedASCIISlaveType{
  uint8_t stationAddress;       //站地址
  uint8_t cmdOrder;             //当前命令在命令列表中的位置
  uint16_t commandNumber;       //命令列表中命令的总数
  uint8_t (*pReadCommand)[17];   //读命令列表
  uint8_t *pLastCommand;        //上一次发送的命令
  uint32_t flagPresetCoil;      //预置线圈控制标志位
  uint32_t flagPresetReg;       //预置寄存器控制标志位
}AsciiAccessedSlaveType;

/* 定义本地ASCII主站对象类型 */
typedef struct LocalASCIIMasterType{
  uint32_t flagWriteSlave[8];   //写一个站控制标志位，最多256个站，与站地址对应。
  uint16_t slaveNumber;         //从站列表中从站的数量
  uint16_t readOrder;           //当前从站在从站列表中的位置
  AsciiAccessedSlaveType *pSlave;         //从站列表
  UpdateCoilStatusType pUpdateCoilStatus;       //更新线圈量函数
  UpdateInputStatusType pUpdateInputStatus;     //更新输入状态量函数
  UpdateHoldingRegisterType pUpdateHoldingRegister;     //更新保持寄存器量函数
  UpdateInputResgisterType pUpdateInputResgister;       //更新输入寄存器量函数
}AsciiLocalMasterType;

/*生成访问服务器的命令*/
uint16_t CreateAccessAsciiSlaveCommand(ObjAccessInfo objInfo,void *dataList,uint8_t *commandBytes);

/*解析收到的服务器相应信息*/
void ParsingAsciiSlaveRespondMessage(AsciiLocalMasterType *master,uint8_t *recievedMessage, uint8_t *command,uint16_t rxLength);

/*接收到返回信息后，判断是否是发送命令列表中命令的返回信息*/
int FindAsciiCommandForRecievedMessage(uint8_t *recievedMessage,uint8_t (*commandList)[17],uint16_t commandNumber);

/* 使能或者失能写从站操作标志位（修改从站的写使能标志位） */
void ModifyWriteASCIISlaveEnableFlag(AsciiLocalMasterType *master,uint8_t slaveAddress,bool en);

/* 获得从站的写使能标志位的状态 */
bool GetWriteASCIISlaveEnableFlag(AsciiLocalMasterType *master,uint8_t slaveAddress);

/*初始化RTU主站对象*/
void InitializeASCIIMasterObject(AsciiLocalMasterType *master,uint16_t slaveNumber,
                            AsciiAccessedSlaveType *pSlave,
                            UpdateCoilStatusType pUpdateCoilStatus,
                            UpdateInputStatusType pUpdateInputStatus,
                            UpdateHoldingRegisterType pUpdateHoldingRegister,
                            UpdateInputResgisterType pUpdateInputResgister
                            );

/* 判断当前是否有写操作使能 */
bool CheckWriteASCIISlaveNone(AsciiLocalMasterType *master);

#endif
/*********** (C) COPYRIGHT 1999-2018 Moonan Technology *********END OF FILE****/