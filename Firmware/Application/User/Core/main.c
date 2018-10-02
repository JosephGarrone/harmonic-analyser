/**
  ******************************************************************************
  * @file    main.c 
  * @author  MCD Application Team
  * @brief   This file provides main program functions
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V. 
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h" 

/** @addtogroup CORE
  * @{
  */

/** @defgroup MAIN
* @brief main file
* @{
*/ 

/** @defgroup MAIN_Private_TypesDefinitions
* @{
*/ 
#ifdef INCLUDE_THIRD_PARTY_MODULE 
typedef  void (*pFunc)(void);
#endif
/**
* @}
*/ 

/** @defgroup MAIN_Private_Defines
* @{
*/ 
/**
* @}
*/ 


/** @defgroup MAIN_Private_Macros
* @{
*/ 
/**
* @}
*/ 


/** @defgroup MAIN_Private_Variables
* @{
*/
/**
* @}
*/ 


/** @defgroup MAIN_Private_FunctionPrototypes
* @{
*/ 

// ADC SPI Definitions
#define ADC_SPI 		  SPI2
#define ADC_CLK_ENABLE()  __HAL_RCC_SPI2_CLK_ENABLE()
#define ADC_SCK_ENABLE()  __HAL_RCC_GPIOD_CLK_ENABLE()
#define ADC_MISO_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE()
#define ADC_MOSI_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE()

#define ADC_SPI_RESET()	  __HAL_RCC_SPI2_FORCE_RESET()
#define ADC_SPI_RELEASE() __HAL_RCC_SPI2_RELEASE_RESET()

#define ADC_SCK_PIN	  	  GPIO_PIN_3
#define ADC_SCK_PORT      GPIOD
#define ADC_SCK_AF	      GPIO_AF5_SPI2
#define ADC_MISO_PIN	  GPIO_PIN_14
#define ADC_MISO_PORT     GPIOB
#define ADC_MISO_AF		  GPIO_AF5_SPI2
#define ADC_MOSI_PIN 	  GPIO_PIN_15
#define ADC_MOSI_PORT     GPIOB
#define ADC_MOSI_AF		  GPIO_AF5_SPI2

SPI_HandleTypeDef SpiHandle;

static void SystemClock_Config(void);
static void GUIThread(void const * argument);
static void TimerCallback(void const *n);

extern K_ModuleItem_Typedef  settings_board;
extern K_ModuleItem_Typedef  harmonics_board;
extern K_ModuleItem_Typedef  sys_info_board;

osTimerId lcd_timer;

uint8_t ucHeap[ configTOTAL_HEAP_SIZE ];
/**
* @}
*/ 

/** @defgroup MAIN_Private_Functions
* @{
*/ 

/**
* @brief  Main program
* @param  None
* @retval int
*/
int main(void)
{     
  /* STM32F4xx HAL library initialization:
  - Configure the Flash ART accelerator on ITCM interface
  - Configure the Systick to generate an interrupt each 1 msec
  - Set NVIC Group Priority to 4
  - Global MSP (MCU Support Package) initialization
  */

  HAL_Init();
  
  /* Configure the system clock @ 180 Mhz */
  SystemClock_Config();

  /* Initialise BSP */
  k_BspInit(); 
  
  SpiHandle.Instance = 				 ADC_SPI;
  SpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
  SpiHandle.Init.Direction =	     SPI_DIRECTION_2LINES;
  SpiHandle.Init.CLKPhase = 		 SPI_PHASE_1EDGE;   // SPI Mode 3
  SpiHandle.Init.CLKPolarity = 		 SPI_POLARITY_HIGH; // SPI Mode 3
  SpiHandle.Init.DataSize = 		 SPI_DATASIZE_8BIT;
  SpiHandle.Init.TIMode = 			 SPI_TIMODE_DISABLE;
  SpiHandle.Init.CRCCalculation = 	 SPI_CRCCALCULATION_DISABLE;
  SpiHandle.Init.CRCPolynomial = 	 7;
  SpiHandle.Init.NSS = 				 SPI_NSS_SOFT;
  SpiHandle.Init.Mode = 			 SPI_MODE_MASTER;
  if (HAL_SPI_Init(&SpiHandle) != HAL_OK)
  {
	while(1);
  }

  /* Initialize RTC */
  k_CalendarBkupInit();


  GPIO_InitTypeDef   GPIO_InitStructure;
  __HAL_RCC_GPIOC_CLK_ENABLE();
  GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStructure.Pull = GPIO_PULLDOWN;
  GPIO_InitStructure.Pin = GPIO_PIN_6;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);
  GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStructure.Pull = GPIO_PULLDOWN;
  GPIO_InitStructure.Pin = GPIO_PIN_8;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);

  /* Setup the ADC */
  setup_adc();
  
  /* Create GUI task */
  osThreadDef(GUI_Thread, GUIThread, osPriorityLow, 0, 2048);
  osThreadCreate (osThread(GUI_Thread), NULL); 
  
  /* Add Modules*/
  k_ModuleInit();
  
  /* Link modules */ 
  k_ModuleAdd(&harmonics_board);
  k_ModuleAdd(&settings_board);
  k_ModuleAdd(&sys_info_board);

  /* Start scheduler */
  osKernelStart ();

  /* We should never get here as control is now taken by the scheduler */
  for( ;; );
}

