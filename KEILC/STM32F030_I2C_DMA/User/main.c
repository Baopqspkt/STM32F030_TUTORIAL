/*
*	Create by bpham
*	In this tutorial i will init delay_ms use timer 3
*	delay_us use systick in cortex M0
*/

#include "stm32f0xx.h"
#include "stm32f0xx_rcc.h"
#include "stm32f0xx_gpio.h" 
#include "stm32f0xx_tim.h"
#include "stm32f0xx_it.h"
#include "stm32f0xx_i2c.h"
#include "stm32f0xx_dma.h"

#define TIME_OUT	((uint32_t)(10 * 0x1000))   
#define I2C_SCL 	GPIO_Pin_9
#define I2C_SDA		GPIO_Pin_10

#define I2C_DR		0x40005428

#define I2C_TEST_ADDR		0x30
#define TEST_BYTE_SEND	1

extern uint32_t SystemCoreClock;
static volatile uint32_t time = 0;
int32_t timeout = TIME_OUT;
uint32_t data_write_dma = 0;

#define DELAY_US			0
#define DELAY_MS			1

void my_gpio_init(void);
void my_tim_init(void);
void delay_ms(uint16_t ms);

//========================> Setup Systick delay <====================
void systick_init(void);
void delay_us(uint32_t us);

//========================> INIT I2C <===============================
void my_i2c_init(void);
void my_i2c_dma_init(void);
void i2c_write_bydma(uint8_t data_write);
uint8_t my_i2c_write(uint8_t addr, uint8_t register_write, uint8_t data_write);
uint8_t my_i2c_read(uint8_t addr, uint8_t addr_register);


int main()
{
	uint8_t ret = 0;
	systick_init();
	my_gpio_init();
	my_tim_init();
	my_i2c_init();
	my_i2c_dma_init();
	
	ret = my_i2c_write(0x5A<<1, 0x80, 0x63);
	delay_ms(1000);
	ret = my_i2c_read(0x5A<<1, 0x5D);
	while(1)
	{
		GPIO_ResetBits(GPIOA, GPIO_Pin_4);
		delay_ms(500);
		GPIO_SetBits(GPIOA, GPIO_Pin_4);
		delay_ms(500);
	}
}

void my_gpio_init(void)
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef stm32f030_gpio_init;
	stm32f030_gpio_init.GPIO_Mode = GPIO_Mode_OUT;
	stm32f030_gpio_init.GPIO_OType = GPIO_OType_OD;
	stm32f030_gpio_init.GPIO_Pin = GPIO_Pin_4;
	stm32f030_gpio_init.GPIO_PuPd = GPIO_PuPd_NOPULL;
	stm32f030_gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &stm32f030_gpio_init);
}

void my_tim_init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	TIM_TimeBaseInitTypeDef my_tim_init;
	my_tim_init.TIM_ClockDivision = 0;
	my_tim_init.TIM_CounterMode = TIM_CounterMode_Up;

	my_tim_init.TIM_Period = 65535;
	my_tim_init.TIM_Prescaler = 48000 - 1;

	TIM_TimeBaseInit(TIM3, &my_tim_init);
}

void delay_ms(uint16_t ms)
{

	TIM_SetCounter(TIM3, 0x0);
	TIM_Cmd(TIM3, ENABLE);
	while(TIM_GetCounter(TIM3) < ms);
	TIM_Cmd(TIM3, DISABLE);
}

//========================> Setup Systick delay <====================
void systick_init(void)
{
	SystemCoreClockUpdate();
	// 48*10^6/1000 = 48000.
	// 48*10^3/48*10^6 -> 1ms
	// 48*10^6/1.000.000 = 48.
	// 48/48*10^6 -> 1us
	while(SysTick_Config(SystemCoreClock/1000000) != 0);
}

void delay_us(uint32_t ms)
{
	time = ms;
	while(time != 0);
}

void SysTick_Handler(void)
{
	if(time != 0)
		time --;
}


