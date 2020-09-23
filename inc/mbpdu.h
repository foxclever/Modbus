/******************************************************************************/
/** 模块名称：Modbus通讯                                                     **/
/** 文件名称：mbpdu.h                                                        **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：用于实现modbus总线协议栈PDU部分封装                            **/
/**--------------------------------------------------------------------------**/
/* 一个典型的Modbus数据帧有如下部分组成：                                     */
/* <------------------- MODBUS串行链路应用数据单元（ADU） ----------------->  */
/*              <------ MODBUS简单协议数据单元（PDU） ------->                */
/*  +-----------+---------------+----------------------------+-------------+  */
/*  |  地址域   |    功能码     | 数据域                     | CRC/LRC     |  */
/*  +-----------+---------------+----------------------------+-------------+  */
/*                                                                            */
/* 一个TCP/IP Modbus数据帧由如下部分组成：                                    */
/* <------------------- MODBUS TCP/IP应用数据单元（ADU） ------------------>  */
/*                       <-------- MODBUS简单协议数据单元（PDU） ---------->  */
/*  +--------------------+---------------+---------------------------------+  */
/*  |  MBAP头部          |    功能码     | 数据域                          |  */
/*  +--------------------+---------------+---------------------------------+  */
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2015-06-17          木南              创建文件               **/
/**                                                                          **/
/******************************************************************************/ 

#ifndef __mbpdu_h
#define __mbpdu_h

#include "stdint.h"
#include "stddef.h"
#include "stdbool.h"
#include "string.h"

/*定义Modbus的操作功能码，支持01、02、03、04、05、06、15、16功能码*/
typedef enum {
  ReadCoilStatus=0x01,       	/*读线圈状态（读多个输出位的状态）*/
  ReadInputStatus=0x02,      	/*读输入位状态（读多个输入位的状态）*/
  ReadHoldingRegister=0x03,  	/*读保持寄存器（读多个保持寄存器的数值）*/
  ReadInputRegister=0x04,    	/*读输入寄存器（读多个输入寄存器的数值）*/
  WriteSingleCoil=0x05,      	/*强制单个线圈（强制单个输出位的状态）*/
  WriteSingleRegister=0x06,  	/*预制单个寄存器（设定一个寄存器的数值）*/
  WriteMultipleCoil=0x0F,    	/*强制多个线圈（强制多个输出位的状态）*/
  WriteMultipleRegister=0x10, 	/*预制多个寄存器（设定多个寄存器的数值）*/
  ReadFileRecord=0x14,          /*读文件记录*/
  WriteFileRecord=0x15,         /*写文件记录*/
  MaskWriteRegister=0x16,       /*屏蔽写寄存器*/
  ReadWriteMultiRegister=0x17,  /*读写多个寄存器*/
  ReadDeviceID=0x2B             /*读设备识别码*/
}FunctionCode;

/*定义接收到指令检测错误时的错误码*/
typedef enum{
  Modbus_OK=0x00,
  InvalidFunctionCode=0x01,     //不合法功能代码
  IllegalDataAddress=0x02,      //非法的数据地址
  IllegalDataValues=0x03,       //非法的数据值或者范围
  SlaveFailure=0x04,            //从站设备故障
  Acknowledgement=0x05,         //确认
  SlaveDeviceBusy=0x06,         //从属设备忙
  MemoryParityError=0x08,       //存储奇偶性差错
  GatewayDisabled=0x0A,         //不可用网关路径
  GatewayResponseFailed=0x0B    //网关目标设备响应失败
}ModbusStatus;

/*定义用于传递要访问从站（服务器）的信息*/
typedef struct{
  uint8_t unitID;
  FunctionCode functionCode;
  uint16_t startingAddress;
  uint16_t quantity;
}ObjAccessInfo;

/*作为RTU主站（TCP客户端）时，生成读写RTU从站（TCP服务器）对象的命令*/
uint16_t GenerateReadWriteCommand(ObjAccessInfo objInfo,bool *statusList,uint16_t *registerList,uint8_t commandBytes[]);

/*解析主站（客户端）从服务器读取的数据*/
void TransformClientReceivedData(uint8_t * receivedMessage,uint16_t quantity,bool *statusList,uint16_t *registerLister);

/*生成主站读访问的响应，包括0x01、0x02、0x03、0x04功能码,返回相应信息的长度*/
uint16_t GenerateMasterAccessRespond(uint8_t *receivedMesasage,bool *statusList,uint16_t *registerList,uint8_t *respondBytes);

/*检查功能吗是否正确*/
ModbusStatus CheckFunctionCode(FunctionCode fc);

#endif
/*********** (C) COPYRIGHT 1999-2016 Moonan Technology *********END OF FILE****/
