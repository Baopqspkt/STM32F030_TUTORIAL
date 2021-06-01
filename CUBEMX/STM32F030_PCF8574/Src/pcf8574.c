#include "pcf8574.h"
#define PCF85741_ADDR			0x40

void pcf85741_write(uint8_t data_write)
{
	i2c_write_pcf(PCF85741_ADDR, data_write);
}

uint8_t pcf85741_read(void)
{
	uint8_t rdata;
  	rdata = i2c_read_pcf(PCF85741_ADDR);
	return rdata;
}

