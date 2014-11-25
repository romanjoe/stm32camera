/* This file is defining functions declarations
 * for manupulating OV7670 camera sensor
 */

#include "i2c_routines.h"

#define SDA GPIO_Pin_6
#define SCL GPIO_Pin_7


void ov7670_capture_next(void);
FlagStatus ov7670_capture_done(void);
void ov7670_write_reg(int32_t addr, int32_t data);
int32_t ov7670_read_reg(int32_t addr);
void ov7670_reset(void);
int32_t ov7670_init(uint8_t * format, int32_t n);
void ov7670_vsync_handler(void);
void ov7670_href_handler(void);
