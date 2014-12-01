#include "ov7670.h"
#include "ov7670_regsmap.h"
#include "i2c_ops.h"

uint32_t ov7670_init()
{
	int hstart = 456, hstop = 24, vstart = 14, vstop = 494;
	unsigned char v;

	if (I2C_readreg(REG_PID) != 0x76) {
		return 1;
	}
	I2C_writereg(REG_COM7, COM7_RESET); /* reset to default values */
	I2C_writereg(REG_CLKRC, CLK_EXT);
	I2C_writereg(REG_COM7, COM7_FMT_VGA | COM7_RGB); /* output format: RGB */

	I2C_writereg(REG_HSTART, (hstart >> 3) & 0xff);
	I2C_writereg(REG_HSTOP, (hstop >> 3) & 0xff);
	v = I2C_readreg(REG_HREF);
	v = (v & 0xc0) | ((hstop & 0x7) << 3) | (hstart & 0x7);
	I2C_writereg(REG_HREF, v);

	I2C_writereg(REG_VSTART, (vstart >> 2) & 0xff);
	I2C_writereg(REG_VSTOP, (vstop >> 2) & 0xff);
	v = I2C_readreg(REG_VREF);
	v = (v & 0xf0) | ((vstop & 0x3) << 2) | (vstart & 0x3);
	I2C_writereg(REG_VREF, v);

	I2C_writereg(REG_COM3, COM3_SCALEEN | COM3_DCWEN);
	I2C_writereg(REG_COM14, COM14_DCWEN | 0x01);
	I2C_writereg(0x73, 0xf1);
	I2C_writereg(0xa2, 0x52);
	I2C_writereg(0x7b, 0x1c);
	I2C_writereg(0x7c, 0x28);
	I2C_writereg(0x7d, 0x3c);
	I2C_writereg(0x7f, 0x69);
	I2C_writereg(REG_COM9, 0x38);
	I2C_writereg(0xa1, 0x0b);
	I2C_writereg(0x74, 0x19);
	I2C_writereg(0x9a, 0x80);
	I2C_writereg(0x43, 0x14);
	I2C_writereg(REG_COM13, 0xc0);
	I2C_writereg(0x70, 0x3A);
	I2C_writereg(0x71, 0x35);
	I2C_writereg(0x72, 0x11);

	/* Gamma curve values */
	I2C_writereg(0x7a, 0x20);
	I2C_writereg(0x7b, 0x10);
	I2C_writereg(0x7c, 0x1e);
	I2C_writereg(0x7d, 0x35);
	I2C_writereg(0x7e, 0x5a);
	I2C_writereg(0x7f, 0x69);
	I2C_writereg(0x80, 0x76);
	I2C_writereg(0x81, 0x80);
	I2C_writereg(0x82, 0x88);
	I2C_writereg(0x83, 0x8f);
	I2C_writereg(0x84, 0x96);
	I2C_writereg(0x85, 0xa3);
	I2C_writereg(0x86, 0xaf);
	I2C_writereg(0x87, 0xc4);
	I2C_writereg(0x88, 0xd7);
	I2C_writereg(0x89, 0xe8);

	/* AGC and AEC parameters.  Note we start by disabling those features,
	 then turn them only after tweaking the values. */
	I2C_writereg(REG_COM8, COM8_FASTAEC | COM8_AECSTEP | COM8_BFILT);
	I2C_writereg(REG_GAIN, 0);
	I2C_writereg(REG_AECH, 0);
	I2C_writereg(REG_COM4, 0x40); /* magic reserved bit */
	I2C_writereg(REG_COM9, 0x18); /* 4x gain + magic rsvd bit */
	I2C_writereg(REG_BD50MAX, 0x05);
	I2C_writereg(REG_BD60MAX, 0x07);
	I2C_writereg(REG_AEW, 0x95);
	I2C_writereg(REG_AEB, 0x33);
	I2C_writereg(REG_VPT, 0xe3);
	I2C_writereg(REG_HAECC1, 0x78);
	I2C_writereg(REG_HAECC2, 0x68);
	I2C_writereg(0xa1, 0x03); /* magic */
	I2C_writereg(REG_HAECC3, 0xd8);
	I2C_writereg(REG_HAECC4, 0xd8);
	I2C_writereg(REG_HAECC5, 0xf0);
	I2C_writereg(REG_HAECC6, 0x90);
	I2C_writereg(REG_HAECC7, 0x94);
	I2C_writereg(REG_COM8,
			COM8_FASTAEC | COM8_AECSTEP | COM8_BFILT | COM8_AGC | COM8_AEC);

	/* Almost all of these are magic "reserved" values.  */
	I2C_writereg(REG_COM5, 0x61);
	I2C_writereg(REG_COM6, 0x4b);
	I2C_writereg(0x16, 0x02);
	I2C_writereg(REG_MVFP, 0x07);
	I2C_writereg(0x21, 0x02);
	I2C_writereg(0x22, 0x91);
	I2C_writereg(0x29, 0x07);
	I2C_writereg(0x33, 0x0b);
	I2C_writereg(0x35, 0x0b);
	I2C_writereg(0x37, 0x1d);
	I2C_writereg(0x38, 0x71);
	I2C_writereg(0x39, 0x2a);
	I2C_writereg(REG_COM12, 0x78);
	I2C_writereg(0x4d, 0x40);
	I2C_writereg(0x4e, 0x20);
	I2C_writereg(REG_GFIX, 0);
	I2C_writereg(0x6b, 0x4a);
	I2C_writereg(0x74, 0x10);
	I2C_writereg(0x8d, 0x4f);
	I2C_writereg(0x8e, 0);
	I2C_writereg(0x8f, 0);
	I2C_writereg(0x90, 0);
	I2C_writereg(0x91, 0);
	I2C_writereg(0x96, 0);
	I2C_writereg(0x9a, 0);
	I2C_writereg(0xb0, 0x84);
	I2C_writereg(0xb1, 0x0c);
	I2C_writereg(0xb2, 0x0e);
	I2C_writereg(0xb3, 0x82);
	I2C_writereg(0xb8, 0x0a);

	/* Matrix coefficients */
	I2C_writereg(0x4f, 0x80);
	I2C_writereg(0x50, 0x80);
	I2C_writereg(0x51, 0);
	I2C_writereg(0x52, 0x22);
	I2C_writereg(0x53, 0x5e);
	I2C_writereg(0x54, 0x80);
	I2C_writereg(0x58, 0x9e);

	/* More reserved magic, some of which tweaks white balance */
	I2C_writereg(0x43, 0x0a);
	I2C_writereg(0x44, 0xf0);
	I2C_writereg(0x45, 0x34);
	I2C_writereg(0x46, 0x58);
	I2C_writereg(0x47, 0x28);
	I2C_writereg(0x48, 0x3a);
	I2C_writereg(0x59, 0x88);
	I2C_writereg(0x5a, 0x88);
	I2C_writereg(0x5b, 0x44);
	I2C_writereg(0x5c, 0x67);
	I2C_writereg(0x5d, 0x49);
	I2C_writereg(0x5e, 0x0e);
	I2C_writereg(0x6c, 0x0a);
	I2C_writereg(0x6d, 0x55);
	I2C_writereg(0x6e, 0x11);
	I2C_writereg(0x6f, 0x9f); /* "9e for advance AWB" */
	I2C_writereg(0x6a, 0x40);
	I2C_writereg(REG_BLUE, 0x40);
	I2C_writereg(REG_RED, 0x60);
	I2C_writereg(REG_COM8,
			COM8_FASTAEC | COM8_AECSTEP | COM8_BFILT | COM8_AGC | COM8_AEC
					| COM8_AWB);
	return 0;
}