/**
  * @brief  Start task
  * @param  argument: pointer that is passed to the thread function as start argument.
  * @retval None
  */
static void GUIThread(void const * argument)
{   
  /* Initialize Storage Units */
  k_StorageInit();
  
  /* Initialize GUI */
  GUI_Init();   
  
  WM_MULTIBUF_Enable(1);
  GUI_SetLayerVisEx (1, 0);
  GUI_SelectLayer(0);
  
  GUI_SetBkColor(GUI_BACKGROUND);
  GUI_Clear();  
  
   /* Set General Graphical proprieties */
  k_SetGuiProfile();

  k_StartUp();

  /* Create Touch screen Timer */
  osTimerDef(TS_Timer, TimerCallback);
  lcd_timer =  osTimerCreate(osTimer(TS_Timer), osTimerPeriodic, (void *)0);

  /* Start the TS Timer */
  osTimerStart(lcd_timer, 30);
  
  /* Show the main menu */
  k_InitMenu();
    
  /* Gui background Task */
  while(1) {
    GUI_Exec(); /* Do the background work ... Update windows etc.) */
    osDelay(30); /* Nothing left to do for the moment ... Idle processing */
  }
}

/**
  * @brief  Timer callbacsk (40 ms)
  * @param  n: Timer index 
  * @retval None
  */
static void TimerCallback(void const *n)
{  
  k_TouchUpdate();
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 180000000
  *            HCLK(Hz)                       = 180000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 8000000
  *            PLL_M                          = 8
  *            PLL_N                          = 360
  *            PLL_P                          = 2
  *            PLL_Q                          = 7
  *            PLL_R                          = 6
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
  
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

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
  RCC_OscInitStruct.PLL.PLLN = 360;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  RCC_OscInitStruct.PLL.PLLR = 6;
  
  
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    while(1);
  }
  /* Enable the OverDrive to reach the 180 Mhz Frequency */  
  if(HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    while(1);
  }
  
  /* Select PLLSAI output as USB clock source */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_CK48;
  PeriphClkInitStruct.Clk48ClockSelection = RCC_CK48CLKSOURCE_PLLSAIP; 
  PeriphClkInitStruct.PLLSAI.PLLSAIN = 384;
  PeriphClkInitStruct.PLLSAI.PLLSAIP = RCC_PLLSAIP_DIV8;
  HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);

  
  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
  clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;  

  
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    while(1);
  }
}

// Initialise SPI
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi) {
	GPIO_InitTypeDef GPIO_InitStruct;

	if (hspi->Instance == ADC_SPI) {
		ADC_SCK_ENABLE();
		ADC_MISO_ENABLE();
		ADC_MOSI_ENABLE();
		ADC_CLK_ENABLE();

		GPIO_InitStruct.Pin = 		ADC_SCK_PIN;
		GPIO_InitStruct.Mode = 		GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = 		GPIO_PULLDOWN;
		GPIO_InitStruct.Speed = 	GPIO_SPEED_HIGH;
		GPIO_InitStruct.Alternate = ADC_SCK_AF;
		HAL_GPIO_Init(ADC_SCK_PORT, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = 		ADC_MISO_PIN;
		GPIO_InitStruct.Alternate = ADC_MISO_AF;
		HAL_GPIO_Init(ADC_MISO_PORT, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = 		ADC_MOSI_PIN;
		GPIO_InitStruct.Alternate = ADC_MOSI_AF;
		HAL_GPIO_Init(ADC_MOSI_PORT, &GPIO_InitStruct);
	}
}

// Uninitialise SPI
void HAL_SPI_MspDeInit(SPI_HandleTypeDef *hspi) {
	if (hspi->Instance == ADC_SPI) {
		ADC_FORCE_RESET();
		ADC_SPI_RELEASE();

		HAL_GPIO_DeInit(ADC_SCK_PORT, ADC_SCK_PIN);
		HAL_GPIO_DeInit(ADC_MISO_PORT, ADC_MISO_PIN);
		HAL_GPIO_DeInit(ADC_MOSI_PORT, ADC_MOSI_PIN);
	}
}

#ifdef USE_FULL_ASSERT
/**
* @brief  assert_failed
*         Reports the name of the source file and the source line number
*         where the assert_param error has occurred.
* @param  File: pointer to the source file name
* @param  Line: assert_param error line source number
* @retval None
*/
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line
  number,ex: printf("Wrong parameters value: file %s on line %d\r\n", 
  file, line) */
  
  /* Infinite loop */
  while (1)
  {}
}

#endif


/**
* @}
*/ 

/**
* @}
*/ 

/**
* @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
