/******************************************************************************/
/** 文件名   dataobject.c                                                    **/
/** 作  者   尹家军                                                          **/
/** 版  本   V1.0.0                                                          **/
/** 日  期   2015年6月17日                                                   **/
/** 简  介   用于定义modbus数据对象的相关属性和方法，包括三项功能            **/
/***1、初始化存储区域，创建数据对象********************************************/
/***2、读取存储区域，获取数据对象的值******************************************/
/***3、写入存储区域，修改数据对象的值******************************************/
/******************************************************************************/

#include "dataobject.h"

//定义变量记录每种数据存储结构的地址、起始位置及数量
StatusObject coilObject={NULL,0,0};
StatusObject inputStatusObject={NULL,0,0};
RegisterObject inputRegisterObject={NULL,0,0};
RegisterObject holdingRegisterObject={NULL,0,0};

/******Begin 声明私有函数******/
//初始化线圈对象的存储区域
void GenerateCoilStorageStructure(DataObject dataObject);
//初始化输入状态对象的存储区域
void GenerateInputStatusStorageStructure(DataObject dataObject);
//初始化输入寄存器对象的存储区域
void GenerateInputRegisterStorageStructure(DataObject dataObject);
//初始化保持寄存器对象的存储区域
void GenerateHoldingRegisterStorageStructure(DataObject dataObject);
//生成存储状态变量的存储区域，包括线圈和输入状态
StatusNode *GenerateStatusObjectStorageStructure(DataObject dataObject);
//生成存储寄存器量的存储区域,包括保持寄存器和输入寄存器
RegisterNode *GenerateRegisterObjectStorageStructure(DataObject dataObject);

//获取线圈对象的值
uint16_t GetCoilObjectValue(uint8_t result[],DataObject dataObject);
//获取输入状态对象的值
uint16_t GetInputStatusObjectValue(uint8_t result[],DataObject dataObject);
//获取保持寄存器对象的值
uint16_t GetHoldingRegisterObjectValue(uint8_t result[],DataObject dataObject);
//获取输入寄存器状态的值
uint16_t GetInputRegisterObjectValue(uint8_t result[],DataObject dataObject);
//读取状态量的值，返回值是字节数,包括线圈和输入状态
uint16_t GetStatusObjectValue(StatusObject sobject,uint8_t result[],DataObject dataObject);
//读取寄存器对象的值，返回值是字节数，包括保持寄存器和输入寄存器
uint16_t GetRegisterObjectValue(RegisterObject robject,uint8_t result[],DataObject dataObject);

//设置Coil对象的值
void SetCoilObjectValue(uint8_t value[],DataObject dataObject);
//设置输入状态对象的值
void SetInputStatusObjectValue(uint8_t value[],DataObject dataObject);
//设置输入寄存器对象的值
void SetInputRegisterObjectValue(uint8_t value[],DataObject dataObject);
//设置保持寄存器状态的值
void SetHoldingRegisterObjectValue(uint8_t value[],DataObject dataObject);
//设置状态对象的值
void SetStatusObjectValue(StatusObject sobject,uint8_t value[],DataObject dataObject);
//设置寄存器对象的值
void SetRegisterObjectValue(RegisterObject robject,uint8_t value[],DataObject dataObject);
/******End 声明私有函数******/		

/******Begin 初始化存储区域创，建数据对象******/
void (*GenerateDataObjectStructure[])(DataObject dataObject)={GenerateCoilStorageStructure,\
															  GenerateInputStatusStorageStructure,\
															  GenerateInputRegisterStorageStructure,\
															  GenerateHoldingRegisterStorageStructure};

//初始化数据存储区域（创建线圈量、输入状态量、保持寄存器、输入寄存器的存储区域）
//输入为结构体数组，在应用程序中定义
void InitializeDataStorageStructure(DataObject dataObject[])
{
	uint16_t length=sizeof(dataObject)/sizeof(dataObject[0]);
	if((length<1)||(length>4))
	{
		return;
	}
	for(int i=0;i<length;i++)
	{
		(GenerateDataObjectStructure[dataObject[i].type])(dataObject[i]);
	}
}

