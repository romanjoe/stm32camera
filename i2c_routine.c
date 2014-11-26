#include "stm32f4xx_i2c.h"

void delay_ms(uint32_t volatile delay_time_ms)
{
    uint32_t delay_time = 0;

    delay_time = (SystemCoreClock / 10000) * delay_time_ms;

    for (; delay_time != 0; delay_time--);
}

/**
 *
 * @param I2Cx - I2C peripheral module number
 * @param I2C_Addr - I2C SLAVE address in network
 * @param addr - slave register address
 * @param buf  - pointer to receiver buffer
 * @param num  - number of bytes to receive
 * @return buffer -
 *
 * @bug Function now works not actually write. input buffer, pointer on which must be used (*buf) do not used.
 *      Instead of this, function is used with *buf == 0, and returns 1 byte via uint8_t buffer
 *
 *      Need to be optimized!!!!!
 */

uint8_t I2C_Read(I2C_TypeDef *I2Cx, uint8_t I2C_Addr, uint8_t addr, uint8_t *buf, uint16_t num)
{
    uint8_t buffer;

    if(num == 0)
        return 1;
    // check is the line is busy
    while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY));
    // enable acknowledgement
    I2C_AcknowledgeConfig(I2Cx, ENABLE);
    // generate start condition
    I2C_GenerateSTART(I2Cx, ENABLE);
    // check if MASTER mode for STM32 is selected
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));
    // send SLAVE I2C address to SLAVE with TRANSMITION MODE SELECT bit
    I2C_Send7bitAddress(I2Cx, I2C_Addr, I2C_Direction_Transmitter);
    // check if TRANSMITTER MODE SELECTED for STM32
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
    // transmit SLAVE's register address to read byte
    I2C_SendData(I2Cx, addr);
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    // check if the address byte transmitted - THIS IS THE END of TRANSMITION
    // generate start event for receiving
    I2C_GenerateSTART(I2Cx, ENABLE);
    // check if MASTER mode for STM32 is selected
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));
    // send self address to slave with RECEPTION MODE SELECT bit
    I2C_Send7bitAddress(I2Cx, I2C_Addr, I2C_Direction_Receiver);
    // check if RECEIVER MODE SELECTED for STM32
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

    while (num)
    { // while number of bytes to receive (num) != 0

        if(num == 1)
        {
            // disable acknowledgement
            I2C_AcknowledgeConfig(I2Cx, DISABLE);
            // and generate end of communication
            I2C_GenerateSTOP(I2Cx, ENABLE);
        }
        // check if the next byte is received
        while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED));  /* EV7 */
        /* white to reception Buffer
         * and increment the index of the buffer in case of
         * multibytes reception operation
         * decrement the read bytes counter
         */
        buffer = I2C_ReceiveData(I2Cx);
        // buf++;																	//
                                                                                    //
        num--;																		//
    }

    I2C_AcknowledgeConfig(I2Cx, ENABLE);											// enable acknowledge for next transmition session

    return buffer;																	// return reception buffer
}

/**
 * @brief Function implements transmission of one byte via I2C
 *
 * @param I2Cx      - I2C peripheral module number
 * @param I2C_Addr  - I2C SLAVE address in network
 * @param addr      - slave register address
 * @param value		- byte to transmit
 * @return 0
 */

uint8_t I2C_WriteOneByte(I2C_TypeDef *I2Cx, uint8_t I2C_Addr, uint8_t addr, uint8_t value)
{

    while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY));

//	I2C_AcknowledgeConfig(I2Cx, ENABLE);

    // generate start condition by master
    I2C_GenerateSTART(I2Cx, ENABLE);
    // check if MASTER MODE selected for STM32
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));
    // send SLAVE I2C address to SLAVE with TRANSMITION MODE SELECT bit
    I2C_Send7bitAddress(I2Cx, I2C_Addr, I2C_Direction_Transmitter);
    // check if TRANSMITTER MODE SELECTED for STM32
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
    // send register to write address in slave device
    I2C_SendData(I2Cx, addr);
    // check if register to write address transmitted
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    // send dedicated byte to slave, addressed register
    I2C_SendData(I2Cx, value);
    // check if register to write address transmitted
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    // generate STOP event
    I2C_GenerateSTOP(I2Cx, ENABLE);

    return 0;
}

/**
 * @brief Function implements transmission of multiple bytes via I2C
 *
 * @param I2Cx		- I2C peripheral module number
 * @param I2C_Addr	- I2C SLAVE address in network
 * @param addr		- slave register address
 * @param buf		- transmission buffer
 * @param num		- number of bytes to transmit
 * @return 0
 */

uint8_t I2C_Write(I2C_TypeDef *I2Cx, uint8_t I2C_Addr, uint8_t addr, uint8_t *buf, uint16_t num)
{
    uint8_t err = 0;

    while(num--)
    {
        if(I2C_WriteOneByte(I2Cx, I2C_Addr, addr++, *buf++))
        {
            err++;
        }
    }

    if(err)
        return 1;
    else
        return 0;
}
