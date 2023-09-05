#include "i2c_driver.h"


#define CLKACTIVITY_I2C_FCLK            24
#define CLKACTIVITY_L4LS_GCLK           8
#define MAX_CYCLES_WAITING              200000

volatile pruI2c* i2c_dev[2] = {&i2c_dev1, &i2c_dev2};
uint32_t* CM_PER_I2C_CLKCTRL[2] = {(uint32_t*)0x44E00048, (uint32_t*)0x44E00044};

uint8_t i2c_pru_initialized[2] = {0, 0};

void i2c_pru_DelayMicros(uint8_t micros)
{
    uint16_t cycles = micros * 100;
    uint16_t i = 0;
    for (i = 0; i < cycles; i++)
    {
    }
}

uint8_t i2c_pru_WaitBB(uint8_t i2cDevice)
{
    uint32_t ticks = 0;
    while (i2c_dev[i2cDevice - 1]->I2C_IRQSTATUS_RAW_bit.I2C_IRQSTATUS_RAW_BB)
    {
        ticks++;
        if (ticks > MAX_CYCLES_WAITING)
        {
            return 0;
        }
    }
    return 1;
}

uint8_t i2c_pru_WaitBF(uint8_t i2cDevice)
{
    uint32_t ticks = 0;
    while (!i2c_dev[i2cDevice - 1]->I2C_IRQSTATUS_RAW_bit.I2C_IRQSTATUS_RAW_BF)
    {
        ticks++;
        if (ticks > MAX_CYCLES_WAITING)
        {
            return 0;
        }
    }
    return 1;
}

uint8_t i2c_pru_WaitXRDY(uint8_t i2cDevice)
{
    uint32_t ticks = 0;
    while (!i2c_dev[i2cDevice - 1]->I2C_IRQSTATUS_RAW_bit.I2C_IRQSTATUS_RAW_XRDY)
    {
        ticks++;
        if (ticks > MAX_CYCLES_WAITING)
        {
            return 0;
        }
    }
    return 1;
}

uint8_t i2c_pru_WaitRRDY(uint8_t i2cDevice)
{
    uint32_t ticks = 0;
    while (!i2c_dev[i2cDevice - 1]->I2C_IRQSTATUS_RAW_bit.I2C_IRQSTATUS_RAW_RRDY)
    {
        ticks++;
        if (ticks > MAX_CYCLES_WAITING)
        {
            return 0;
        }
    }
    return 1;
}

uint8_t i2c_pru_WaitARDY(uint8_t i2cDevice)
{
    uint32_t ticks = 0;
    while (!i2c_dev[i2cDevice - 1]->I2C_IRQSTATUS_RAW_bit.I2C_IRQSTATUS_RAW_ARDY)
    {
        ticks++;
        if (ticks > MAX_CYCLES_WAITING)
        {
            return 0;
        }
    }
    return 1;
}

uint8_t i2c_pru_WaitRDONE(uint8_t i2cDevice)
{
    uint32_t ticks = 0;
    while (!i2c_dev[i2cDevice - 1]->I2C_SYSS_bit.I2C_SYSS_RDONE)
    {
        ticks++;
        if (ticks > MAX_CYCLES_WAITING)
        {
            return 0;
        }
    }
    return 1;
}

