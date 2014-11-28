#ifndef _I2CROUTINES_H
#define _I2CROUTINES_H

#include "stm32f4xx.h"

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */

 /**
 * @brief Private functions prototypes
 */

 void I2C_start(I2C_TypeDef* I2Cx, uint8_t address, uint8_t direction);
 void I2C_write(I2C_TypeDef* I2Cx, uint8_t data);
 uint8_t I2C_read_ack(I2C_TypeDef* I2Cx);
 uint8_t I2C_read_nack(I2C_TypeDef* I2Cx);
 void I2C_stop(I2C_TypeDef* I2Cx);
 uint8_t I2C_readreg(uint8_t reg);
 uint8_t I2C_writereg(uint8_t reg, uint8_t data);
 uint8_t I2C_readreg(uint8_t reg);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*_I2CROUTINES_H*/
