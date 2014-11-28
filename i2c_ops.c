#include "i2c_ops.h"
#include "stm32f4xx_i2c.h"

#define SLAVE_ADDRESS 0x42

/* This function issues a start condition and
 * transmits the slave address + R/W bit
 *
 * Parameters:
 * 		I2Cx --> the I2C peripheral e.g. I2C1
 * 		address --> the 7 bit slave address
 * 		direction --> the transmission direction can be:
 * 						I2C_Direction_Tranmitter for Master transmitter mode
 * 						I2C_Direction_Receiver for Master receiver
 */

void Delay(uint32_t volatile DelayTime_uS)
{
	uint32_t DelayTime = 0;
	DelayTime = (SystemCoreClock/10000)*DelayTime_uS;
	for (; DelayTime!=0; DelayTime--);
}

void I2C_start(I2C_TypeDef* I2Cx, uint8_t address, uint8_t direction)
{
	// wait until I2C1 is not busy anymore
	while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY));
	// Send I2C1 START condition
	I2C_GenerateSTART(I2Cx, ENABLE);
	// wait for I2C1 EV5 --> Slave has acknowledged start condition
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));
	// Send slave Address for write
	I2C_Send7bitAddress(I2Cx, address, direction);
	/* wait for I2C1 EV6, check if
	 * either Slave has acknowledged Master transmitter or
	 * Master receiver mode, depending on the transmission
	 * direction
	 */
	if(direction == I2C_Direction_Transmitter)
	{
		while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	}
	else if(direction == I2C_Direction_Receiver)
	{
		while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
	}
}


/* This function transmits one byte to the slave device
 * Parameters:
 *		I2Cx --> the I2C peripheral e.g. I2C1
 *		data --> the data byte to be transmitted
 */
void I2C_write(I2C_TypeDef* I2Cx, uint8_t data)
{
	I2C_SendData(I2Cx, data);
	// wait for I2C1 EV8_2 --> byte has been transmitted
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
}

/* This function reads one byte from the slave device
 * and acknowledges the byte (requests another byte)
 */
uint8_t I2C_read_ack(I2C_TypeDef* I2Cx){
	uint8_t data;
	// enable acknowledge of received data
	I2C_AcknowledgeConfig(I2Cx, ENABLE);
	// wait until one byte has been received
	while( !I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED) );
	// read data from I2C data register and return data byte
	data = I2C_ReceiveData(I2Cx);
	return data;
}


/* This function reads one byte from the slave device
 * and doesn't acknowledge the received data
 */
uint8_t I2C_read_nack(I2C_TypeDef* I2Cx){
	uint8_t data;
	// disable acknowledge of received data
	I2C_AcknowledgeConfig(I2Cx, DISABLE);
	// wait until one byte has been received
	while( !I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED) );
	// read data from I2C data register and return data byte
	data = I2C_ReceiveData(I2Cx);
	return data;
}


/* This function issues a stop condition and therefore
 * releases the bus
 */
void I2C_stop(I2C_TypeDef* I2Cx){
	// Send I2C1 STOP Condition
	I2C_GenerateSTOP(I2Cx, ENABLE);
}


uint8_t I2C_writereg(uint8_t reg, uint8_t data)
{
	uint8_t tmp;

	I2C_start(I2C1, SLAVE_ADDRESS, I2C_Direction_Transmitter); // start a transmission in Master transmitter mode
	I2C_write(I2C1, reg); // write one byte to the slave
	Delay(100);
	I2C_write(I2C1, data); // write one byte to the slave
	I2C_stop(I2C1); // stop the transmission
//	Delay(100);
	I2C_start(I2C1, SLAVE_ADDRESS, I2C_Direction_Receiver); // start a transmission in Master receiver mode
	tmp = I2C_read_nack(I2C1);
	I2C_stop(I2C1); // stop the transmission
//	Delay(100);

	return tmp;
}


uint8_t I2C_readreg(uint8_t reg)
{
	uint8_t tmp;

	I2C_start(I2C1, SLAVE_ADDRESS, I2C_Direction_Transmitter); // start a transmission in Master transmitter mode
	I2C_write(I2C1, reg); // write one byte to the slave
	I2C_stop(I2C1); // stop the transmission

//	Delay(100);

	I2C_start(I2C1, SLAVE_ADDRESS, I2C_Direction_Receiver); // start a transmission in Master receiver mode
	tmp = I2C_read_nack(I2C1);
	I2C_stop(I2C1); // stop the transmission

//	Delay(100);

	return tmp;
}
