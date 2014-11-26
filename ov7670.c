#include "ov7670.h"
#include "i2c_routines.h"

#define write_reg(reg_addr, data) (I2C_WriteOneByte(I2C1, ov7670_i2c_wr_addr, \
                                                    (reg_addr), (data)) )


int32_t ov7670_init(uint8_t * format, int32_t n)
{
        ov7670_reset();
        ov7670_reset();



        return 0;
}


void ov7670_reset(void)
{
        /* 0x12 is CR7 reg of ov7670 */
        write_reg(0x12, 0x80);
        delay_ms(200);
        delay_ms(200);

        write_reg(REG_RGB444, 0x00);
        write_reg(REG_COM10, 0x02); // vsync negative

        write_reg(REG_MVFP, 0x27);
        write_reg(REG_CLKRC); // prescaler x1
        write_reg(DBLV, 0x0a); // bypass pll

        write_reg(REG_COM11, 0x0A);
        write_reg(REG_TSLB, 0x04);  // 0D == UYVY 04 == YUYV
        write_reg(REG_COM13, 0x88); // connect to REG_TSLB


}