void my_i2c_init(void)
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

	//RCC_I2CCLKConfig(RCC_I2C1CLK_SYSCLK);
	GPIO_InitTypeDef i2c_gpio_init;
	I2C_InitTypeDef my_i2c_init;
	
	i2c_gpio_init.GPIO_Mode = GPIO_Mode_AF;
	i2c_gpio_init.GPIO_OType = GPIO_OType_OD;
	i2c_gpio_init.GPIO_Pin = I2C_SCL|I2C_SDA;
	i2c_gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
	i2c_gpio_init.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &i2c_gpio_init);
	
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9,GPIO_AF_4);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10,GPIO_AF_4);
	
	RCC_I2CCLKConfig(RCC_I2C1CLK_SYSCLK);
	my_i2c_init.I2C_Ack = I2C_Ack_Enable;
	my_i2c_init.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	my_i2c_init.I2C_AnalogFilter = I2C_AnalogFilter_Enable;
	my_i2c_init.I2C_DigitalFilter = 0x0;
	my_i2c_init.I2C_Mode = I2C_Mode_I2C;
	my_i2c_init.I2C_OwnAddress1 = 0x00;
	my_i2c_init.I2C_Timing = 0x20303e5d; //0x20303e5d
	I2C_Init(I2C1,&my_i2c_init);
	
}

void my_i2c_dma_init(void)
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	DMA_InitTypeDef i2c_dma_init;
	
	DMA_DeInit(DMA1_Channel2);
	// DMA1_CH2_I2C1_TX
	i2c_dma_init.DMA_PeripheralBaseAddr = (uint32_t)(&I2C1->TXDR);
	i2c_dma_init.DMA_MemoryBaseAddr = (uint32_t)&data_write_dma;
	i2c_dma_init.DMA_DIR = DMA_DIR_PeripheralDST;
	i2c_dma_init.DMA_BufferSize = 2;
	i2c_dma_init.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	i2c_dma_init.DMA_MemoryInc = DMA_MemoryInc_Disable;
	i2c_dma_init.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	i2c_dma_init.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	i2c_dma_init.DMA_Mode = DMA_Mode_Normal;
	i2c_dma_init.DMA_Priority = DMA_Priority_High;
	i2c_dma_init.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel2, &i2c_dma_init);
	
	// This function is available only for STM32F091 devices.
	// DMA_RemapConfig(DMA1, DMA1_CH2_I2C1_TX);
}

void i2c_write_bydma(uint8_t data_write)
{
	data_write_dma = data_write;
	DMA_ClearFlag(DMA1_FLAG_TC2|DMA1_FLAG_HT2);
	I2C_DMACmd(I2C1, I2C_DMAReq_Tx, ENABLE);
	DMA_Cmd(DMA1_Channel2, ENABLE);
	timeout = TIME_OUT;
	while(DMA_GetFlagStatus(DMA1_FLAG_TC2|DMA1_FLAG_HT2) == RESET)
	{
		timeout --;
		if(timeout == 0)
			return ; //ERROR 1
	}
	
	DMA_ClearFlag(DMA1_FLAG_TC2|DMA1_FLAG_HT2);
	DMA_Cmd(DMA1_Channel2, DISABLE);
	I2C_DMACmd(I2C1, I2C_DMAReq_Tx, DISABLE);
}

uint8_t my_i2c_write(uint8_t addr, uint8_t register_write, uint8_t data_write)
{
	I2C_Cmd(I2C1, ENABLE);
	timeout = TIME_OUT;
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) != RESET)
	{
		timeout --;
		if(timeout == 0)
			
			return 1; //ERROR 1
	}
	I2C_TransferHandling(I2C1, addr, 1, I2C_Reload_Mode, I2C_Generate_Start_Write);
	// Wait until bit TX flag is set
	timeout = TIME_OUT;
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_TXIS) == RESET)
	{
		timeout --;
		if(timeout == 0)
		{
			I2C_GenerateSTOP(I2C1,ENABLE);
			I2C_Cmd(I2C1, DISABLE);
			return 1; //ERROR 2
		}
			
	}

	i2c_write_bydma(register_write);
	// I2C_SendData(I2C1,register_write);
	timeout = TIME_OUT;
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_TCR) == RESET)
	{
		timeout --;
		if(timeout == 0)
		{
			I2C_GenerateSTOP(I2C1,ENABLE);
			I2C_Cmd(I2C1, DISABLE);
			return 1; // ERROR 3
		}
	}
	
	timeout = TIME_OUT;
	while(I2C1->TXDR != register_write)
	{
		timeout --;
		if(timeout == 0)
		{			
			I2C_GenerateSTOP(I2C1,ENABLE);
			I2C_Cmd(I2C1, DISABLE);
			return 1; // ERROR 3
		}
	}
	
	I2C_TransferHandling(I2C1, addr, TEST_BYTE_SEND, I2C_AutoEnd_Mode, I2C_No_StartStop);
	timeout = TIME_OUT;
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_TXIS) == RESET)
	{
		timeout --;
		if(timeout == 0)
		{
			I2C_GenerateSTOP(I2C1,ENABLE);
			I2C_Cmd(I2C1, DISABLE);
			return 1; // ERROR 3
		}
	}
	
	i2c_write_bydma(data_write);
	timeout = TIME_OUT;
	while(I2C1->TXDR != data_write)
	{
		timeout --;
		if(timeout == 0)
		{
			I2C_GenerateSTOP(I2C1,ENABLE);
			I2C_Cmd(I2C1, DISABLE);
			return 1; // ERROR 3
		}
	}
	// I2C_SendData(I2C1,data_write);
	timeout = TIME_OUT;
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_TCR) == RESET)
	{
		timeout --;
		if(timeout == 0)
		{			
			I2C_GenerateSTOP(I2C1,ENABLE);
			I2C_Cmd(I2C1, DISABLE);
			return 1; // ERROR 3
		}
	}
	
	I2C_GenerateSTOP(I2C1,ENABLE);
	timeout = TIME_OUT;
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_STOPF))
	{
		timeout --;
		if(timeout == 0)
			return 1; // ERROR 3
	}
	I2C_ClearFlag(I2C1, I2C_ICR_STOPCF);
	I2C_Cmd(I2C1, DISABLE);
	return 0;
}

