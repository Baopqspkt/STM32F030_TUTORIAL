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
#include "stm32f0xx_spi.h"


#define SPI_MOSI	GPIO_Pin_7
#define SPI_MISO	GPIO_Pin_6
#define SPI_SCLK	GPIO_Pin_5
#define SPI_CS		GPIO_Pin_4

#define TIME_OUT	((uint32_t)(10 * 0x1000))   
#define I2C_SCL 	GPIO_Pin_9
#define I2C_SDA		GPIO_Pin_10

#define I2C_TEST_ADDR		0x30
#define TEST_BYTE_SEND	1

extern uint32_t SystemCoreClock;
static volatile uint32_t time = 0;

#define DELAY_US			0
#define DELAY_MS			1

void my_gpio_init(void);
void my_tim_init(void);
void delay_ms(uint16_t ms);

//========================> Setup Systick delay <====================
void systick_init(void);
void delay_us(uint32_t us);

//========================> INIT SPI <===============================
void my_spi_init(void);
void my_spi_write(uint8_t data);
uint8_t my_spi_read(void);

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

//========================> INIT SPI <===============================
void my_spi_init(void)
{
	// Init clock 
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	
	GPIO_InitTypeDef spi_gpio;
	spi_gpio.GPIO_Pin = SPI_SCLK|SPI_MISO|SPI_MOSI;
	spi_gpio.GPIO_Mode = GPIO_Mode_AF;
	spi_gpio.GPIO_OType = GPIO_OType_PP;
	spi_gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
	spi_gpio.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &spi_gpio);
	
	spi_gpio.GPIO_Pin = SPI_CS;
	spi_gpio.GPIO_Mode = GPIO_Mode_OUT;
	spi_gpio.GPIO_OType = GPIO_OType_PP;
	spi_gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
	spi_gpio.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &spi_gpio);
	
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource4, GPIO_AF_1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_1);
	
	SPI_InitTypeDef my_spi;
	SPI_I2S_DeInit(SPI1);
	my_spi.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	my_spi.SPI_CPHA = SPI_CPHA_1Edge;
	my_spi.SPI_CPOL = SPI_CPOL_Low;
	my_spi.SPI_CRCPolynomial = 7;
	my_spi.SPI_DataSize = SPI_DataSize_8b;
	my_spi.SPI_FirstBit = SPI_FirstBit_MSB;
	my_spi.SPI_Mode = SPI_Mode_Master;
	my_spi.SPI_NSS = SPI_NSS_Soft;
	my_spi.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
	
	SPI_Init(SPI1, &my_spi);
	SPI_Cmd(SPI1, ENABLE);
	
}

void my_spi_write(uint8_t data)
{
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET);
	SPI_SendData8(SPI1, data);
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
}

uint8_t my_spi_read(void)
{
	
}


