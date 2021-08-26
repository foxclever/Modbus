/******************************************************************************/
/** 模块名称：Modbus本地从站实例模块                                         **/
/** 文件名称：mcudsprocess.h                                                 **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：Modbus本地从站实例模块上位显示串行接口软件，实现对上位机的通   **/
/**           讯。基于USART1端口，采用RS232，实现Modbus RTU从站。            **/
/**           PA9       USART1_TX       USART1串行发送                       **/
/**           PA10      USART1_RX       USART1串行接收                       **/
/**           PD8       RS232_INT       RS232中断信号                        **/
/**           基于STM32F407ZGT6硬件平台，软件库采用HAL FW_F4 V1.26.0库       **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2021-03-29          木南              创建文件               **/
/**                                                                          **/
/******************************************************************************/ 

#include "usart1process.h"

#define RS232_INT_Pin GPIO_PIN_8
#define RS232_INT_GPIO_Port GPIOD

#define MCUDRECEIVELENGTH 256    //接收数据的最大长度

UART_HandleTypeDef huart1;

extern AnalogParaTypeDef aPara;
extern DigitalParaTypeDef dPara;

uint16_t localStation=1;
uint16_t mcudRxLength=0;
uint8_t mcudRxBuffer[MCUDRECEIVELENGTH];

/* USART1初始化 */
static void USART1_Init_Configuration(void);
/* RS232通讯GPIO初始化 */
static void GPIO_Init_Configuration(void);
/* USART1数据发送函数 */
static void McudSendData(uint8_t *sData,uint16_t sSize);

/*设备对外通讯数据处理*/
void LocalSlaveProcess(void)
{
    uint16_t respondLength=0;
    if(mcudRxLength>=8)
    {
        uint8_t respondBytes[MCUDRECEIVELENGTH];
        
        respondLength=ParsingMasterAccessCommand(mcudRxBuffer,respondBytes,mcudRxLength,localStation);
        if(respondLength!=65535)
        {
            /* USART1数据发送函数 */
            McudSendData(respondBytes,respondLength);
        }
    }
}

/* USART1数据发送函数 */
static void McudSendData(uint8_t *sData,uint16_t sSize)
{
    /*关闭中断*/
    __HAL_UART_DISABLE_IT(&huart1,UART_IT_RXNE);
    mcudRxLength=0;
    
    HAL_UART_Transmit(&huart1,sData,sSize,1000);
    
    /*启用串口接收中端*/
    __HAL_UART_ENABLE_IT(&huart1,UART_IT_RXNE);
}

/* 数据接收中断处理函数,添加到USART1中断响应函数中 */
void USART1_ReceiveDataHandle(void)
{
    if(mcudRxLength>=MCUDRECEIVELENGTH)
    {
        mcudRxLength=0;
    }
    
    if(__HAL_UART_GET_FLAG(&huart1,UART_FLAG_ORE)!=RESET)
    {
        uint8_t rData;
        /*获取接收到的字节*/
        HAL_UART_Receive(&huart1,&rData,1,100);
    }
    
    /*接收寄存器为空,等待字节被对应的串口完全接收*/
    if(__HAL_UART_GET_FLAG(&huart1,UART_FLAG_RXNE)!=RESET)
    {
        uint8_t rData;
        /*获取接收到的字节*/
        HAL_UART_Receive(&huart1,&rData,1,100);
        if((0!=mcudRxLength)||(localStation==rData))
        {
            mcudRxBuffer[mcudRxLength++] = rData;
        }
        __HAL_UART_CLEAR_FLAG(&huart1,UART_FLAG_RXNE);
    }
}

/*设备对外通讯数据处理*/
void LocalSlaveConfiguration(void)
{
    /* USART1初始化 */
    USART1_Init_Configuration();
    
    /* RS232通讯GPIO初始化 */
    GPIO_Init_Configuration();
    
    
}

/* RS232通讯GPIO初始化 */
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
    
    /* 配置GPIO引脚：RS232_INT_Pin */
    GPIO_InitStruct.Pin = RS232_INT_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
}

/* USART1初始化 */
static void USART1_Init_Configuration(void)
{
    huart1.Instance = USART1;
    huart1.Init.BaudRate = 115200;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart1) != HAL_OK)
    {
        Error_Handler();
    }
    
    /*启用串口接收中端*/
    __HAL_UART_ENABLE_IT(&huart1,UART_IT_RXNE);
}

/*********** (C) COPYRIGHT 1999-2021 Moonan Technology *********END OF FILE****/