uint8_t my_i2c_read(uint8_t addr, uint8_t addr_register)
{
	I2C_Cmd(I2C1, ENABLE);
	uint8_t rdata;
	timeout = TIME_OUT;
	while(I2C_GetFlagStatus(I2C1, I2C_ISR_BUSY) != RESET)
	{
		timeout --;
		if(timeout == 0)
		{
			I2C_Cmd(I2C1, DISABLE);
			return 1; // ERROR 1
		}
	}
	I2C_TransferHandling(I2C1, addr, TEST_BYTE_SEND, I2C_AutoEnd_Mode, I2C_Generate_Start_Write);
	
	// Wait until bit TX flag is set
	timeout = TIME_OUT;
	while(I2C_GetFlagStatus(I2C1, I2C_ISR_TXIS) == RESET)
	{
		timeout --;
		if(timeout == 0)
		{
			I2C_GenerateSTOP(I2C1,ENABLE);
			I2C_Cmd(I2C1, DISABLE);
			return 1; //ERROR 2
		}
	}
	
	// send register addr
	i2c_write_bydma(addr_register);
	while(I2C1->TXDR != addr_register)
	{
		timeout --;
		if(timeout == 0)
		{
			I2C_GenerateSTOP(I2C1,ENABLE);
			I2C_Cmd(I2C1, DISABLE);
			return 1; // ERROR 3
		}
	}
	
	i2c_write_bydma(addr_register);
	while(I2C1->TXDR != addr_register)
	{
		timeout --;
		if(timeout == 0)
		{
			I2C_GenerateSTOP(I2C1,ENABLE);
			I2C_Cmd(I2C1, DISABLE);
			return 1; // ERROR 3
		}
	}
//	I2C_SendData(I2C1, addr_register);
	timeout = TIME_OUT;
	while(I2C_GetFlagStatus(I2C1, I2C_ISR_TC) == RESET)
	{
		timeout --;
		if(timeout == 0)
		{
			I2C_GenerateSTOP(I2C1,ENABLE);
			I2C_Cmd(I2C1, DISABLE);
			return 1; //ERROR 3
		}
	}
	
	I2C_TransferHandling(I2C1, addr, TEST_BYTE_SEND, I2C_AutoEnd_Mode, I2C_Generate_Start_Read);
	timeout = TIME_OUT;
	while(I2C_GetFlagStatus(I2C1, I2C_ISR_RXNE) == RESET)
	{
		timeout --;
		if(timeout == 0)
		{
			I2C_GenerateSTOP(I2C1,ENABLE);
			I2C_Cmd(I2C1, DISABLE);
			return 1; //ERROR 4
		}
	}
	rdata = I2C_ReceiveData(I2C1);
	
	
	timeout = TIME_OUT;
	while(I2C_GetFlagStatus(I2C1, I2C_ISR_STOPF) == RESET)
	{
		timeout --;
		if(timeout == 0)
			return 1; //ERROR 5 
	}
	I2C_ClearFlag(I2C1, I2C_ICR_STOPCF);
	I2C_Cmd(I2C1, DISABLE);
	return rdata;
	
}
