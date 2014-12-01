/* This file is defining functions declarations
 * for manupulating OV7670 camera sensor
 */

#ifndef _OV7670_H_
#define _OV7670_H_

#include "i2c_ops.h"


#define ov7670_cntr_port GPIOB
#define rst		GPIO_Pin_11
#define href	GPIO_Pin_8	/* horizontal sync */
#define vsync	GPIO_Pin_9	/* vertical sync */
#define sda 	GPIO_Pin_6
#define scl 	GPIO_Pin_7
#define pclk	GPIO_Pin_5 /* pwm signal must be applied  */
#define xclk	GPIO_Pin_4 /* pwm signal must be applied  */

#define ov7670_port		 GPIOE

#define db0		GPIO_Pin_0
#define db1		GPIO_Pin_1
#define db2		GPIO_Pin_2
#define db3		GPIO_Pin_3
#define db4		GPIO_Pin_4
#define db5		GPIO_Pin_5
#define db6		GPIO_Pin_6
#define db7		GPIO_Pin_7

#define ov7670_i2c_rd_addr 0x42
#define ov7670_i2c_wr_addr 0x43

void delay_ms(uint32_t volatile delay_time_ms);
void ov7670_capture_next(void);
FlagStatus ov7670_capture_done(void);
void ov7670_write_reg(int32_t addr, int32_t data);
int32_t ov7670_read_reg(int32_t addr);
void ov7670_reset(void);
uint32_t ov7670_init();
void ov7670_vsync_handler(void);
void ov7670_href_handler(void);

#endif /* _OV7670_H_ */
