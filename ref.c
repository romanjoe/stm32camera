#include "ov7670.h"


OV7670::OV7670(PinName sda, PinName scl, PinName vs, PinName hr, PinName we, PortName port, int mask, PinName rt, PinName o, PinName rc) : _i2c(sda,scl),vsync(vs),href(hr),wen(we),data(port,mask),rrst(rt),oe(o),rclk(rc)
{
        _i2c.stop();
        _i2c.frequency(OV7670_I2CFREQ);
        vsync.fall(this,&OV7670::VsyncHandler);     // interrupt fall edge
        CaptureReq = false;
        Busy = false;
        Done = false;
        LineCounter = 0;
        rrst = 1;
        oe = 1;
        rclk = 1;
        wen = 0;
}


OV7670::~OV7670()
{

}


// capture request
void OV7670::CaptureNext(void)
{
    CaptureReq = true;
    Busy = true;
}


// capture done? (with clear)
bool OV7670::CaptureDone(void)
{
    bool result;
    if (Busy)
    {
        result = false;
    }else
    {
        result = Done;
        Done = false;
    }
    return result;
}


// vsync handler
void OV7670::VsyncHandler(void)
{
    // Capture Enable
    if (CaptureReq)
    {
        wen = 1;
        Done = false;
        CaptureReq = false;
    }else
    {
        wen = 0;
        if (Busy)
        {
            Busy = false;
            Done = true;
        }
    }
}


// Data Read
int OV7670::ReadOnebyte(void)
{
        int B1;
        rclk = 1;
        B1 = (((data&0x07800000)>>19)|((data&0x078000)>>15));
        rclk = 0;
        return B1;
}


// Data Start read from FIFO
void OV7670::ReadStart(void)
{
    rrst = 0;
    oe = 0;
    wait_us(1);
    rclk = 0;
    wait_us(1);
    rclk = 1;
    wait_us(1);
    rrst = 1;
}


// Data Stop read from FIFO
void OV7670::ReadStop(void)
{
    oe = 1;
    ReadOnebyte();
    rclk = 1;
}


void OV7670::Reset(void)
{
    WriteReg(0x12, 0x80);                  // RESET CAMERA
    wait_ms(200);
}


int OV7670::Init(char c, int n)
{
    if(c == 'b' || c == 'y')     // YUV
    {
        format_temp = "BAW";
    }
    else
    if(c == 'r')                // RGB565
    {
        format_temp = "RGB";
    }
    return Init(format_temp, n);
}