//初始化线圈对象的存储区域
void GenerateCoilStorageStructure(DataObject dataObject)
{
	coilObject.startNode=GenerateStatusObjectStorageStructure(dataObject);
	coilObject.startingAddress=dataObject.startingAddress;
	coilObject.quantity=dataObject.quantity;
}

//初始化输入状态对象的存储区域
void GenerateInputStatusStorageStructure(DataObject dataObject)
{
	inputStatusObject.startNode=GenerateStatusObjectStorageStructure(dataObject);
	inputStatusObject.startingAddress=dataObject.startingAddress;
	inputStatusObject.quantity=dataObject.quantity;
}

//初始化输入寄存器对象的存储区域
void GenerateInputRegisterStorageStructure(DataObject dataObject)
{
	inputRegisterObject.startNode=GenerateRegisterObjectStorageStructure(dataObject);
	inputRegisterObject.startingAddress=dataObject.startingAddress;
	inputRegisterObject.quantity=dataObject.quantity;
}

//初始化保持寄存器对象的存储区域
void GenerateHoldingRegisterStorageStructure(DataObject dataObject)
{
	holdingRegisterObject.startNode=GenerateRegisterObjectStorageStructure(dataObject);
	holdingRegisterObject.startingAddress=dataObject.startingAddress;
	holdingRegisterObject.quantity=dataObject.quantity;
}

//生成存储状态变量的存储区域，包括线圈和输入状态
StatusNode *GenerateStatusObjectStorageStructure(DataObject dataObject)
{
	StatusNode *startNode=NULL;
	StatusNode *lastNode=NULL;
	uint16_t length=dataObject.quantity/8;

	for(int i=0;i<length;i++)
	{
		StatusNode node={dataObject.startingAddress+i*8,0,NULL};
		
		if(!i)
		{
			startNode=&node;
		}
		else
		{
			lastNode->next=&node;
		}
		lastNode=&node;
	}
	
	return startNode;
}

//生成存储寄存器量的存储区域,包括保持寄存器和输入寄存器
RegisterNode *GenerateRegisterObjectStorageStructure(DataObject dataObject)
{
	RegisterNode *startNode=NULL;
	RegisterNode *lastNode=NULL;
	for(int i=0;i<dataObject.quantity;i++)
	{
		RegisterNode node={i,0,0,NULL};
		if(!i)
		{
			startNode=&node;
		}
		else
		{
			lastNode->next=&node;
		}
		lastNode=&node;
	}
	
	return startNode;
}

/******End 初始化存储区域，建数据对象******/

/******Begin 获取对象的值******/
uint16_t (*GetDataObjectValue[])(uint8_t result[],DataObject dataObject)={GetCoilObjectValue,\
																		  GetInputStatusObjectValue,\
																		  GetInputRegisterObjectValue,\
																		  GetHoldingRegisterObjectValue};

//从对象地址获取值
uint16_t GetObjectValue(uint8_t result[],DataObject dataObject)
{
	uint16_t byteCount=0;
	byteCount=GetDataObjectValue[dataObject.type](result,dataObject);
	return byteCount;
}

//获取线圈对象的值
uint16_t GetCoilObjectValue(uint8_t result[],DataObject dataObject)
{
	//判断该区域是否开辟存储空间,判断地址是否在合法区间
	if((coilObject.startNode==NULL)||\
	   (dataObject.startingAddress<coilObject.startingAddress)||\
	   ((dataObject.startingAddress+dataObject.quantity)>(coilObject.startingAddress+coilObject.quantity)))
	{
		return 0;
	}
	
	return GetStatusObjectValue(coilObject,result,dataObject);
}

//获取输入状态对象的值
uint16_t GetInputStatusObjectValue(uint8_t result[],DataObject dataObject)
{
	//判断该对象类型是否开辟了存储空间,判断访问的地址是否合法
	if((inputStatusObject.startNode==NULL)||\
	   (dataObject.startingAddress<inputStatusObject.startingAddress)||\
	   ((dataObject.startingAddress+dataObject.quantity)>(inputStatusObject.startingAddress+inputStatusObject.quantity)))
	{
		return 0;
	}
	return GetStatusObjectValue(inputStatusObject,result,dataObject);
}

