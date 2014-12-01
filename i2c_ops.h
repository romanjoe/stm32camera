#ifndef _I2CROUTINES_H
#define _I2CROUTINES_H

#include "stm32f4xx.h"

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */


#define i2c_dev I2C2
 /**
 * @brief Private functions prototypes
 */
 void Delay(uint32_t volatile DelayTime_uS);
 void I2C_start(I2C_TypeDef* I2Cx, uint8_t address, uint8_t direction);
 void I2C_write(I2C_TypeDef* I2Cx, uint8_t data);
 uint8_t I2C_read_ack(I2C_TypeDef* I2Cx);
 uint8_t I2C_read_nack(I2C_TypeDef* I2Cx);
 void I2C_stop(I2C_TypeDef* I2Cx);
 uint8_t I2C_writereg(uint32_t reg, uint32_t data);
 uint8_t I2C_readreg(uint32_t reg);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*_I2CROUTINES_H*/
