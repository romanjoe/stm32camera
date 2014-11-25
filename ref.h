//
// OV7670 library
//

#pragma once
#include "mbed.h"
#include "ov7670reg.h"

#define OV7670_WRITE (0x42)
#define OV7670_READ  (0x43)
#define OV7670_WRITEWAIT (20)
#define OV7670_NOACK (0)
#define OV7670_REGMAX (201)
#define OV7670_I2CFREQ (100000)


class OV7670
{
    public:

        OV7670(
            PinName sda,    // Camera I2C port
            PinName scl,    // Camera I2C port
            PinName vs,     // VSYNC
            PinName hr,     // HREF
            PinName we,     // WEN

            PortName port,  // 8bit bus port
            int mask,       // 0b0000_0M65_4000_0321_L000_0000_0000_0000 = 0x07878000

            PinName rt,     // /RRST
            PinName o,      // /OE
            PinName rc      // RCLK
            );

        ~OV7670();

        void CaptureNext(void);              // capture request
        bool CaptureDone(void);              // capture done? (with clear)
        void WriteReg(int addr,int data);    // write to camera
        int ReadReg(int addr);               // read from camera
        void Reset(void);                    // reset reg camera
        int Init(char c, int n);             // Old init reg
        int Init(char *format, int n);       // init reg
        void VsyncHandler(void);             // New vsync handler
        void HrefHandler(void);              // href handler
        int ReadOnebyte(void);               // Data Read
        void ReadStart(void);                // Data Start
        void ReadStop(void);                 // Data Stop


    private:
        I2C _i2c;
        InterruptIn vsync,href;

        DigitalOut wen;
        PortIn data;
        DigitalOut rrst,oe,rclk;
        volatile int LineCounter;
        volatile int LastLines;
        volatile bool CaptureReq;
        volatile bool Busy;
        volatile bool Done;
        char *format_temp;
};
