
#include "main.h"
#include "rc522.h"
#include "gpio.h"
#include "i2c.h"
#include "pcf85741.h"
#include "spi.h"
#include "uart.h"
#include "gpio.h"
#include "rcc.h"

uint8_t CardID[5];
char szBuff[100];
uint8_t cnt;

int main(void)
{
  HAL_Init();
  SystemClock_Config();

  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_SPI1_Init();
  MX_USART1_UART_Init();
	
  printf("======================\n\r");
  printf("== STM32F030 + RFID ==\n\r");
  printf("======================\n\r");
	
	printf("\n\r\n\rSET PCF85741 OUTPUT HIGH \n\r");
	pcf85741_write(0x00);

  //--- This function below use to init RC522 ---//
	#if 0
  rc522_init();
	#endif
  while (1)
  {
    #if 0
    if (TM_MFRC522_Check(CardID) == MI_OK)
    {
			printf("Check Card OK \n\r");
      sprintf(szBuff, "ID: 0x%02X%02X%02X%02X%02X", CardID[0], CardID[1], CardID[2], CardID[3], CardID[4]);
      printf("szBuff: ");
			for (cnt = 0; cnt < 100; cnt++)
      {
        printf("%c", szBuff[cnt]);
      }
    }
		#else
		pcf85741_write(0x00);
		HAL_Delay(200);
		pcf85741_write(0xF0);
		HAL_Delay(200);
    #endif
  }
  /* USER CODE END 3 */
}

void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}

#endif