uint8_t i2c_pru_ReadBytes(uint8_t i2cDevice, uint8_t address, uint8_t reg, uint8_t bytes, uint8_t *buffer)
{
    if (!i2c_pru_initialized[i2cDevice - 1])
    {
        if (!i2c_pru_Init(i2cDevice))
        {
            return 0;
        }
    }

    if (!i2c_pru_WaitBB(i2cDevice))
    {
        return 0;
    }

    i2c_dev[i2cDevice - 1]->I2C_SA_bit.I2C_SA_SA = address; // 7 bit address
    i2c_dev[i2cDevice - 1]->I2C_CNT_bit.I2C_CNT_DCOUNT = 1; // 1 byte to transmit
    i2c_dev[i2cDevice - 1]->I2C_CON = 0x8601;             // MST/TRX/STT
    i2c_pru_DelayMicros(7);

    if (!i2c_pru_WaitXRDY(i2cDevice))
    {
        return 0;
    }
    // write register to read
    i2c_dev[i2cDevice - 1]->I2C_DATA = reg;
    i2c_dev[i2cDevice - 1]->I2C_IRQSTATUS_RAW_bit.I2C_IRQSTATUS_RAW_XRDY = 0b1;

    // wait access to registers
    if (!i2c_pru_WaitARDY(i2cDevice))
    {
        return 0;
    }
    i2c_pru_DelayMicros(4);

    if (i2c_dev[i2cDevice - 1]->I2C_IRQSTATUS_RAW_bit.I2C_IRQSTATUS_RAW_AERR |
        i2c_dev[i2cDevice - 1]->I2C_IRQSTATUS_RAW_bit.I2C_IRQSTATUS_RAW_NACK)
    {
        return 0;
    }

    // read data
    i2c_dev[i2cDevice - 1]->I2C_CNT_bit.I2C_CNT_DCOUNT = bytes; // bytes to reveive
    i2c_dev[i2cDevice - 1]->I2C_CON = 0x8403;                  // MST/STP/STT
    i2c_pru_DelayMicros(12);
    i2c_dev[i2cDevice - 1]->I2C_IRQSTATUS_RAW_bit.I2C_IRQSTATUS_RAW_ARDY = 0b1;

    // wait data
    if (!i2c_pru_WaitRRDY(i2cDevice))
    {
        return 0;
    }

    uint8_t count;
    for (count = 0; count < bytes; count++)
    {
        // read byte
        buffer[count] = i2c_dev[i2cDevice - 1]->I2C_DATA;

        // require next data
        i2c_dev[i2cDevice - 1]->I2C_IRQSTATUS_RAW_bit.I2C_IRQSTATUS_RAW_RRDY = 0b1;

        // wait data
        if (!i2c_pru_WaitRRDY(i2cDevice))
        {
            return 0;
        }
        i2c_pru_DelayMicros(1);

        if (i2c_dev[i2cDevice - 1]->I2C_IRQSTATUS_RAW_bit.I2C_IRQSTATUS_RAW_AERR |
            i2c_dev[i2cDevice - 1]->I2C_IRQSTATUS_RAW_bit.I2C_IRQSTATUS_RAW_NACK)
        {
            return 0;
        }
    }

    // wait for access ready
    if (!i2c_pru_WaitARDY(i2cDevice))
    {
        return 0;
    }

    // wait for bus free
    // wait data
    if (!i2c_pru_WaitBF(i2cDevice))
    {
        return 0;
    }

    return count;
}

uint8_t i2c_pru_WriteBytes(uint8_t i2cDevice, uint8_t address, uint8_t reg, uint8_t bytes, uint8_t *buffer)
{
    if (!i2c_pru_initialized[i2cDevice - 1])
    {
        if (!i2c_pru_Init(i2cDevice))
        {
            return 0;
        }
    }
    if (!i2c_pru_WaitBB(i2cDevice))
    {
        return 0;
    }

    i2c_dev[i2cDevice - 1]->I2C_SA_bit.I2C_SA_SA = address; // 7 bit address
    i2c_dev[i2cDevice - 1]->I2C_CNT_bit.I2C_CNT_DCOUNT = bytes + 1; // 1 byte to transmit
    i2c_dev[i2cDevice - 1]->I2C_CON = 0x8603;                      // MST/TRX/STT/STP
    i2c_pru_DelayMicros(7);

    if (!i2c_pru_WaitXRDY(i2cDevice))
    {
        return 0;
    }
    // write register to read
    i2c_dev[i2cDevice - 1]->I2C_DATA = reg;
    i2c_dev[i2cDevice - 1]->I2C_IRQSTATUS_RAW_bit.I2C_IRQSTATUS_RAW_XRDY = 0b1;
    i2c_pru_DelayMicros(1);
    if (i2c_dev[i2cDevice - 1]->I2C_IRQSTATUS_RAW_bit.I2C_IRQSTATUS_RAW_AERR |
        i2c_dev[i2cDevice - 1]->I2C_IRQSTATUS_RAW_bit.I2C_IRQSTATUS_RAW_NACK)
    {
        return 0;
    }

    uint8_t count;
    for (count = 0; count < bytes; count++)
    {
        i2c_pru_WaitXRDY(i2cDevice);
        i2c_dev[i2cDevice - 1]->I2C_DATA = buffer[count];
        i2c_dev[i2cDevice - 1]->I2C_IRQSTATUS_RAW_bit.I2C_IRQSTATUS_RAW_XRDY = 0b1;
        i2c_pru_DelayMicros(1);
        if (i2c_dev[i2cDevice - 1]->I2C_IRQSTATUS_RAW_bit.I2C_IRQSTATUS_RAW_AERR |
            i2c_dev[i2cDevice - 1]->I2C_IRQSTATUS_RAW_bit.I2C_IRQSTATUS_RAW_NACK)
        {
            return 0;
        }
    }

    // wait for access ready
    if (!i2c_pru_WaitARDY(i2cDevice))
    {
        return 0;
    }
    i2c_pru_DelayMicros(6);

    // wait for bus free
    // wait data
    if (!i2c_pru_WaitBF(i2cDevice))
    {
        return 0;
    }

    // serve?
    i2c_dev[i2cDevice - 1]->I2C_IRQSTATUS_RAW_bit.I2C_IRQSTATUS_RAW_ARDY = 0b1;
    i2c_dev[i2cDevice - 1]->I2C_IRQSTATUS_RAW_bit.I2C_IRQSTATUS_RAW_XRDY = 1;
    i2c_dev[i2cDevice - 1]->I2C_IRQSTATUS_RAW_bit.I2C_IRQSTATUS_RAW_RRDY = 1;
    return count;
}

