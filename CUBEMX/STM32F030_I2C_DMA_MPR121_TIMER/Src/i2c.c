/*
 * i2c.c
 *
 *  Created on: Apr 23, 2021
 *      Author: PC
 */
#include "i2c.h"

// Format for send data is: wdata[2]={register_addr, write_data}
void i2c_write(I2C_HandleTypeDef *hi2c,uint8_t slave_addr, uint8_t addr_register, uint8_t send_data)
{
//	HAL_I2C_Master_Transmit(hi2c, slave_addr, send_data, sizeof(send_data),100);
	HAL_I2C_Mem_Write(hi2c, slave_addr, addr_register, 1, &send_data, 1, 1000);
}

uint8_t i2c_read(I2C_HandleTypeDef *hi2c,uint8_t slave_addr, uint8_t register_addr)
{
	uint8_t rdata;
	HAL_I2C_Mem_Read(hi2c, slave_addr, register_addr, sizeof(register_addr), &rdata, sizeof(rdata), 1000);
	return rdata;
}

//----------------------------- These function below use by DMA -------------------------//
void i2c_write_dma(I2C_HandleTypeDef *hi2c,uint8_t slave_addr, uint8_t addr_register, uint8_t send_data)
{
	HAL_I2C_Mem_Write_DMA(hi2c, slave_addr, addr_register, 1, &send_data, 1);
}


