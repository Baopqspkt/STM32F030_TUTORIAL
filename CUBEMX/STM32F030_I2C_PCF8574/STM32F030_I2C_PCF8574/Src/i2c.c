/*
 * i2c.c
 *
 *  Created on: Apr 23, 2021
 *      Author: PC
 */
#include "i2c.h"

I2C_HandleTypeDef hi2c1;

/* I2C1 init function */
void MX_I2C1_Init(void)
{
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x2000090E;
  hi2c1.Init.OwnAddress1 = 96;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure Analogue filter 
    */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure Digital filter 
    */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

#if 0
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
#else
// Format for send data is: wdata[2]={register_addr, write_data}
void i2c_write(uint8_t slave_addr, uint8_t send_data)
{
	HAL_I2C_Master_Transmit(&hi2c1, slave_addr, &send_data, 1, 100);
}

uint8_t i2c_read(uint8_t slave_addr)
{
	uint8_t rdata;
	HAL_I2C_Master_Receive(&hi2c1, slave_addr, &rdata, sizeof(rdata), 100);
	return rdata;
}
#endif
//----------------------------- These function below use by DMA -------------------------//
void i2c_write_dma(I2C_HandleTypeDef *hi2c,uint8_t slave_addr, uint8_t addr_register, uint8_t send_data)
{
	HAL_I2C_Mem_Write_DMA(hi2c, slave_addr, addr_register, 1, &send_data, 1);
}



