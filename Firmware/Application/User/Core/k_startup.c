/**
  ******************************************************************************
  * @file    k_startup.c
  * @author  MCD Application Team
  * @brief   This file provides the startup functions
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

/** @defgroup KERNEL_STARTUP
  * @brief Kernel startup routines 
  * @{
  */


/* External variables --------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

#if defined ( __ICCARM__ )
#pragma location="ExtQSPIFlashSection" 
#else
__attribute__((section(".ExtQSPIFlashSection")))  
#endif


/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Startup : Shows ST logo and slogan
  * @param  None
  * @retval None
  */
void k_StartUp(void)
{
  uint32_t XPos = 35, XOffset = 5;
  const char title1[] = "Design of a Low-Cost Harmonic Analyser";
  const char title2[] = "for Industrial and Residential Networks";
  const char author[] = "By Joseph Garrone";
  const char supervisor[] = "Supervised by Dr Mithulan Nadarajah";
  const char *p = title1;

  GUI_SetBkColor(GUI_BACKGROUND);
  GUI_Clear();
  
  GUI_SetFont(&GUI_FontAvantGarde32B);
  GUI_SetColor(GUI_ACCENT);
  
  do{
    GUI_DispCharAt(*p, XPos , 100);
    XPos += GUI_GetCharDistX(*p++) + XOffset + 1;
    GUI_Delay(10);
  } while (*p != 0);

  XPos = 45;
  p = title2;

  do{
    GUI_DispCharAt(*p, XPos , 150);
    XPos += GUI_GetCharDistX(*p++) + XOffset + 1;
    GUI_Delay(10);
  } while (*p != 0);


  GUI_SetFont(&GUI_FontAvantGarde32);
  GUI_SetColor(GUI_FOREGROUND);

  p = author;
  XPos = 245;

  do{
    GUI_DispCharAt(*p, XPos , 370);
    XPos += GUI_GetCharDistX(*p++) + XOffset + 1;
    GUI_Delay(20);
  } while (*p != 0);

  p = supervisor;
  XPos = 130;

  GUI_SetFont(&GUI_FontAvantGarde24);
  GUI_SetColor(GUI_FOREGROUND_DIM);
  do{
    GUI_DispCharAt(*p, XPos , 430);
    XPos += GUI_GetCharDistX(*p++) + XOffset + 1;
    GUI_Delay(10);
  } while (*p != 0);

  GUI_Delay(2000);
}

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