int OV7670::Init(char *format, int n)
{

    if (ReadReg(REG_PID) != 0x76)           // check id camera
    {
        return 0;
    }

    Reset();                                 // Resets all registers to default values
    Reset();                                 // Resets all registers to default values

    WriteReg(REG_RGB444, 0x00);              // Disable RGB444
    WriteReg(REG_COM10, 0x02);               // 0x02   VSYNC negative (http://nasulica.homelinux.org/?p=959)
    WriteReg(REG_MVFP, 0x27);                // mirror image

    WriteReg(REG_CLKRC, 0x80);               // prescaler x1
    WriteReg(DBLV, 0x0a);                    // bypass PLL

    WriteReg(REG_COM11, 0x0A) ;
    WriteReg(REG_TSLB, 0x04);                // 0D = UYVY  04 = YUYV
    WriteReg(REG_COM13, 0x88);               // connect to REG_TSLB


    if((strcmp("BAW", format) == 0) || (strcmp("YUV", format) == 0)|| (strcmp("RAW", format) == 0))     // YUV
    {
        WriteReg(REG_COM7, 0x00);           // YUV
        WriteReg(REG_COM17, 0x00);          // color bar disable
        WriteReg(REG_COM3, 0x04);
        WriteReg(REG_COM15, 0xC0);          // Set normal rgb with Full range

    }else
    if((strcmp("RGB", format) == 0))                // RGB565
    {
        WriteReg(REG_COM7, 0x04);           // RGB + color bar disable
        WriteReg(REG_RGB444, 0x00);         // Disable RGB444
        WriteReg(REG_COM15, 0x10);          // Set rgb565 with Full range    0xD0
        WriteReg(REG_COM3, 0x04);
        WriteReg(REG_CLKRC, 0x80);          // prescaler x1
    }

    WriteReg(0x70, 0x3A);                   // Scaling Xsc
    WriteReg(0x71, 0x35);                   // Scaling Ysc
    WriteReg(0xA2, 0x02);                   // pixel clock delay

    if(n == 19200)              // 160*120
    {
        WriteReg(REG_COM14, 0x1a);          // divide by 4
        WriteReg(0x72, 0x22);               // downsample by 4
        WriteReg(0x73, 0xf2);               // divide by 4
        WriteReg(REG_HREF, 0xa4);
        WriteReg(REG_HSTART, 0x16);
        WriteReg(REG_HSTOP, 0x04);
        WriteReg(REG_VREF, 0x0a);
        WriteReg(REG_VSTART, 0x02);
        WriteReg(REG_VSTOP, 0x7a);

        WriteReg(0x7a, 0x20);
        WriteReg(0x7b, 0x1c);
        WriteReg(0x7c, 0x28);
        WriteReg(0x7d, 0x3c);
        WriteReg(0x7e, 0x5a);
        WriteReg(0x7f, 0x68);
        WriteReg(0x80, 0x76);
        WriteReg(0x81, 0x80);
        WriteReg(0x82, 0x88);
        WriteReg(0x83, 0x8f);
        WriteReg(0x84, 0x96);
        WriteReg(0x85, 0xa3);
        WriteReg(0x86, 0xaf);
        WriteReg(0x87, 0xc4);
        WriteReg(0x88, 0xd7);
        WriteReg(0x89, 0xe8);

        WriteReg(0x13, 0xe0);
        WriteReg(0x00, 0x00);
        WriteReg(0x10, 0x00);
        WriteReg(0x0d, 0x40);
        WriteReg(0x14, 0x18);
        WriteReg(0xa5, 0x05);
        WriteReg(0xab, 0x07);
        WriteReg(0x24, 0x95);
        WriteReg(0x25, 0x33);
        WriteReg(0x26, 0xe3);
        WriteReg(0x9f, 0x78);
        WriteReg(0xa0, 0x68);
        WriteReg(0xa1, 0x03);
        WriteReg(0xa6, 0xd8);
        WriteReg(0xa7, 0xd8);
        WriteReg(0xa8, 0xf0);
        WriteReg(0xa9, 0x90);
        WriteReg(0xaa, 0x94);
        WriteReg(0x13, 0xe5);

        WriteReg(0x0e, 0x61);
        WriteReg(0x0f, 0x4b);
        WriteReg(0x16, 0x02);

        WriteReg(0x21, 0x02);
        WriteReg(0x22, 0x91);
        WriteReg(0x29, 0x07);
        WriteReg(0x33, 0x0b);
        WriteReg(0x35, 0x0b);
        WriteReg(0x37, 0x1d);
        WriteReg(0x38, 0x71);
        WriteReg(0x39, 0x2a);
        WriteReg(0x3c, 0x78);
        WriteReg(0x4d, 0x40);
        WriteReg(0x4e, 0x20);
        WriteReg(0x69, 0x00);

        WriteReg(0x74, 0x10);
        WriteReg(0x8d, 0x4f);
        WriteReg(0x8e, 0x00);
        WriteReg(0x8f, 0x00);
        WriteReg(0x90, 0x00);
        WriteReg(0x91, 0x00);
        WriteReg(0x92, 0x00);

        WriteReg(0x96, 0x00);
        WriteReg(0x9a, 0x80);
        WriteReg(0xb0, 0x84);
        WriteReg(0xb1, 0x0c);
        WriteReg(0xb2, 0x0e);
        WriteReg(0xb3, 0x82);
        WriteReg(0xb8, 0x0a);

        WriteReg(0x43, 0x0a);
        WriteReg(0x44, 0xf0);
        WriteReg(0x45, 0x34);
        WriteReg(0x46, 0x58);
        WriteReg(0x47, 0x28);
        WriteReg(0x48, 0x3a);
        WriteReg(0x59, 0x88);
        WriteReg(0x5a, 0x88);
        WriteReg(0x5b, 0x44);
        WriteReg(0x5c, 0x67);
        WriteReg(0x5d, 0x49);
        WriteReg(0x5e, 0x0e);
        WriteReg(0x64, 0x04);
        WriteReg(0x65, 0x20);
        WriteReg(0x66, 0x05);
        WriteReg(0x94, 0x04);
        WriteReg(0x95, 0x08);

        WriteReg(0x6c, 0x0a);
        WriteReg(0x6d, 0x55);
        WriteReg(0x6e, 0x11);
        WriteReg(0x6f, 0x9f);
        WriteReg(0x6a, 0x40);
        WriteReg(0x01, 0x40);
        WriteReg(0x02, 0x40);
        WriteReg(0x13, 0xe7);
        WriteReg(0x15, 0x02);

        WriteReg(0x4f, 0x80);
        WriteReg(0x50, 0x80);
        WriteReg(0x51, 0x00);
        WriteReg(0x52, 0x22);
        WriteReg(0x53, 0x5e);
        WriteReg(0x54, 0x80);
        WriteReg(0x58, 0x9e);

        WriteReg(0x41, 0x08);
        WriteReg(0x3f, 0x00);
        WriteReg(0x75, 0x05);
        WriteReg(0x76, 0xe1);
        WriteReg(0x4c, 0x00);
        WriteReg(0x77, 0x01);
        WriteReg(0x3d, 0xc1);
        WriteReg(0x4b, 0x09);
        WriteReg(0xc9, 0x60);
        WriteReg(0x41, 0x38);
        WriteReg(0x56, 0x40);

        WriteReg(0x34, 0x11);
        WriteReg(0x3b, 0x02);
        WriteReg(0xa4, 0x88);
        WriteReg(0x96, 0x00);
        WriteReg(0x97, 0x30);
        WriteReg(0x98, 0x20);
        WriteReg(0x99, 0x30);
        WriteReg(0x9a, 0x84);
        WriteReg(0x9b, 0x29);
        WriteReg(0x9c, 0x03);
        WriteReg(0x9d, 0x4c);
        WriteReg(0x9e, 0x3f);
        WriteReg(0x78, 0x04);

        WriteReg(0x79, 0x01);
        WriteReg(0xc8, 0xf0);
        WriteReg(0x79, 0x0f);
        WriteReg(0xc8, 0x00);
        WriteReg(0x79, 0x10);
        WriteReg(0xc8, 0x7e);
        WriteReg(0x79, 0x0a);
        WriteReg(0xc8, 0x80);
        WriteReg(0x79, 0x0b);
        WriteReg(0xc8, 0x01);
        WriteReg(0x79, 0x0c);
        WriteReg(0xc8, 0x0f);
        WriteReg(0x79, 0x0d);
        WriteReg(0xc8, 0x20);
        WriteReg(0x79, 0x09);
        WriteReg(0xc8, 0x80);
        WriteReg(0x79, 0x02);
        WriteReg(0xc8, 0xc0);
        WriteReg(0x79, 0x03);
        WriteReg(0xc8, 0x40);
        WriteReg(0x79, 0x05);
        WriteReg(0xc8, 0x30);
        WriteReg(0x79, 0x26);
        WriteReg(0x09, 0x03);
        WriteReg(0x3b, 0x42);

        WriteReg(0xff, 0xff);   /* END MARKER */

    }
    if(n == 76800)              // 320*240
    {
        WriteReg(REG_COM14, 0x19);
        WriteReg(0x72, 0x11);
        WriteReg(0x73, 0xf1);
        WriteReg(REG_HREF, 0x24);
        WriteReg(REG_HSTART, 0x16);
        WriteReg(REG_HSTOP, 0x04);
        WriteReg(REG_VREF, 0x0a);
        WriteReg(REG_VSTART,0x02);
        WriteReg(REG_VSTOP, 0x7a);

        WriteReg(0x7a, 0x20);
        WriteReg(0x7b, 0x1c);
        WriteReg(0x7c, 0x28);
        WriteReg(0x7d, 0x3c);
        WriteReg(0x7e, 0x55);
        WriteReg(0x7f, 0x68);
        WriteReg(0x80, 0x76);
        WriteReg(0x81, 0x80);
        WriteReg(0x82, 0x88);
        WriteReg(0x83, 0x8f);
        WriteReg(0x84, 0x96);
        WriteReg(0x85, 0xa3);
        WriteReg(0x86, 0xaf);
        WriteReg(0x87, 0xc4);
        WriteReg(0x88, 0xd7);
        WriteReg(0x89, 0xe8);

        WriteReg(0x13, 0xe0);
        WriteReg(0x00, 0x00);
        WriteReg(0x10, 0x00);
        WriteReg(0x0d, 0x00);
        WriteReg(0x14, 0x28);
        WriteReg(0xa5, 0x05);
        WriteReg(0xab, 0x07);
        WriteReg(0x24, 0x75);
        WriteReg(0x25, 0x63);
        WriteReg(0x26, 0xA5);
        WriteReg(0x9f, 0x78);
        WriteReg(0xa0, 0x68);
        WriteReg(0xa1, 0x03);
        WriteReg(0xa6, 0xdf);
        WriteReg(0xa7, 0xdf);
        WriteReg(0xa8, 0xf0);
        WriteReg(0xa9, 0x90);
        WriteReg(0xaa, 0x94);
        WriteReg(0x13, 0xe5);

        WriteReg(0x0e, 0x61);
        WriteReg(0x0f, 0x4b);
        WriteReg(0x16, 0x02);
        WriteReg(0x21, 0x02);
        WriteReg(0x22, 0x91);
        WriteReg(0x29, 0x07);
        WriteReg(0x33, 0x0b);
        WriteReg(0x35, 0x0b);
        WriteReg(0x37, 0x1d);
        WriteReg(0x38, 0x71);
        WriteReg(0x39, 0x2a);
        WriteReg(0x3c, 0x78);
        WriteReg(0x4d, 0x40);
        WriteReg(0x4e, 0x20);
        WriteReg(0x69, 0x00);
        WriteReg(0x6b, 0x00);
        WriteReg(0x74, 0x19);
        WriteReg(0x8d, 0x4f);
        WriteReg(0x8e, 0x00);
        WriteReg(0x8f, 0x00);
        WriteReg(0x90, 0x00);
        WriteReg(0x91, 0x00);
        WriteReg(0x92, 0x00);
        WriteReg(0x96, 0x00);
        WriteReg(0x9a, 0x80);
        WriteReg(0xb0, 0x84);
        WriteReg(0xb1, 0x0c);
        WriteReg(0xb2, 0x0e);
        WriteReg(0xb3, 0x82);
        WriteReg(0xb8, 0x0a);
        WriteReg(0x43, 0x14);
        WriteReg(0x44, 0xf0);
        WriteReg(0x45, 0x34);
        WriteReg(0x46, 0x58);
        WriteReg(0x47, 0x28);
        WriteReg(0x48, 0x3a);
        WriteReg(0x59, 0x88);
        WriteReg(0x5a, 0x88);
        WriteReg(0x5b, 0x44);
        WriteReg(0x5c, 0x67);
        WriteReg(0x5d, 0x49);
        WriteReg(0x5e, 0x0e);
        WriteReg(0x64, 0x04);
        WriteReg(0x65, 0x20);
        WriteReg(0x66, 0x05);
        WriteReg(0x94, 0x04);
        WriteReg(0x95, 0x08);
        WriteReg(0x6c, 0x0a);
        WriteReg(0x6d, 0x55);
        WriteReg(0x6e, 0x11);
        WriteReg(0x6f, 0x9f);
        WriteReg(0x6a, 0x40);
        WriteReg(0x01, 0x40);
        WriteReg(0x02, 0x40);
        WriteReg(0x13, 0xe7);
        WriteReg(0x15, 0x02);
        WriteReg(0x4f, 0x80);
        WriteReg(0x50, 0x80);
        WriteReg(0x51, 0x00);
        WriteReg(0x52, 0x22);
        WriteReg(0x53, 0x5e);
        WriteReg(0x54, 0x80);
        WriteReg(0x58, 0x9e);
        WriteReg(0x41, 0x08);
        WriteReg(0x3f, 0x00);
        WriteReg(0x75, 0x05);
        WriteReg(0x76, 0xe1);
        WriteReg(0x4c, 0x00);
        WriteReg(0x77, 0x01);
        WriteReg(0x3d, 0xc2);
        WriteReg(0x4b, 0x09);
        WriteReg(0xc9, 0x60);
        WriteReg(0x41, 0x38);
        WriteReg(0x56, 0x40);
        WriteReg(0x34, 0x11);
        WriteReg(0x3b, 0x02);
        WriteReg(0xa4, 0x89);
        WriteReg(0x96, 0x00);
        WriteReg(0x97, 0x30);
        WriteReg(0x98, 0x20);
        WriteReg(0x99, 0x30);
        WriteReg(0x9a, 0x84);
        WriteReg(0x9b, 0x29);
        WriteReg(0x9c, 0x03);
        WriteReg(0x9d, 0x4c);
        WriteReg(0x9e, 0x3f);
        WriteReg(0x78, 0x04);
        WriteReg(0x79, 0x01);
        WriteReg(0xc8, 0xf0);
        WriteReg(0x79, 0x0f);
        WriteReg(0xc8, 0x00);
        WriteReg(0x79, 0x10);
        WriteReg(0xc8, 0x7e);
        WriteReg(0x79, 0x0a);
        WriteReg(0xc8, 0x80);
        WriteReg(0x79, 0x0b);
        WriteReg(0xc8, 0x01);
        WriteReg(0x79, 0x0c);
        WriteReg(0xc8, 0x0f);
        WriteReg(0x79, 0x0d);
        WriteReg(0xc8, 0x20);
        WriteReg(0x79, 0x09);
        WriteReg(0xc8, 0x80);
        WriteReg(0x79, 0x02);
        WriteReg(0xc8, 0xc0);
        WriteReg(0x79, 0x03);
        WriteReg(0xc8, 0x40);
        WriteReg(0x79, 0x05);
        WriteReg(0xc8, 0x30);
        WriteReg(0x79, 0x26);
        WriteReg(0x09, 0x03);
        WriteReg(0x3b, 0x42);

        WriteReg(0xff, 0xff);   /* END MARKER */

    }
    if(n == 307200)             // 640*480
    {
        WriteReg(REG_CLKRC, 0x01);
        WriteReg(REG_TSLB,  0x04);
        WriteReg(REG_COM7, 0x01);
        WriteReg(DBLV, 0x4a);
        WriteReg(REG_COM3, 0);
        WriteReg(REG_COM14, 0);

        WriteReg(REG_HSTART, 0x13);
        WriteReg(REG_HSTOP, 0x01);
        WriteReg(REG_HREF, 0xb6);
        WriteReg(REG_VSTART, 0x02);
        WriteReg(REG_VSTOP, 0x7a);
        WriteReg(REG_VREF, 0x0a);
        WriteReg(0x72, 0x11);
        WriteReg(0x73, 0xf0);

        /* Gamma curve values */
        WriteReg(0x7a, 0x20);
        WriteReg(0x7b, 0x10);
        WriteReg(0x7c, 0x1e);
        WriteReg(0x7d, 0x35);
        WriteReg(0x7e, 0x5a);
        WriteReg(0x7f, 0x69);
        WriteReg(0x80, 0x76);
        WriteReg(0x81, 0x80);
        WriteReg(0x82, 0x88);
        WriteReg(0x83, 0x8f);
        WriteReg(0x84, 0x96);
        WriteReg(0x85, 0xa3);
        WriteReg(0x86, 0xaf);
        WriteReg(0x87, 0xc4);
        WriteReg(0x88, 0xd7);
        WriteReg(0x89, 0xe8);

        /* AGC and AEC parameters.  Note we start by disabling those features,
        then turn them only after tweaking the values. */
        WriteReg(0x13, COM8_FASTAEC | COM8_AECSTEP | COM8_BFILT);
        WriteReg(0x00, 0);
        WriteReg(0x10, 0);
        WriteReg(0x0d, 0x40);
        WriteReg(0x14, 0x18);
        WriteReg(0xa5, 0x05);
        WriteReg(0xab, 0x07);
        WriteReg(0x24, 0x95);
        WriteReg(0x25, 0x33);
        WriteReg(0x26, 0xe3);
        WriteReg(0x9f, 0x78);
        WriteReg(0xa0, 0x68);
        WriteReg(0xa1, 0x03);
        WriteReg(0xa6, 0xd8);
        WriteReg(0xa7, 0xd8);
        WriteReg(0xa8, 0xf0);
        WriteReg(0xa9, 0x90);
        WriteReg(0xaa, 0x94);
        WriteReg(0x13, COM8_FASTAEC|COM8_AECSTEP|COM8_BFILT|COM8_AGC|COM8_AEC);

        /* Almost all of these are magic "reserved" values.  */
        WriteReg(0x0e, 0x61);
        WriteReg(0x0f, 0x4b);
        WriteReg(0x16, 0x02);
        WriteReg(0x1e, 0x27);
        WriteReg(0x21, 0x02);
        WriteReg(0x22, 0x91);
        WriteReg(0x29, 0x07);
        WriteReg(0x33, 0x0b);
        WriteReg(0x35, 0x0b);
        WriteReg(0x37, 0x1d);
        WriteReg(0x38, 0x71);
        WriteReg(0x39, 0x2a);
        WriteReg(0x3c, 0x78);
        WriteReg(0x4d, 0x40);
        WriteReg(0x4e, 0x20);
        WriteReg(0x69, 0);
        WriteReg(0x6b, 0x0a);
        WriteReg(0x74, 0x10);
        WriteReg(0x8d, 0x4f);
        WriteReg(0x8e, 0);
        WriteReg(0x8f, 0);
        WriteReg(0x90, 0);
        WriteReg(0x91, 0);
        WriteReg(0x96, 0);
        WriteReg(0x9a, 0);
        WriteReg(0xb0, 0x84);
        WriteReg(0xb1, 0x0c);
        WriteReg(0xb2, 0x0e);
        WriteReg(0xb3, 0x82);
        WriteReg(0xb8, 0x0a);

        /* More reserved magic, some of which tweaks white balance */
        WriteReg(0x43, 0x0a);
        WriteReg(0x44, 0xf0);
        WriteReg(0x45, 0x34);
        WriteReg(0x46, 0x58);
        WriteReg(0x47, 0x28);
        WriteReg(0x48, 0x3a);
        WriteReg(0x59, 0x88);
        WriteReg(0x5a, 0x88);
        WriteReg(0x5b, 0x44);
        WriteReg(0x5c, 0x67);
        WriteReg(0x5d, 0x49);
        WriteReg(0x5e, 0x0e);
        WriteReg(0x6c, 0x0a);
        WriteReg(0x6d, 0x55);
        WriteReg(0x6e, 0x11);
        WriteReg(0x6f, 0x9f);
        WriteReg(0x6a, 0x40);
        WriteReg(0x01, 0x40);
        WriteReg(0x02, 0x60);
        WriteReg(0x13, COM8_FASTAEC|COM8_AECSTEP|COM8_BFILT|COM8_AGC|COM8_AEC|COM8_AWB);

        /* Matrix coefficients */
        WriteReg(0x4f, 0x80);
        WriteReg(0x50, 0x80);
        WriteReg(0x51, 0);
        WriteReg(0x52, 0x22);
        WriteReg(0x53, 0x5e);
        WriteReg(0x54, 0x80);
        WriteReg(0x58, 0x9e);

        WriteReg(0x41, 0x08);
        WriteReg(0x3f, 0);
        WriteReg(0x75, 0x05);
        WriteReg(0x76, 0xe1);
        WriteReg(0x4c, 0);
        WriteReg(0x77, 0x01);
        WriteReg(0x3d, 0xc3);
        WriteReg(0x4b, 0x09);
        WriteReg(0xc9, 0x60);
        WriteReg(0x41, 0x38);
        WriteReg(0x56, 0x40);

        WriteReg(0x34, 0x11);
        WriteReg(0x3b, COM11_EXP|COM11_HZAUTO);
        WriteReg(0xa4, 0x88);
        WriteReg(0x96, 0);
        WriteReg(0x97, 0x30);
        WriteReg(0x98, 0x20);
        WriteReg(0x99, 0x30);
        WriteReg(0x9a, 0x84);
        WriteReg(0x9b, 0x29);
        WriteReg(0x9c, 0x03);
        WriteReg(0x9d, 0x4c);
        WriteReg(0x9e, 0x3f);
        WriteReg(0x78, 0x04);

        /* Extra-weird stuff.  Some sort of multiplexor register */
        WriteReg(0x79, 0x01);
        WriteReg(0xc8, 0xf0);
        WriteReg(0x79, 0x0f);
        WriteReg(0xc8, 0x00);
        WriteReg(0x79, 0x10);
        WriteReg(0xc8, 0x7e);
        WriteReg(0x79, 0x0a);
        WriteReg(0xc8, 0x80);
        WriteReg(0x79, 0x0b);
        WriteReg(0xc8, 0x01);
        WriteReg(0x79, 0x0c);
        WriteReg(0xc8, 0x0f);
        WriteReg(0x79, 0x0d);
        WriteReg(0xc8, 0x20);
        WriteReg(0x79, 0x09);
        WriteReg(0xc8, 0x80);
        WriteReg(0x79, 0x02);
        WriteReg(0xc8, 0xc0);
        WriteReg(0x79, 0x03);
        WriteReg(0xc8, 0x40);
        WriteReg(0x79, 0x05);
        WriteReg(0xc8, 0x30);
        WriteReg(0x79, 0x26);

        WriteReg(0xff, 0xff); /* END MARKER */
    }

    return 1;
}




// write to camera
void OV7670::WriteReg(int addr,int data)
{
    _i2c.start();
    _i2c.write(OV7670_WRITE);
    wait_us(OV7670_WRITEWAIT);
    _i2c.write(addr);
    wait_us(OV7670_WRITEWAIT);
    _i2c.write(data);
    _i2c.stop();
}

// read from camera
int OV7670::ReadReg(int addr)
{
    int data;

    _i2c.start();
    _i2c.write(OV7670_WRITE);
    wait_us(OV7670_WRITEWAIT);
    _i2c.write(addr);
    _i2c.stop();
    wait_us(OV7670_WRITEWAIT);

    _i2c.start();
    _i2c.write(OV7670_READ);
    wait_us(OV7670_WRITEWAIT);
    data = _i2c.read(OV7670_NOACK);
    _i2c.stop();

    return data;
}
