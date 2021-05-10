#include "stm32f0xx_hal.h"


#define GPIO_RESET	0
#define GPIO_SET		1


void MX_GPIO_Init(void);
void gpio_set_value(GPIO_TypeDef* GPIOx,uint8_t pin, uint8_t value_write);
