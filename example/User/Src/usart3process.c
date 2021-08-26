/******************************************************************************/
/** 模块名称：Modbus本地主机实例模块                                         **/
/** 文件名称：mcutsprocess.c                                                 **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：Modbus本地主机实例模块测试端口串行接口软件，实现对下位机的通   **/
/**           讯。基于USART3端口，采用RS485，实现Modbus RTU主站。            **/
/**           PB10      USART3_TX       USART3串行发送                       **/
/**           PB11      USART3_RX       USART3串行接收                       **/
/**           PB7       RS485_CTL3      RS485收发控制                        **/
/**           基于STM32F407ZGT6硬件平台，软件库采用HAL FW_F4 V1.26.0库       **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2021-03-29          尹家军            创建文件               **/
/**                                                                          **/
/******************************************************************************/ 

#include "usart3process.h"

#define RS485_CTL3_Pin GPIO_PIN_7
#define RS485_CTL3_GPIO_Port GPIOB

#define MCUT_RECIEVE_ALLOW()   HAL_GPIO_WritePin(RS485_CTL3_GPIO_Port,RS485_CTL3_Pin,GPIO_PIN_RESET)    /*方向为接收*/
#define MCUT_TRANSMIT_ALLOW()  HAL_GPIO_WritePin(RS485_CTL3_GPIO_Port,RS485_CTL3_Pin,GPIO_PIN_SET)      /*方向为发送*/

/*定义远程从站的站地址*/
#define RemoteSlave1    1
#define RemoteSlave2    2
#define RemoteSlave3    3
#define RemoteSlave4    4

#define MCUTRECEIVELENGTH 256    //接收数据的最大长度

UART_HandleTypeDef huart3;

extern AnalogParaTypeDef aPara;
extern DigitalParaTypeDef dPara;

uint16_t mcutRxLength=0;
uint8_t mcutRxBuffer[MCUTRECEIVELENGTH];

RTULocalMasterType deMaster;
uint8_t deReadRegisterCommand[8][8];
RTUAccessedSlaveType deSlave[4]={{RemoteSlave1,0,2,&deReadRegisterCommand[0],NULL,0x00,0x00},
{RemoteSlave2,0,2,&deReadRegisterCommand[2],NULL,0x00,0x00},
{RemoteSlave3,0,2,&deReadRegisterCommand[4],NULL,0x00,0x00},
{RemoteSlave4,0,2,&deReadRegisterCommand[6],NULL,0x00,0x00}};

/* RS485控制引脚初始化 */
static void GPIO_Init_Configuration(void);
/* 本地主机端口USART3初始化 */
static void USART3_Init_Configuration(void);
/*发送数据*/
static void McutSendData(uint8_t *sData,uint16_t sSize);
/*更新读回来的保持寄存器*/
static void DeUpdateHoldingRegister(uint8_t salveAddress,uint16_t startAddress,uint16_t quantity,uint16_t *registerValue);
/*更新读回来的线圈状态*/
static void DeUpdateCoilStatus(uint8_t salveAddress,uint16_t startAddress,uint16_t quantity,bool *stateValue);
/* 写寄存器操作 */
static void WriteSlaveOperation(void);

/*设备本地主机通讯处理*/
void LocalMasterProcess(void)
{
    WriteSlaveOperation();
    
    if(deMaster.readOrder>=deMaster.slaveNumber)
    {
        deMaster.readOrder=0;
    }
    
    if(CheckWriteRTUSlaveNone(&deMaster))      //没有设置命令
    {
        /*发送读数据命令读数据*/
        if(deSlave[deMaster.readOrder].cmdOrder>=deSlave[deMaster.readOrder].commandNumber)
        {
            deSlave[deMaster.readOrder].cmdOrder=0;
        }
        
        McutSendData(deSlave[deMaster.readOrder].pReadCommand[deSlave[deMaster.readOrder].cmdOrder],8);
        
        deSlave[deMaster.readOrder].pLastCommand=deSlave[deMaster.readOrder].pReadCommand[deSlave[deMaster.readOrder].cmdOrder];
        
        deSlave[deMaster.readOrder].cmdOrder++;
        
        HAL_Delay(30);
        
        ParsingSlaveRespondMessage(&deMaster,mcutRxBuffer,NULL);
        
        deMaster.readOrder++;
    }

}

