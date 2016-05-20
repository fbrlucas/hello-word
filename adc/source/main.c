#include "main.h"
#include "stm32f4xx_hal.h"

// Debug tip:
// Connect to target, reset (black button), then load and press c

ADC_HandleTypeDef adc_dev;

static void SystemClock_Config(void);
static void Error_Handler(void);

void HAL_SYSTICK_Callback(void)
{
}

void setup_adc(void)
{
    ADC_ChannelConfTypeDef channel;
    GPIO_InitTypeDef  adc_pin;

    // Ligando os clocks
    __HAL_RCC_ADC1_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    // Setup do pino
    adc_pin.Pin = GPIO_PIN_0;
    adc_pin.Mode = GPIO_MODE_ANALOG;
    adc_pin.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &adc_pin);

    // CONFIGURAÇÕES GERAIS DO ADC
    // Qual ADC ? 1, 2 ou 3?
    adc_dev.Instance = ADC1;
    // Do manual, máximo é 30MHz (APB2/4 = 84/4 = 21MHz)
    adc_dev.Init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV4;
    // Qual resolução ?
    adc_dev.Init.Resolution = ADC_RESOLUTION_12B;
    // Conversão multi channel ou single channel  ?
    adc_dev.Init.ScanConvMode = DISABLE;
    // Fica lendo continuamente ou somente uma vez ?
    adc_dev.Init.ContinuousConvMode = DISABLE;
    // Algum tipo de leitura discontínua ? Se sim, quantas ?
    adc_dev.Init.DiscontinuousConvMode = DISABLE;
    adc_dev.Init.NbrOfDiscConversion = 0;
    // Alguma fonte externa que inicia a conversão ?
    adc_dev.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    adc_dev.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T1_CC1;
    // Alinha à direita ou esquerda ?
    adc_dev.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    // Quantas conversões serão feitas ? (multi channel, 1 a 16)
    adc_dev.Init.NbrOfConversion = 1;
    // Vamos usar DMA ?
    adc_dev.Init.DMAContinuousRequests = DISABLE;
    // Interrupção no fim do scan ou no fim de cada canal ?
    adc_dev.Init.EOCSelection = ADC_EOC_SINGLE_CONV;

    if(HAL_ADC_Init(&adc_dev) != HAL_OK)
      Error_Handler(); 

    // CONFIGURAÇÕES DO/DOS CANAL/CANAIS

    // Qual canal está sendo confgurado ?
    channel.Channel = ADC_CHANNEL_8;
    // Posição ?
    channel.Rank = 1;
    // Tempo de conversão usado ?
    channel.SamplingTime = ADC_SAMPLETIME_28CYCLES;
    // Não usado
    channel.Offset = 0;

    if(HAL_ADC_ConfigChannel(&adc_dev, &channel) != HAL_OK)
      Error_Handler(); 
}


int main(void)
{
    uint32_t adc_val;

    // inicializa o HAL
    HAL_Init();

    // muda o clock de interno para externo, com 168MHz
    SystemClock_Config();

    // adjust systick
    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);
    
    setup_adc();

    while (1)
    {
        // liga o início da conversão
        HAL_ADC_Start(&adc_dev);

        // espera até 2000ms para que a conversão finalize ou aborta
        if(HAL_ADC_PollForConversion(&adc_dev, 2000) == HAL_OK)
        {
            adc_val = HAL_ADC_GetValue(&adc_dev);
        }
        else
        {
            Error_Handler(); 
        } 
    }
}


/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
static void Error_Handler(void)
{
  while(1)
  {
  }
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 168000000
  *            HCLK(Hz)                       = 168000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 8000000
  *            PLL_M                          = 8
  *            PLL_N                          = 336
  *            PLL_P                          = 2
  *            PLL_Q                          = 7
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 5
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();
  
  /* The voltage scaling allows optimizing the power consumption when the device is 
     clocked below the maximum system frequency, to update the voltage scaling value 
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  
  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);
  
  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;  
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);

  /* STM32F405x/407x/415x/417x Revision Z devices: prefetch is supported  */
  if (HAL_GetREVID() == 0x1001)
  {
    /* Enable the Flash prefetch */
    __HAL_FLASH_PREFETCH_BUFFER_ENABLE();
  }
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
