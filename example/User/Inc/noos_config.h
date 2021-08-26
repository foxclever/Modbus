/******************************************************************************/
/** 模块名称：不带操作系统LwIP以太网通讯                                     **/
/** 文件名称：noos_config.h                                                  **/
/** 版    本：V1.0.0                                                         **/
/** 简    介：用于实现基于LwIP的软件功能的配置                               **/
/**--------------------------------------------------------------------------**/
/** 修改记录：                                                               **/
/**     版本      日期              作者              说明                   **/
/**     V1.0.0  2018-08-01          木南              创建文件               **/
/**                                                                          **/
/******************************************************************************/ 

#ifndef __NOOS_CONFIG_H
#define __NOOS_CONFIG_H

#include "stdbool.h"
#include "stm32f4xx_hal.h"
#include "mbconfig.h"

/* GPIO宏定义 */
#define ETH_MII_INT_Pin GPIO_PIN_0
#define ETH_MII_INT_GPIO_Port GPIOB

#ifndef UDP_SERVER_ENABLE
#define UDP_SERVER_ENABLE       (0)
#endif

#ifndef UDP_CLIENT_ENABLE
#define UDP_CLIENT_ENABLE       (0)
#endif

#ifndef TFTP_SERVER_ENABLE
#define TFTP_SERVER_ENABLE      (0)
#endif

#ifndef TCP_SERVER_ENABLE
#define TCP_SERVER_ENABLE       (1)
#endif

#ifndef TCP_CLIENT_ENABLE
#define TCP_CLIENT_ENABLE       (0)
#endif

#ifndef HTTP_SERVER_ENABLE
#define HTTP_SERVER_ENABLE       (0)
#endif

#ifndef HTTP_CLIENT_ENABLE
#define HTTP_CLIENT_ENABLE       (0)
#endif

#ifndef TELNET_SERVER_ENABLE
#define TELNET_SERVER_ENABLE       (0)
#endif

/*定义用于变量操作及通讯的类型*/
typedef union {
    struct {
        uint32_t	beatTime;	//心跳检测
        float	mbAI1;	//模拟量测试
        float	mbAO1;	//模拟量测试
        uint16_t	mbAI2;	//模拟量测试
        uint16_t	mbAO2;	//模拟量测试
		
        float	mbSalve1AI1;	//目标从站1的模拟量输入参数1
        uint32_t	mbSalve1AI2;	//目标从站1的模拟量输入参数2
        uint16_t	mbSalve1AI3;	//目标从站1的模拟量输入参数3
        uint16_t	mbSalve1AO1;	//目标从站1的模拟量输出参数1
        uint16_t	mbSalve1AO2;	//目标从站1的模拟量输出参数2
        uint16_t	mbSalve1AO3;	//目标从站1的模拟量输出参数3
		
        float	mbSalve2AI1;	//目标从站2的模拟量输入参数1
        uint32_t	mbSalve2AI2;	//目标从站2的模拟量输入参数2
        uint16_t	mbSalve2AI3;	//目标从站2的模拟量输入参数3
        uint16_t	mbSalve2AO1;	//目标从站2的模拟量输出参数1
        uint16_t	mbSalve2AO2;	//目标从站2的模拟量输出参数2
        uint16_t	mbSalve2AO3;	//目标从站2的模拟量输出参数3
		
        float	mbSalve3AI1;	//目标从站3的模拟量输入参数1
        uint32_t	mbSalve3AI2;	//目标从站3的模拟量输入参数2
        uint16_t	mbSalve3AI3;	//目标从站3的模拟量输入参数3
        uint16_t	mbSalve3AO1;	//目标从站3的模拟量输出参数1
        uint16_t	mbSalve3AO2;	//目标从站3的模拟量输出参数2
        uint16_t	mbSalve3AO3;	//目标从站3的模拟量输出参数3
		
        float	mbSalve4AI1;	//目标从站4的模拟量输入参数1
        uint32_t	mbSalve4AI2;	//目标从站4的模拟量输入参数2
        uint16_t	mbSalve4AI3;	//目标从站4的模拟量输入参数3
        uint16_t	mbSalve4AO1;	//目标从站4的模拟量输出参数1
        uint16_t	mbSalve4AO2;	//目标从站4的模拟量输出参数2
        uint16_t	mbSalve4AO3;	//目标从站4的模拟量输出参数3
        
    }phyPara;
    uint16_t holdingRegister[HoldingRegisterEndAddress+1]; /*保持寄存器*/
}AnalogParaTypeDef;

typedef union {
    struct {
        bool	mbDI1;	//数字量输入参数1
        bool	mbDI2;	//数字量输入参数2
        bool	mbDI3;	//数字量输入参数3
        bool	mbDI4;	//数字量输入参数4
        bool	mbDO1;	//数字量输出参数1
        bool	mbDO2;	//数字量输出参数2
        bool	mbDO3;	//数字量输出参数3
        bool	mbDO4;	//数字量输出参数4
		
        bool	mbSalve1DI1;	//目标从站1的数字量输入参数1
        bool	mbSalve1DI2;	//目标从站1的数字量输入参数2
        bool	mbSalve1DI3;	//目标从站1的数字量输入参数3
        bool	mbSalve1DI4;	//目标从站1的数字量输入参数4
        bool	mbSalve1DO1;	//目标从站1的数字量输出参数1
        bool	mbSalve1DO2;	//目标从站1的数字量输出参数2
        bool	mbSalve1DO3;	//目标从站1的数字量输出参数3
        bool	mbSalve1DO4;	//目标从站1的数字量输出参数4
		
        bool	mbSalve2DI1;	//目标从站2的数字量输入参数1
        bool	mbSalve2DI2;	//目标从站2的数字量输入参数2
        bool	mbSalve2DI3;	//目标从站2的数字量输入参数3
        bool	mbSalve2DI4;	//目标从站2的数字量输入参数4
        bool	mbSalve2DO1;	//目标从站2的数字量输出参数1
        bool	mbSalve2DO2;	//目标从站2的数字量输出参数2
        bool	mbSalve2DO3;	//目标从站2的数字量输出参数3
        bool	mbSalve2DO4;	//目标从站2的数字量输出参数4
		
        bool	mbSalve3DI1;	//目标从站3的数字量输入参数1
        bool	mbSalve3DI2;	//目标从站3的数字量输入参数2
        bool	mbSalve3DI3;	//目标从站3的数字量输入参数3
        bool	mbSalve3DI4;	//目标从站3的数字量输入参数4
        bool	mbSalve3DO1;	//目标从站3的数字量输出参数1
        bool	mbSalve3DO2;	//目标从站3的数字量输出参数2
        bool	mbSalve3DO3;	//目标从站3的数字量输出参数3
        bool	mbSalve3DO4;	//目标从站3的数字量输出参数4
		
        bool	mbSalve4DI1;	//目标从站4的数字量输入参数1
        bool	mbSalve4DI2;	//目标从站4的数字量输入参数2
        bool	mbSalve4DI3;	//目标从站4的数字量输入参数3
        bool	mbSalve4DI4;	//目标从站4的数字量输入参数4
        bool	mbSalve4DO1;	//目标从站4的数字量输出参数1
        bool	mbSalve4DO2;	//目标从站4的数字量输出参数2
        bool	mbSalve4DO3;	//目标从站4的数字量输出参数3
        bool	mbSalve4DO4;	//目标从站4的数字量输出参数4
        
    }phyPara;
    bool coil[CoilEndAddress+1];         /*线圈量*/
}DigitalParaTypeDef;




void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)

#endif

/*********** (C) COPYRIGHT 1999-2018 Moonan Technology *********END OF FILE****/