//获取保持寄存器对象的值
uint16_t GetHoldingRegisterObjectValue(uint8_t result[],DataObject dataObject)
{
	//判断该对象类型是否开辟了存储空间,判断访问的地址是否合法
	if((holdingRegisterObject.startNode==NULL)||\
	   (dataObject.startingAddress<holdingRegisterObject.startingAddress)||\
	   ((dataObject.startingAddress+dataObject.quantity)>(holdingRegisterObject.startingAddress+holdingRegisterObject.quantity)))
	{
		return 0;
	}
	return GetRegisterObjectValue(holdingRegisterObject,result,dataObject);
}

//获取输入寄存器状态的值
uint16_t GetInputRegisterObjectValue(uint8_t result[],DataObject dataObject)
{
	//判断该对象类型是否开辟了存储空间,判断访问的地址是否合法
	if((inputRegisterObject.startNode==NULL)||\
	   (dataObject.startingAddress<inputRegisterObject.startingAddress)||\
	   ((dataObject.startingAddress+dataObject.quantity)>(inputRegisterObject.startingAddress+inputRegisterObject.quantity)))
	{
		return 0;
	}
	return GetRegisterObjectValue(inputRegisterObject,result,dataObject);
}

//读取状态量的值，返回值是字节数,包括线圈和输入状态
uint16_t GetStatusObjectValue(StatusObject sobject,uint8_t result[],DataObject dataObject)
{
	StatusNode *node=sobject.startNode;
	uint16_t startPosition=((dataObject.startingAddress-sobject.startingAddress)/8)*8+sobject.startingAddress;
	uint16_t endPosition=((dataObject.startingAddress+dataObject.quantity-1-sobject.startingAddress)/8)*8+sobject.startingAddress;
	uint16_t length=(dataObject.startingAddress+dataObject.quantity-1-sobject.startingAddress)/8+1;
	
	uint8_t value[10];

	for(int i=0;i<length;i++)
	{
		if((startPosition<=node->index)&&(node->index<=endPosition))
		{
			//将对应的值取出来
			value[i]=node->statusByte;
		}
		node=node->next;
	}
	uint16_t byteCount=(uint16_t)ceil(dataObject.quantity/8);
	uint16_t offset=(dataObject.startingAddress-sobject.startingAddress)%8;
	uint16_t eoffset=7-((dataObject.startingAddress-sobject.startingAddress+dataObject.quantity-1)%8);
	uint16_t startIndex=(startPosition-sobject.startingAddress)/8;
	uint16_t endIndex=(endPosition-sobject.startingAddress)/8;
	value[endIndex]=(value[endIndex]<<eoffset)>>eoffset;
	
	for(int i=startIndex;i<=endIndex;i++)
	{
		if((i+1)<=endIndex)
		{
			result[i-startIndex]=(value[i]>>offset)+(value[i+1]<<(7-offset));
		}
		else
		{
			if((i-startIndex)<byteCount)
			{
				result[i-startIndex]=value[i]>>offset;
			}
		}
	}
	
	return byteCount;
}

//读取寄存器对象的值，返回值是字节数，包括保持寄存器和输入寄存器
uint16_t GetRegisterObjectValue(RegisterObject robject,uint8_t result[],DataObject dataObject)
{
	RegisterNode *node=robject.startNode;
	uint16_t byteCount=0;
	for(int i=robject.startingAddress;i<dataObject.startingAddress+dataObject.quantity;i++)
	{
		if((node!=NULL)&&(node->index>=dataObject.startingAddress))
		{
			result[byteCount++]=node->hiByte;
			result[byteCount++]=node->loByte;
		}
		
		node=node->next;
	}
	return byteCount;
}
/******End 获取对象的值******/

