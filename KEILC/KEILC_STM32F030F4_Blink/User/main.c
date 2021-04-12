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


extern uint32_t SystemCoreClock;
static volatile uint32_t time = 0;

//========================> Setup GPIO and TIM <====================
void my_gpio_init(void);
void my_tim_init(void);
void delay_ms(uint16_t ms);
//========================> Setup i2c Master <====================
void my_i2c_init(void);

//========================> Setup Systick delay <====================
void systick_init(void);
void delay_us(uint32_t us);


int main()
{
	systick_init();
	my_gpio_init();
	my_tim_init();
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
	// First init i2c gpio
	
	// init I2c
	
	
	// Init DMA
}


