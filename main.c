#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_i2c.h"
#include "stm32f4xx_dcmi.h"
#include "i2c_ops.h"
#include "stm32f4xx.h"

void i2c_init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

    I2C_InitTypeDef i2c_init;
    I2C_StructInit(&i2c_init);

    i2c_init.I2C_Ack = I2C_Ack_Enable;
    i2c_init.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    i2c_init.I2C_ClockSpeed = 100000;
    i2c_init.I2C_DutyCycle = I2C_DutyCycle_2;
    i2c_init.I2C_Mode = I2C_Mode_I2C;
    i2c_init.I2C_OwnAddress1 = 21;
    I2C_Init(I2C1, &i2c_init);

    I2C_Cmd (I2C1, ENABLE);
}

void camera_init(void)
{
    RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_DCMI, ENABLE);

    DCMI_InitTypeDef camera_init;

    camera_init.DCMI_CaptureMode = DCMI_CaptureMode_SnapShot;
    camera_init.DCMI_CaptureRate = DCMI_CaptureRate_All_Frame;
    camera_init.DCMI_ExtendedDataMode = DCMI_ExtendedDataMode_8b;
    camera_init.DCMI_HSPolarity = DCMI_HSPolarity_High;
    camera_init.DCMI_PCKPolarity = DCMI_PCKPolarity_Rising;
    camera_init.DCMI_SynchroMode = DCMI_SynchroMode_Hardware;
    camera_init.DCMI_VSPolarity = DCMI_VSPolarity_High;

    DCMI_StructInit(&camera_init);
}

int main(void)
{
    uint8_t c = 0;
    SystemInit();

    RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd (RCC_APB2Periph_SYSCFG, ENABLE);

    GPIO_InitTypeDef GPIOBInitStructure;
    GPIO_StructInit (&GPIOBInitStructure);

    GPIOBInitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIOBInitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIOBInitStructure.GPIO_OType = GPIO_OType_OD;
    GPIOBInitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIOBInitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init (GPIOB, &GPIOBInitStructure);

    GPIO_PinAFConfig (GPIOB, GPIO_PinSource6, GPIO_AF_I2C1);    // I2C1_SCL
    GPIO_PinAFConfig (GPIOB, GPIO_PinSource7, GPIO_AF_I2C1);    // I2C1_SDA

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

    GPIOD_Init.GPIO_Pin = GPIO_Pin_13;
    GPIOD_Init.GPIO_Mode = GPIO_Mode_OUT;
    GPIOD_Init.GPIO_OType = GPIO_OType_PP;
    GPIOD_Init.GPIO_Speed = GPIO_Speed_100MHz;

    GPIO_Init(GPIOD, &GPIOD_Init);
    i2c_init();
    c = I2C_readreg(0x0a);
    if(c == 0x76)
        GPIO_SetBits(GPIOD, GPIO_Pin_13);

    while(1);

    return 0;
}