/* 写寄存器操作 */
static void WriteSlaveOperation(void)
{
     /* 写从站1操作 */
    if(GetWriteRTUSlaveEnableFlag(&deMaster,deSlave[0].stationAddress))
    {
        
        ModifyWriteRTUSlaveEnableFlag(&deMaster,deSlave[0].stationAddress,false);
    }
    
    /* 写从站2操作 */
    if(GetWriteRTUSlaveEnableFlag(&deMaster,deSlave[1].stationAddress))
    {
        

        ModifyWriteRTUSlaveEnableFlag(&deMaster,deSlave[1].stationAddress,false);
    }
    
    /* 写从站3操作 */
    if(GetWriteRTUSlaveEnableFlag(&deMaster,deSlave[2].stationAddress))
    {
        
        
        ModifyWriteRTUSlaveEnableFlag(&deMaster,deSlave[2].stationAddress,false);
    }
    
    /* 写从站4操作 */
    if(GetWriteRTUSlaveEnableFlag(&deMaster,deSlave[3].stationAddress))
    {
        
        
        ModifyWriteRTUSlaveEnableFlag(&deMaster,deSlave[3].stationAddress,false);
    }
}

/*发送数据*/
static void McutSendData(uint8_t *sData,uint16_t sSize)
{
    /*关闭中断*/
    __HAL_UART_DISABLE_IT(&huart3,UART_IT_RXNE);
    /*RS485设置为发送模式，准备发送*/
    MCUT_TRANSMIT_ALLOW();
    mcutRxLength=0;
    
    HAL_UART_Transmit(&huart3,sData,sSize,1000);
    
    /*发送完毕，将RS485改为接收模式准备接收*/
    MCUT_RECIEVE_ALLOW();
    
    /*启用串口接收中端*/
    __HAL_UART_ENABLE_IT(&huart3,UART_IT_RXNE);
    
}

/*数据接收中断处理函数,添加到USART3中断响应函数中*/
void USART3_ReceiveDataHandle(void)
{
    if(mcutRxLength>=MCUTRECEIVELENGTH)
    {
        mcutRxLength=0;
    }
    
    if(__HAL_UART_GET_FLAG(&huart3,UART_FLAG_ORE)!=RESET)
    {
        uint8_t rData;
        /*获取接收到的字节*/
        HAL_UART_Receive(&huart3,&rData,1,100);
    }
    
    /*接收寄存器为空,等待字节被对应的串口完全接收*/
    if(__HAL_UART_GET_FLAG(&huart3,UART_FLAG_RXNE)!=RESET)
    {
        uint8_t rData;
        /*获取接收到的字节*/
        HAL_UART_Receive(&huart3,&rData,1,100);
        if((mcutRxLength !=0x00 )||(rData != 0x00))
        {
            mcutRxBuffer[mcutRxLength++] = rData;
        }
        __HAL_UART_CLEAR_FLAG(&huart3,UART_FLAG_RXNE);
    }
}

/*对本地主机端口配置*/
void LocalMasterConfiguration(void)
{
    ObjAccessInfo objInfo;
    
    /* RS485控制引脚初始化 */
    GPIO_Init_Configuration();
    /* 本地主机端口USART3初始化 */
    USART3_Init_Configuration();

    objInfo.unitID=RemoteSlave1;
    objInfo.functionCode=ReadHoldingRegister;
    objInfo.startingAddress=0x00;
    objInfo.quantity=8;
    
    CreateAccessSlaveCommand(objInfo,NULL,deReadRegisterCommand[0]);
    
    objInfo.functionCode=ReadCoilStatus;
    
    CreateAccessSlaveCommand(objInfo,NULL,deReadRegisterCommand[1]);
    
    objInfo.unitID=RemoteSlave2;
    objInfo.functionCode=ReadHoldingRegister;
    objInfo.startingAddress=0x00;
    objInfo.quantity=8;
    
    CreateAccessSlaveCommand(objInfo,NULL,deReadRegisterCommand[2]);
    
    objInfo.functionCode=ReadCoilStatus;
    
    CreateAccessSlaveCommand(objInfo,NULL,deReadRegisterCommand[3]);
    
    objInfo.unitID=RemoteSlave3;
    objInfo.functionCode=ReadHoldingRegister;
    objInfo.startingAddress=0x00;
    objInfo.quantity=8;
    
    CreateAccessSlaveCommand(objInfo,NULL,deReadRegisterCommand[4]);
    
    objInfo.functionCode=ReadCoilStatus;
    
    CreateAccessSlaveCommand(objInfo,NULL,deReadRegisterCommand[5]);
    
    objInfo.unitID=RemoteSlave4;
    objInfo.functionCode=ReadHoldingRegister;
    objInfo.startingAddress=0x00;
    objInfo.quantity=8;
    
    CreateAccessSlaveCommand(objInfo,NULL,deReadRegisterCommand[6]);
    
    objInfo.functionCode=ReadCoilStatus;
    
    CreateAccessSlaveCommand(objInfo,NULL,deReadRegisterCommand[7]);
    
    /*初始化RTU主站对象*/
    InitializeRTUMasterObject(&deMaster,        //主站对象
                              4,                  //主站对应的从站数量
                              deSlave,         //主站管理的从站列表
                              DeUpdateCoilStatus,               //主站更新线圈量函数，无线圈量NULL
                              NULL,               //主站更新状态量函数，无线状态NULL
                              DeUpdateHoldingRegister,  //更新保持寄存器函数
                              NULL    //更行输入寄存器函数
                                  );
    
    MCUT_RECIEVE_ALLOW();
}

