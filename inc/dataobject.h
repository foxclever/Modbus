/******************************************************************************/
/** 文件名   dataobject.h                                                    **/
/** 作  者   尹家军                                                          **/
/** 版  本   V1.0.0                                                          **/
/** 日  期   2015年6月17日                                                   **/
/** 简  介   用于声明modbus数据对象的相关属性和方法                          **/
/******************************************************************************/

#ifndef __dataobject_h
#define __dataobject_h

#include "stdint.h"
#include "stddef.h"
#include <math.h>
#include "stdbool.h"

//定义数据对象，0线圈，1输入状态，3输入寄存器，4保持寄存器
enum DataObjectType {
	Coil=0,				//线圈对象
	InputStatus=1,                  //输入状态
	InputRegister=2,		//输入寄存器
	HoldingRegister=3		//保持寄存器
};

//定义一个字节按位索引的结构
typedef struct bindex
{
	uint8_t bit0:1;
	uint8_t bit1:1;
	uint8_t bit2:1;
	uint8_t bit3:1;
	uint8_t bit4:1;
	uint8_t bit5:1;
	uint8_t bit6:1;
	uint8_t bit7:1;
}BitIndex;

//定义用于Modbus寄存器数据存储的结构体
typedef struct rnode
{
  uint16_t index;     	//参数编号
  uint8_t hiByte;     	//数据值高字节
  uint8_t loByte;		//数据值低字节
  struct rnode * next; 	//下一个节点
}RegisterNode;

//定义用于Modbus状态量数据存储的结构体
typedef struct snode
{
	uint16_t index;     	//参数编号
	uint8_t statusByte;     //以字节的方式存储状态量
	struct snode * next; 	//下一个节点
}StatusNode;

//定义用于Modbus中的数据对象，包括种类,起始地址和数量（enum DataObjectType中规定的种类）
typedef struct {
	enum DataObjectType type;
	uint16_t startingAddress;
	uint16_t quantity;
}DataObject;

//定义状态量的对象存储结构，包括初始节点的地址，数据对象的起始地址，总的数量
typedef struct {
	StatusNode * startNode;
	uint16_t startingAddress;
	uint16_t quantity;
}StatusObject;

//定义寄存器量的对象存储结构，包括初始节点的地址，数据对象的起始地址，总的数量
typedef struct {
	RegisterNode * startNode;
	uint16_t startingAddress;
	uint16_t quantity;
}RegisterObject;

//初始化数据存储区域（创建线圈量、输入状态量、保持寄存器、输入寄存器的存储区域）
//输入为结构体数组，在应用程序中定义
void InitializeDataStorageStructure(DataObject dataObject[]);

//从对象地址获取值
uint16_t GetObjectValue(uint8_t result[],DataObject dataObject);

//设置对象的值
void SetObjectValue(uint8_t value[],DataObject dataObject);
#endif
/*********** (C) COPYRIGHT 1999-2016 Moonan Technology *********END OF FILE****/