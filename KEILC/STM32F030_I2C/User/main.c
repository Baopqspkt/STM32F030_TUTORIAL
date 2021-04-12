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

#define TIME_OUT	((uint32_t)(10 * 0x1000))   
#define I2C_SCL 	GPIO_Pin_9
#define I2C_SDA		GPIO_Pin_10

#define I2C_TEST_ADDR		0x30
#define TEST_BYTE_SEND	1

extern uint32_t SystemCoreClock;
static volatile uint32_t time = 0;
uint32_t timeout = TIME_OUT;

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
uint8_t my_i2c_write(uint8_t addr, uint8_t addr_register, uint8_t data);
uint8_t my_i2c_read(uint8_t addr, uint8_t addr_register);

int main()
{
	uint8_t ret = 0;
	systick_init();
	my_gpio_init();
	my_tim_init();
	my_i2c_init();
	ret = my_i2c_write(0x5a<<1, 0x80, 0x63);
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
	i2c_gpio_init.GPIO_Speed = GPIO_Speed_2MHz;
	i2c_gpio_init.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &i2c_gpio_init);
	
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9,GPIO_AF_4);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10,GPIO_AF_4);
	
	my_i2c_init.I2C_Ack = I2C_Ack_Enable;
	my_i2c_init.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	my_i2c_init.I2C_AnalogFilter = I2C_AnalogFilter_Enable;
	my_i2c_init.I2C_DigitalFilter = 0x0;
	my_i2c_init.I2C_Mode = I2C_Mode_I2C;
	my_i2c_init.I2C_OwnAddress1 = 0x30;
	my_i2c_init.I2C_Timing = 0x20303e5d;
	I2C_Init(I2C1,&my_i2c_init);
	I2C_Cmd(I2C1, ENABLE);
}

uint8_t my_i2c_write(uint8_t addr, uint8_t addr_register, uint8_t data)
{
	timeout = TIME_OUT;
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) != RESET)
	{
		timeout --;
		if(timeout == 0)
			return 1; //ERROR 1
	}
	I2C_TransferHandling(I2C1, addr, TEST_BYTE_SEND, I2C_Reload_Mode, I2C_Generate_Start_Write);
	// Wait until bit TX flag is set
	timeout = TIME_OUT;
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_TXIS) == RESET)
	{
		timeout --;
		if(timeout == 0)
			return 1; //ERROR 2
	}
	
//	I2C_GenerateSTART(I2C1, ENABLE);
//	i2c_write_bydma(addr);
//	i2c_write_bydma(addr_register);
	// I2C_SendData(I2C1, addr);
	I2C_SendData(I2C1, addr_register);
	timeout = TIME_OUT;
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_TCR) == RESET)
	{
		timeout --;
		if(timeout == 0)
			return 1; // ERROR 3
	}
	
	I2C_TransferHandling(I2C1, addr, TEST_BYTE_SEND, I2C_AutoEnd_Mode, I2C_No_StartStop);
	timeout = TIME_OUT;
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_TXIS) == RESET)
	{
		timeout --;
		if(timeout == 0)
			return 1; // ERROR 3
	}
	
	// i2c_write_bydma(data);
	I2C_SendData(I2C1, data);
	timeout = TIME_OUT;
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_TCR) == RESET)
	{
		timeout --;
		if(timeout == 0)
			return 1; // ERROR 3
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
	return 0;
}

uint8_t my_i2c_read(uint8_t addr, uint8_t addr_register)
{
	uint8_t rdata;
	timeout = TIME_OUT;
	while(I2C_GetFlagStatus(I2C1, I2C_ISR_BUSY) != RESET)
	{
		timeout --;
		if(timeout == 0)
			return 1; // ERROR 1
	}
	I2C_TransferHandling(I2C1, addr, TEST_BYTE_SEND, I2C_SoftEnd_Mode, I2C_Generate_Start_Write);
	
	// Wait until bit TX flag is set
	timeout = TIME_OUT;
	while(I2C_GetFlagStatus(I2C1, I2C_ISR_TXIS) == RESET)
	{
		timeout --;
		if(timeout == 0)
			return 1; //ERROR 2
	}
	
	// send register addr
	// i2c_write_bydma(addr_register);
	I2C_SendData(I2C1, addr_register);
	timeout = TIME_OUT;
	while(I2C_GetFlagStatus(I2C1, I2C_ISR_TC) == RESET)
	{
		timeout --;
		if(timeout == 0)
			return 1; //ERROR 3
	}
	
	I2C_TransferHandling(I2C1, addr, TEST_BYTE_SEND, I2C_AutoEnd_Mode, I2C_Generate_Start_Read);
	timeout = TIME_OUT;
	while(I2C_GetFlagStatus(I2C1, I2C_ISR_RXNE) == RESET)
	{
		timeout --;
		if(timeout == 0)
			return 1; //ERROR 4
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
	
	return rdata;
}
