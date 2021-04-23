/*
 * i2c.h
 *
 *  Created on: Apr 23, 2021
 *      Author: PC
 */

#include "main.h"

//void i2c_write(I2C_HandleTypeDef *hi2c,uint8_t slave_addr, uint8_t* send_data);
void i2c_write(I2C_HandleTypeDef *hi2c,uint8_t slave_addr, uint8_t addr_register, uint8_t send_data);
uint8_t i2c_read(I2C_HandleTypeDef *hi2c,uint8_t slave_addr, uint8_t register_addr);