/******Begin 设置对象的值******/
void (*SetDataObjectValue[])(uint8_t value[],DataObject dataObject)={SetCoilObjectValue,SetInputStatusObjectValue,SetInputRegisterObjectValue,SetHoldingRegisterObjectValue};

//设置对象的值
void SetObjectValue(uint8_t value[],DataObject dataObject)
{
	SetDataObjectValue[dataObject.type](value,dataObject);
}

//设置Coil对象的值
void SetCoilObjectValue(uint8_t value[],DataObject dataObject)
{
	//判断该区域是否开辟存储空间,判断地址是否在合法区间
	if((coilObject.startNode==NULL)||\
	   (dataObject.startingAddress<coilObject.startingAddress)||\
	   ((dataObject.startingAddress+dataObject.quantity)>(coilObject.startingAddress+coilObject.quantity)))
	{
		return;
	}
	SetStatusObjectValue(coilObject,value,dataObject);
}

//设置输入状态对象的值
void SetInputStatusObjectValue(uint8_t value[],DataObject dataObject)
{
	//判断该对象类型是否开辟了存储空间,判断访问的地址是否合法
	if((inputStatusObject.startNode==NULL)||\
	   (dataObject.startingAddress<inputStatusObject.startingAddress)||\
	   ((dataObject.startingAddress+dataObject.quantity)>(inputStatusObject.startingAddress+inputStatusObject.quantity)))
	{
		return;
	}
	SetStatusObjectValue(inputStatusObject,value,dataObject);
}

//设置输入寄存器对象的值
void SetInputRegisterObjectValue(uint8_t value[],DataObject dataObject)
{
	//判断该对象类型是否开辟了存储空间,判断访问的地址是否合法
	if((inputRegisterObject.startNode==NULL)||\
	   (dataObject.startingAddress<inputRegisterObject.startingAddress)||\
	   ((dataObject.startingAddress+dataObject.quantity)>(inputRegisterObject.startingAddress+inputRegisterObject.quantity)))
	{
		return;
	}
	SetRegisterObjectValue(inputRegisterObject,value,dataObject);
}

//设置保持寄存器状态的值
void SetHoldingRegisterObjectValue(uint8_t value[],DataObject dataObject)
{
	//判断该对象类型是否开辟了存储空间,判断访问的地址是否合法
	if((holdingRegisterObject.startNode==NULL)||\
	   (dataObject.startingAddress<holdingRegisterObject.startingAddress)||\
	   ((dataObject.startingAddress+dataObject.quantity)>(holdingRegisterObject.startingAddress+holdingRegisterObject.quantity)))
	{
		return;
	}
	SetRegisterObjectValue(holdingRegisterObject,value,dataObject);
}

//设置状态对象的值
void SetStatusObjectValue(StatusObject sobject,uint8_t value[],DataObject dataObject)
{
/*	StatusNode *node=sobject.startNode;
	uint16_t byteCount=(dataObject.startingAddress+dataObject.quantity-sobject.startingAddress)/8+1;
	uint16_t leftmove=(dataObject.startingAddress-sobject.startingAddress)%8;
	uint16_t startByte=((dataObject.startingAddress-sobject.startingAddress)/8)*8+sobject.startingAddress;
	uint16_t endByte=((dataObject.startingAddress+dataObject.quantity-sobject.startingAddress)/8)*8+sobject.startingAddress;
	for(int i=sobject.startingAddress;i<=endByte;i++)
	{
		
	}*/
}

//设置寄存器对象的值
void SetRegisterObjectValue(RegisterObject robject,uint8_t value[],DataObject dataObject)
{
	RegisterNode *node=robject.startNode;
	int index=0;
	for(int i=robject.startingAddress;i<(dataObject.startingAddress+dataObject.quantity);i++)
	{
		if((i>=dataObject.startingAddress)&&(index<dataObject.quantity*2))
		{
			node->hiByte=value[index*2];
			node->loByte=value[index*2+1];
			index++;
		}
		node=node->next;
	}
}
/******End 设置对象的值******/

/*********** (C) COPYRIGHT 1999-2016 Moonan Technology *********END OF FILE****/