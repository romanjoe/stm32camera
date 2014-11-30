#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_i2c.h"
#include "stm32f4xx_dcmi.h"
#include "stm32f4xx_dma.h"
#include "i2c_ops.h"
#include "stm32f4xx.h"
#include "misc.h"

volatile int32_t dma_handler_counter = 0;

uint32_t camera_frame[800]; // for 320*240 resolution

void camera_init(void)
{
    RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_DCMI, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    /* Clocking of  GPIOB is enabled in i2c_init function */

    /* PA4 - HSYNC
       PA6 - PIXCLK
       PB7 - DCMI_VSYNC
       PC6 - DCMI_D0
       PC7 - DCMI_D1
       PC8 - DCMI_D2
       PC9 - DCMI_D3
       PC11 - DCMI_D4
       PB6 - DCMI_D5
       PB8 - DCMI_D6
       PB9 - DCMI_D7
     */
    /* set  alternate function mode for pins */
    GPIOA->MODER |= (uint32_t) 0x00002200;
    GPIOB->MODER |= (uint32_t) 0x0000AA00;
    GPIOC->MODER |= (uint32_t) 0x0008AA00;
    /* set speed 50MHz for pins */
    GPIOA->OTYPER |= (uint32_t) 0x00002200;
    GPIOB->OTYPER |= (uint32_t) 0x0000AA00;
    GPIOC->OTYPER |= (uint32_t) 0x0008AA00;
    /* set type of alternative function  */
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource4, GPIO_AF_DCMI);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_DCMI);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_DCMI);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_DCMI);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_DCMI);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_DCMI);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_DCMI);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource8, GPIO_AF_DCMI);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource9, GPIO_AF_DCMI);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_DCMI);

    DCMI_InitTypeDef camera_init;

    camera_init.DCMI_CaptureMode = DCMI_CaptureMode_SnapShot;
    camera_init.DCMI_CaptureRate = DCMI_CaptureRate_All_Frame;
    camera_init.DCMI_ExtendedDataMode = DCMI_ExtendedDataMode_8b;
    camera_init.DCMI_HSPolarity = DCMI_HSPolarity_High;
    camera_init.DCMI_PCKPolarity = DCMI_PCKPolarity_Rising;
    camera_init.DCMI_SynchroMode = DCMI_SynchroMode_Hardware;
    camera_init.DCMI_VSPolarity = DCMI_VSPolarity_High;

    DCMI_StructInit(&camera_init);
    // enable DCMI interface
    DCMI_Cmd(ENABLE);

    NVIC_InitTypeDef camera_irq;
    camera_irq.NVIC_IRQChannel = DCMI_IRQn;
    camera_irq.NVIC_IRQChannelPreemptionPriority = 0;
    camera_irq.NVIC_IRQChannelSubPriority = 1;
    camera_irq.NVIC_IRQChannelCmd = ENABLE;

    NVIC_Init(&camera_irq);

    DCMI_ITConfig(DCMI_IT_ERR | DCMI_IT_FRAME | DCMI_IT_LINE |
                  DCMI_IT_OVF | DCMI_IT_VSYNC, ENABLE);
}

void i2c_init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);

    I2C_InitTypeDef i2c_init;

    i2c_init.I2C_Ack = I2C_Ack_Enable;
    i2c_init.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    i2c_init.I2C_ClockSpeed = 100000;
    i2c_init.I2C_DutyCycle = I2C_DutyCycle_2;
    i2c_init.I2C_Mode = I2C_Mode_I2C;
    i2c_init.I2C_OwnAddress1 = 21;

    I2C_Init(I2C2, &i2c_init);
    // enable I2C interface
    I2C_Cmd(I2C2, ENABLE);
}

void dma_init(void)
{
    DMA_DeInit(DMA2_Stream1);

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);

    DMA_InitTypeDef dma_init;

    dma_init.DMA_Channel = DMA_Channel_1;
    dma_init.DMA_PeripheralBaseAddr = DCMI_BASE;
    dma_init.DMA_Memory0BaseAddr = (uint32_t) camera_frame;
    dma_init.DMA_DIR = DMA_DIR_PeripheralToMemory;
    dma_init.DMA_BufferSize = 800;
    dma_init.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    dma_init.DMA_MemoryInc = DMA_MemoryInc_Enable;
    dma_init.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
    dma_init.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
    dma_init.DMA_Mode = DMA_Mode_Normal;
    dma_init.DMA_Priority = DMA_Priority_High;
    dma_init.DMA_FIFOMode = DMA_FIFOMode_Enable;
    dma_init.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
    dma_init.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    dma_init.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;

    DMA_Init(DMA2_Stream1, &dma_init);

    DMA_ITConfig(DMA2_Stream1, DMA_IT_TC | DMA_IT_FE, ENABLE);

    NVIC_InitTypeDef dma_stream1_irq;

    dma_stream1_irq.NVIC_IRQChannel = DMA2_Stream1_IRQn;
    dma_stream1_irq.NVIC_IRQChannelPreemptionPriority = 0;
    dma_stream1_irq.NVIC_IRQChannelSubPriority = 0;
    dma_stream1_irq.NVIC_IRQChannelCmd = ENABLE;

    NVIC_Init(&dma_stream1_irq);
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
}

void DMA2_Stream1_IRQHandler(void)
{
    if(DMA_GetITStatus(DMA2_Stream1, DMA_IT_TCIF1) != RESET)
    {
        dma_handler_counter++;
        DMA_ClearITPendingBit(DMA2_Stream1, DMA_IT_TCIF1);
    }
}
int main(void)
{
    uint8_t c = 0;
    SystemInit();

    RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd (RCC_APB2Periph_SYSCFG, ENABLE);

    GPIO_InitTypeDef GPIOBInitStructure;

    GPIOBInitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
    GPIOBInitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIOBInitStructure.GPIO_OType = GPIO_OType_OD;
    GPIOBInitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIOBInitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init (GPIOB, &GPIOBInitStructure);

    GPIO_PinAFConfig (GPIOB, GPIO_PinSource10, GPIO_AF_I2C2);    // I2C2_SCL
    GPIO_PinAFConfig (GPIOB, GPIO_PinSource11, GPIO_AF_I2C2);    // I2C2_SDA

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

    GPIO_InitTypeDef mco_Init;

    mco_Init.GPIO_Pin = GPIO_Pin_8;
    mco_Init.GPIO_Mode = GPIO_Mode_AF;
    mco_Init.GPIO_Speed = GPIO_Speed_100MHz;
    mco_Init.GPIO_OType = GPIO_OType_PP;

    GPIO_Init(GPIOA, &mco_Init);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_MCO);

    GPIO_InitTypeDef GPIOD_Init;

    GPIOD_Init.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15;
    GPIOD_Init.GPIO_Mode = GPIO_Mode_OUT;
    GPIOD_Init.GPIO_OType = GPIO_OType_PP;
    GPIOD_Init.GPIO_Speed = GPIO_Speed_100MHz;

    GPIO_Init(GPIOD, &GPIOD_Init);
    i2c_init();
    camera_init();
    dma_init();

    c = I2C_readreg(0x0a);
    if(c == 0x76)
        GPIO_SetBits(GPIOD, GPIO_Pin_13);
    DMA_Cmd(DMA2_Stream2, ENABLE);

    while(1)
    {
        if(dma_handler_counter == 800)
            GPIO_SetBits(GPIOD, GPIO_Pin_15);
    }

    return 0;
}
