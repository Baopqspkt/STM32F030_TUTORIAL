/*
 * i2c.h
 *
 *  Created on: Apr 23, 2021
 *      Author: PC
 */

#include "stm32f0xx_hal.h"
void MX_I2C1_Init(void);
#if 0
//void i2c_write(I2C_HandleTypeDef *hi2c,uint8_t slave_addr, uint8_t* send_data);
//void i2c_write(I2C_HandleTypeDef *hi2c,uint8_t slave_addr, uint8_t addr_register, uint8_t send_data);
uint8_t i2c_read(I2C_HandleTypeDef *hi2c,uint8_t slave_addr, uint8_t register_addr);
#else
void i2c_write(uint8_t slave_addr,uint8_t send_data);
uint8_t i2c_read(uint8_t slave_addr);
#endif
//----------------------------- These function below use by DMA -------------------------//
void i2c_write_dma(I2C_HandleTypeDef *hi2c,uint8_t slave_addr, uint8_t addr_register, uint8_t send_data);