uint8_t i2c_pru_ReadReg(uint8_t i2cDevice, uint8_t address, uint8_t reg, uint8_t *buffer)
{
    return i2c_pru_ReadBytes(i2cDevice, address, reg, 1, buffer);
}

uint8_t i2c_pru_WriteReg(uint8_t i2cDevice, uint8_t address, uint8_t reg, uint8_t value)
{
    return i2c_pru_WriteBytes(i2cDevice, address, reg, 1, &value);
}

void i2c_pru_Set400KHz(uint8_t i2cDevice)
{
    // prescaler
    i2c_dev[i2cDevice - 1]->I2C_PSC = 0x04; // 24MHz
    /*
     * tLow = (SCLL +7)*83ns
     * 83ns is the time period at 48MHz/4(il PSC),
     * tLow = (1000000000ns/400000Hz)/2) is the time period (in ns) at low signal on SCL
     * SCLL = tLow/83ns -7
     * SCLL = 1250/83 -7
     * SCLL is like 8.06 (rounded)
     */
    i2c_dev[i2cDevice - 1]->I2C_SCLL = 0x09; // from linux setting ... check linux driver for calc
    /*
     * tHigh = (SCLH +5)*83ns
     * 83ns is the time period at 48MHz/4,
     * tHigh = (1000000000ns/400000Hz)/2) is the time period (in ns) at high signal on SCL
     * SCLH = tHigh/83ns -5
     * SCLH = 1250/83 -5
     * SCLH is like 10 (rounded)
     */
    i2c_dev[i2cDevice - 1]->I2C_SCLH = 0x0c; // from linux setting ... check linux driver for calc
}

void i2c_pru_Set100KHz(uint8_t i2cDevice)
{
    // prescaler
    i2c_dev[i2cDevice - 1]->I2C_PSC = 0x04; // 24MHz
    /*
     * tLow = (SCLL +7)*83ns
     * 83ns is the time period at 48MHz/4(il PSC),
     * tLow = (1000000000ns/100000Hz)/2) is the time period (in ns) at low signal on SCL
     * SCLL = tLow/83ns -7
     * SCLL = 3000/83 -7
     * SCLL is like 32 (rounded)
     */
    i2c_dev[i2cDevice - 1]->I2C_SCLL = 32;
    /*
     * tHigh = (SCLH +5)*83ns
     * 83ns is the time period at 48MHz/4,
     * tHigh = (1000000000ns/100000Hz)/2) is the time period (in ns) at high signal on SCL
     * SCLH = tHigh/83ns -5
     * SCLH = 3000/83 -5
     * SCLH is like 32 (rounded)
     */
    i2c_dev[i2cDevice - 1]->I2C_SCLH = 32;
}

uint8_t i2c_pru_Init(uint8_t i2cDevice)
{
    if (i2c_pru_initialized[i2cDevice - 1])
    {
        return 1;
    }

    if (i2cDevice < 1 || i2cDevice > 2)
    {
        return 0;
    }

    // enable clock
    *CM_PER_I2C_CLKCTRL[i2cDevice - 1] = 0x02;

    // reset
    i2c_dev[i2cDevice - 1]->I2C_SYSC_bit.I2C_SYSC_SRST = 0b1;

    // wait reset
    while (i2c_dev[i2cDevice - 1]->I2C_SYSS_bit.I2C_SYSS_RDONE != 1)
    {
    }

    // set I2C to functional mode
    i2c_dev[i2cDevice - 1]->I2C_CON = 0x0040;

    // disable auto idle
    i2c_dev[i2cDevice - 1]->I2C_SYSC_bit.I2C_SYSC_ENAWAKEUP = 0b1;
    i2c_dev[i2cDevice - 1]->I2C_SYSC_bit.I2C_SYSC_AUTOIDLE = 0b0;

    // set pin-mux to enable I2C on the pins
    i2c_pru_DelayMicros(100);

    i2c_pru_initialized[i2cDevice - 1] = 1;
    return 1;
}
