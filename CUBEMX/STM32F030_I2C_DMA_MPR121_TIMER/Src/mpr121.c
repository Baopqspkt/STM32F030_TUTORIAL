/*
 * #define MPR121.c
 *
 *  Created on: Apr 24, 2021
 *      Author: PC
 */


#include "mpr121.h"

// define VALUE WRITE FOR MPR121
#define MPR121_RESET			   				0x63
#define MPR121_DEFAULT_CONFIG2	   	0x24
/*
*	This function will init #define MPR121 with step:
*	1. 	Reset #define MPR121.
*	2. 	Set electrode configure to default value.
*	3. 	Read MPR121_CONFIG2 to check default value after reset.
*	4.	Set touch and thresholds.
*	5. 	Configure baseline filtering control register.
*	6. 	Set other configuration register.
*	7.	Enable ectrodes
*/
void mpr121_int(I2C_HandleTypeDef *hi2c)
{
	uint8_t rdata = 0;
	// Step 1
	i2c_write(hi2c, SLAVE_ADDR, MPR121_SOFTRESET, MPR121_RESET);
	HAL_Delay(1);
	// Step 2
	i2c_write(hi2c, SLAVE_ADDR, MPR121_ECR, 0X0);
	// Step 3
	rdata = i2c_read(hi2c, SLAVE_ADDR, MPR121_CONFIG2);
	if((rdata & MPR121_DEFAULT_CONFIG2) != MPR121_DEFAULT_CONFIG2)
	{
		// Should Set Led to high
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
		return;
	}

	// Step 4
	mpr121_set_thresholds(hi2c, 12, 6, 12);
	// Set BaseLine
	#if 0
	mpr121_set_baseline(hi2c, 0x4e, 12);
	#endif
	// Step 5
	i2c_write(hi2c, SLAVE_ADDR, MPR121_MHDR, 0x1);
	i2c_write(hi2c, SLAVE_ADDR, MPR121_NHDR, 0X1);
	i2c_write(hi2c, SLAVE_ADDR, MPR121_NCLR, 0XE);
	i2c_write(hi2c, SLAVE_ADDR, MPR121_FDLR, 0X0);
	i2c_write(hi2c, SLAVE_ADDR, MPR121_MHDF, 0X1);
	i2c_write(hi2c, SLAVE_ADDR, MPR121_NHDF, 0X5);
	i2c_write(hi2c, SLAVE_ADDR, MPR121_NCLF, 0X1);
	i2c_write(hi2c, SLAVE_ADDR, MPR121_FDLF, 0X0);
	i2c_write(hi2c, SLAVE_ADDR, MPR121_NHDT, 0X0);
	i2c_write(hi2c, SLAVE_ADDR, MPR121_NCLT, 0X0);
	i2c_write(hi2c, SLAVE_ADDR, MPR121_FDLT, 0X0);

	// Step 6
	i2c_write(hi2c, SLAVE_ADDR, MPR121_DEBOUNCE, 0X0);
	i2c_write(hi2c, SLAVE_ADDR, MPR121_CONFIG1, 0X10);
	i2c_write(hi2c, SLAVE_ADDR, MPR121_CONFIG2, 0X20);

	// Step 7
	i2c_write(hi2c, SLAVE_ADDR, MPR121_ECR, 0X83); // 0000 1000 0011 0 -> 2
	mpr121_set_output(hi2c);
}

void mpr121_set_thresholds(I2C_HandleTypeDef *hi2c, uint8_t touch, uint8_t release, uint8_t Nb_pin_used)
{
	// Init all 12 register touch and release
	uint8_t loop;
	for (loop = 0; loop < Nb_pin_used; loop ++)
	{
		i2c_write(hi2c, SLAVE_ADDR, MPR121_TOUCHTH_0 + (loop*2), touch);
		i2c_write(hi2c, SLAVE_ADDR, MPR121_RELEASETH_0 + (loop*2), release);
	}

}

void mpr121_set_baseline(I2C_HandleTypeDef *hi2c, uint8_t base_line, uint8_t Nb_pin_used)
{
	// Init all 12 register touch and release
	uint8_t loop;
	for (loop = 0; loop < Nb_pin_used; loop ++)
	{
		i2c_write(hi2c, SLAVE_ADDR, MPR121_BASELINE_0 + (loop*2), base_line);
	}

}