/* 本地主机端口USART3初始化 */
static void USART3_Init_Configuration(void)
{
    huart3.Instance = USART3;
    huart3.Init.BaudRate = 9600;
    huart3.Init.WordLength = UART_WORDLENGTH_8B;
    huart3.Init.StopBits = UART_STOPBITS_1;
    huart3.Init.Parity = UART_PARITY_NONE;
    huart3.Init.Mode = UART_MODE_TX_RX;
    huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart3.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart3) != HAL_OK)
    {
        Error_Handler();
    }
    
    /*启用串口接收中端*/
    __HAL_UART_ENABLE_IT(&huart3,UART_IT_RXNE);
}

/* RS485控制引脚初始化 */
static void GPIO_Init_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    /* 使能GPIO端口时钟 */
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    
    /* 设置GPIO引脚初始电平 */
    HAL_GPIO_WritePin(RS485_CTL3_GPIO_Port, RS485_CTL3_Pin, GPIO_PIN_RESET);
    
    /* 配置GPIO引脚RS485_CTL3_Pin */
    GPIO_InitStruct.Pin = RS485_CTL3_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(RS485_CTL3_GPIO_Port, &GPIO_InitStruct);
    
}

/*更新读回来的线圈状态*/
static void DeUpdateCoilStatus(uint8_t salveAddress,uint16_t startAddress,uint16_t quantity,bool *stateValue)
{
    uint16_t startCoil=CoilEndAddress+1;
    
  switch(salveAddress)
    {
    case RemoteSlave1:       //更新读取从站1参数
        {
            startCoil=8;
            break;
        }
    case RemoteSlave2:      //更新读取从站2参数
        {
            startCoil=16;
            break;
        }
    case RemoteSlave3:      //更新读取从站3参数
        {
            startCoil=24;
            break;
        }
    case RemoteSlave4:      //更新读取从站4参数
        {
            startCoil=32;
            break;
        }
    default:                      //故障态
        {
            startCoil=CoilEndAddress+1;
            break;
        }
    }
  
  if(startCoil<=CoilEndAddress)
    {
        for(int i=0;i<quantity;i++)
        {
            dPara.coil[startCoil+i]=stateValue[i];
        }
    }
}

/*更新读回来的保持寄存器*/
static void DeUpdateHoldingRegister(uint8_t salveAddress,uint16_t startAddress,uint16_t quantity,uint16_t *registerValue)
{
    uint16_t startRegister=HoldingRegisterEndAddress+1;
    
    switch(salveAddress)
    {
    case RemoteSlave1:       //更新读取从站1参数
        {
            startRegister=8;
            break;
        }
    case RemoteSlave2:      //更新读取从站2参数
        {
            startRegister=16;
            break;
        }
    case RemoteSlave3:      //更新读取从站3参数
        {
            startRegister=24;
            break;
        }
    case RemoteSlave4:      //更新读取从站4参数
        {
            startRegister=32;
            break;
        }
    default:                      //故障态
        {
            startRegister=HoldingRegisterEndAddress+1;
            break;
        }
    }
    
    if(startRegister<=HoldingRegisterEndAddress)
    {
        for(int i=0;i<quantity;i++)
        {
            aPara.holdingRegister[startRegister+i]=registerValue[i];
        }
    }
}
/*********** (C) COPYRIGHT 1999-2021 Moonan Technology *********END OF FILE****/