uint8_t mpr121_get_thresholds(I2C_HandleTypeDef *hi2c, uint8_t pin, uint8_t type)
{
	uint8_t rdata;
	if(type == MPR121_TOUCH_THRESHOLD)
	{
		rdata = i2c_read(hi2c, SLAVE_ADDR, MPR121_TOUCHTH_0 + (pin*2));
	}
	else if (type == MPR121_RELEASE_THRESHOLD)
	{
		rdata = i2c_read(hi2c, SLAVE_ADDR, MPR121_RELEASETH_0 + (pin*2));
	}
	return rdata;
}

uint8_t mpr121_filtered_data(I2C_HandleTypeDef *hi2c, uint8_t pin, uint8_t type)
{
	uint8_t rdata;
	if(type == MPR121_FILTER_L)
	{		
		rdata = i2c_read(hi2c, SLAVE_ADDR, MPR121_FILTDATA_0L + (pin*2));
	}
	else
	{
		rdata = i2c_read(hi2c, SLAVE_ADDR, MPR121_FILTDATA_0H + (pin*2));
	}
	return rdata;
}


uint8_t mpr121_baseline_data(I2C_HandleTypeDef *hi2c, uint8_t pin)
{
	uint8_t rdata;
	rdata = i2c_read(hi2c, SLAVE_ADDR, MPR121_BASELINE_0 + (pin));
	return rdata;
}

uint16_t mpr121_touched(I2C_HandleTypeDef *hi2c)
{
	uint8_t rdatal;
	uint16_t rdata;
	rdatal = i2c_read(hi2c, SLAVE_ADDR, MPR121_TOUCHSTATUS_L);
	rdata = i2c_read(hi2c, SLAVE_ADDR, MPR121_TOUCHSTATUS_H);
	rdata = ((rdata<<8)|(rdatal));
	
	#if 0
	printf("\n\r\n\r-------------------------------------\n\r");
	printf("rdatal: %x, Touch Status: %x \n\r", rdatal, rdata);
	
	// Display mpr121_filtered_data
	rdatal = mpr121_filtered_data(hi2c, 0, MPR121_FILTER_L);
	printf("filtered_L: %x, ", rdatal);
	rdatal = mpr121_filtered_data(hi2c, 0, MPR121_FILTER_H);
	printf("filtered_H: %x \n\r", rdatal);
	// Display base line
	rdatal = mpr121_baseline_data(hi2c, 0);
	printf("baseline: %x \n\r", rdatal);
	// get thresold
	rdatal = mpr121_get_thresholds(hi2c, 0, MPR121_TOUCH_THRESHOLD);
	printf("touch threshold: %x \n\r", rdatal);
	rdatal = mpr121_get_thresholds(hi2c, 0, MPR121_RELEASE_THRESHOLD);
	printf("release threshold: %x \n\r", rdatal);
	printf("-------------------------------------\n\r");
	#endif
	return rdata&0xFFF;
}

void mpr121_set_output(I2C_HandleTypeDef *hi2c)
{
	// Enable register
	i2c_write(hi2c, SLAVE_ADDR, MPR121_GPIOEN, 0xF0);
	// Set direction
	i2c_write(hi2c, SLAVE_ADDR, MPR121_GPIODIR, 0xF0);
	// Set GTL 
	i2c_write(hi2c, SLAVE_ADDR, MPR121_GPIOGTL0, 0x0);
	i2c_write(hi2c, SLAVE_ADDR, MPR121_GPIOGTL1, 0x0);
	
	// Set data
	i2c_write(hi2c, SLAVE_ADDR, MPR121_GPIODATA, 0xF0);
}

void mpr121_write_data(I2C_HandleTypeDef *hi2c, uint8_t data)
{
	//printf("Led Display: %x \n\r", ~(data));
	i2c_write(hi2c, SLAVE_ADDR, MPR121_GPIODATA, data);
}

void mpr121_display_touch(I2C_HandleTypeDef *hi2c)
{
	uint8_t rdata_touch;
	rdata_touch = mpr121_touched(hi2c);
	
	//printf("rdata touch: %x \n\r", rdata_touch);
	mpr121_write_data(hi2c, rdata_touch << 4);
}



