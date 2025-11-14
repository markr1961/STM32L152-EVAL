/**
  ******************************************************************************
  * @file    stm32l1xx_it.c
  * @author  MCD Application Team
  * @version V1.2.0
  * @date    24-January-2012
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * FOR MORE INFORMATION PLEASE READ CAREFULLY THE LICENSE AGREEMENT FILE
  * LOCATED IN THE ROOT DIRECTORY OF THIS FIRMWARE PACKAGE.
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usb_istr.h"

/** @addtogroup STM32L152_EVAL_Demo
  * @{
  */

/** @defgroup IT
  * @brief Main Interrupt Service Routines.
  *        This file provides template for all exceptions handler and peripherals
  *        interrupt service routine.
  * @{
  */

/** @defgroup IT_Private_Types
  * @{
  */
/**
  * @}
  */

/** @defgroup IT_Private_Defines
  * @{
  */
/**
  * @}
  */

/** @defgroup IT_Private_Macros
  * @{
  */
/**
  * @}
  */

/** @defgroup IT_Private_Variables
  * @{
  */
__IO uint8_t Index = 0, Counter1 = 0;
__IO uint32_t WaveCounter = 0;
static __IO uint8_t AlarmStatus = 0;
static __IO uint8_t LedCounter = 0;
extern uint8_t Buffer1[];
extern uint8_t Buffer2[];

uint32_t dmaindex = 0;
extern FIL F;
extern UINT BytesWritten;
extern UINT BytesRead;
extern __IO uint8_t SMbusAlertOccurred;
extern __IO uint32_t  LCDContrast[];
extern __IO uint8_t ContrastIndex;
extern WAVE_FormatTypeDef WAVE_Format;
extern __IO uint8_t LowPowerStatus;

uint32_t systick_1ms;

/**
  * @}
  */


/** @defgroup IT_Private_FunctionPrototypes
  * @{
  */
/**
  * @}
  */


/** @defgroup IT_Private_Functions
  * @{
  */

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
  uint32_t tmp = 318, index = 0;

  RCC_MSIRangeConfig(RCC_MSIRange_6);
  
  /* Disable LCD Window mode */
  LCD_WindowModeDisable();

  /* If HSE is not detected at program startup or HSE clock failed during program execution */
  if ((RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET) || (RCC_GetITStatus(RCC_IT_CSS) != RESET))
  {
    /* Clear the LCD */
    LCD_Clear(LCD_COLOR_WHITE);
    /* Set the LCD Back Color */
    LCD_SetBackColor(LCD_COLOR_BLUE);
    /* Set the LCD Text Color */
    LCD_SetTextColor(LCD_COLOR_WHITE);
    /* Display " No Clock Detected  " message */
    LCD_DisplayStringLine(LCD_LINE_0, "No HSE Clock         ");
    LCD_DisplayStringLine(LCD_LINE_1, "Detected. STANDBY    ");
    LCD_DisplayStringLine(LCD_LINE_2, "mode in few seconds. ");

    LCD_DisplayStringLine(LCD_LINE_5, "If HSE Clock         ");
    LCD_DisplayStringLine(LCD_LINE_6, "recovers before the  ");
    LCD_DisplayStringLine(LCD_LINE_7, "time out, a System   ");
    LCD_DisplayStringLine(LCD_LINE_8, "Reset is generated.  ");
    LCD_ClearLine(LCD_LINE_9);
    /* Clear Clock Security System interrupt pending bit */
    RCC_ClearITPendingBit(RCC_IT_CSS);
    GPIO_SetBits(GPIOC, GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9);
    /* Enable HSE */
    RCC_HSEConfig(RCC_HSE_ON);
    LCD_ClearLine(LCD_LINE_4);
    /* Set the Back Color */
    LCD_SetBackColor(LCD_COLOR_WHITE);
    /* Set the Text Color */
    LCD_SetTextColor(LCD_COLOR_RED);
    LCD_DrawRect(71, 319, 25, 320);
    LCD_SetBackColor(LCD_COLOR_GREEN);
    LCD_SetTextColor(LCD_COLOR_WHITE);
    /* Wait till HSE is ready */
    while (RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET)
    {
      if (index == 0xFFFF)
      {
        LCD_DisplayChar(LCD_LINE_3, tmp, 0x20);
        tmp -= 16;
        index = 0;
      }
      index++;
      /* Enters the system in STANDBY mode */
      if (tmp < 16)
      {
        LCD_SetBackColor(LCD_COLOR_BLUE);
        LCD_ClearLine(LCD_LINE_3);
        LCD_ClearLine(LCD_LINE_4);
        LCD_ClearLine(LCD_LINE_5);
        LCD_ClearLine(LCD_LINE_6);
        LCD_DisplayStringLine(LCD_LINE_7, " MCU in STANDBY Mode");
        LCD_DisplayStringLine(LCD_LINE_8, "To exit press Wakeup");
        /* Request to enter STANDBY mode */
        PWR_EnterSTANDBYMode();
      }
    }

    /* Generate a system reset */
    NVIC_SystemReset();
  }
}

/**
  * @brief  This function handles PVD Handler.
  * @param  None
  * @retval None
  */
void PVD_IRQHandler(void)
{
  EXTI_ClearITPendingBit(EXTI_Line16);

  if (PWR_GetFlagStatus(PWR_FLAG_PVDO) != RESET)
  {  
    /* VDD < PVD level */
    /* Adapt the System Clock to 16MHz and the Voltage Range to Range 2 */
    SetHCLKTo16();
    SystemCoreClockUpdate();
    TIM_SetAutoreload(TIM6, SystemCoreClock/22050);
    Demo_SysTickConfig();
    Demo_LedShowInit();
    Demo_LedShow(ENABLE);
  }
  else
  { 
    /* VDD > PVD level */
    /* Adapt the System Clock to 32MHz and the Voltage Range to Range 1 */
    SetHCLKTo32();
    SystemCoreClockUpdate();
    TIM_SetAutoreload(TIM6, SystemCoreClock/WAVE_Format.SampleRate);
    Demo_SysTickConfig();
    Demo_LedShowInit();
    Demo_LedShow(ENABLE);
  }  
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
  systick_1ms++;
  /* Decrement the TimingDelay variable */
  Demo_DecrementTimingDelay();
}


/******************************************************************************/
/*            STM32L1xx Peripherals Interrupt Handlers                        */
/******************************************************************************/

/**
  * @brief  This function handles RTC Auto wake-up interrupt request.
  * @param  None
  * @retval None
  */
void RTC_WKUP_IRQHandler(void)
{
  if (RTC_GetITStatus(RTC_IT_WUT) != RESET)
  {
    Counter1++;

    if (Counter1 < 4)
    {
      LCD_ShowTimeCalendar();
    }
    else if (Counter1 < 8)
    {
      LCD_ShowDateCalendar();
    }
    else if (Counter1 == 8)
    {
      LCD_GLASS_DisplayString(" STM32L ");
    }

    /* Reset Index to replay leds switch on sequence  */
    if (Counter1 == 12)
    {
      Counter1 = 0;
    }
    RTC_ClearITPendingBit(RTC_IT_WUT);
    EXTI_ClearITPendingBit(EXTI_Line20);
  }
}

/**
  * @brief  This function handles External lines 9 to 5 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI9_5_IRQHandler(void)
{
  if (EXTI_GetITStatus(EXTI_Line8) != RESET)
  {
    /* SEL function */
    while (STM_EVAL_PBGetState(BUTTON_SEL) == Bit_RESET);
    /* SEL function */
    Set_SELStatus();
    /* Clear the EXTI Line 8 */
    EXTI_ClearITPendingBit(EXTI_Line8);
  }

  if (EXTI_GetITStatus(EXTI_Line9) != RESET)
  {
    /* UP function */
     while (STM_EVAL_PBGetState(BUTTON_UP) == Bit_RESET);
    Set_UPStatus();
    /* Clear the EXTI Line 9 */
    EXTI_ClearITPendingBit(EXTI_Line9);
  }
  
  if (EXTI_GetITStatus(SD_DETECT_EXTI_LINE) != RESET)
  {
    /* Restore Demonstration Context. */
    IDD_Measurement_RestoreContext();

    /* Configures system clock after wake-up from STOP: enable HSE, PLL and select PLL
     as system clock source (HSE and PLL are disabled in STOP mode) */
    IDD_Measurement_SYSCLKConfig_STOP();

    /* Disable TIM6 */
    TIM_Cmd(TIM6, DISABLE);

    LCD_Clear(LCD_COLOR_WHITE);
    /* Set the Back Color */
    LCD_SetBackColor(LCD_COLOR_BLUE);
    /* Set the Text Color */
    LCD_SetTextColor(LCD_COLOR_RED);
    LCD_DisplayStringLine(LCD_LINE_2, "Err: SDCard Removed ");
    LCD_DisplayStringLine(LCD_LINE_3, "Please check SD Card");
    /* Set the Text Color */
    LCD_SetTextColor(LCD_COLOR_WHITE);
    LCD_DisplayStringLine(LCD_LINE_5, "Press JoyStick Up to");
    LCD_DisplayStringLine(LCD_LINE_6, "Restart the Demo... ");

    /* Wait until no key is pressed */
    while (Menu_ReadKey() != UP)
    {}
    /* Generate System Reset to load the new option byte values */
    NVIC_SystemReset();
  }
}

/**
  * @brief  This function handles TIM2 overflow and update interrupt request.
  * @param  None
  * @retval None
  */
void TIM2_IRQHandler(void)
{
  /* Clear the TIM2 Update pending bit */
  TIM_ClearITPendingBit(TIM2, TIM_IT_Update);

  if (AlarmStatus == 1)
  {
    if ((LedCounter & 0x01) == 0)
    {
      STM_EVAL_LEDOff(LED1);
      STM_EVAL_LEDOff(LED2);
      STM_EVAL_LEDOff(LED3);
      STM_EVAL_LEDOff(LED4);
    }
    else if ((LedCounter & 0x01) == 0x01)
    {
      STM_EVAL_LEDOn(LED1);
      STM_EVAL_LEDOn(LED2);
      STM_EVAL_LEDOn(LED3);
      STM_EVAL_LEDOn(LED4);
    }
    LedCounter++;
    if (LedCounter == 59)
    {
      AlarmStatus = 0;
      LedCounter = 0;
    }
  }
  else
  {
    /* If LedShowStatus is TRUE: enable leds toggling */
    if (Demo_Get_LedShowStatus() != 0)
    {
      switch (Index)
      {
          /* LD1 turned on, LD4 turned off */
        case 0:
        {
          STM_EVAL_LEDOff(LED1);
          STM_EVAL_LEDOn(LED4);
          Index++;
          break;
        }
        /* LD2 turned on, LD1 turned off */
        case 1:
        {
          STM_EVAL_LEDOff(LED2);
          STM_EVAL_LEDOn(LED1);
          Index++;
          break;
        }
        /* LD3 turned on, LD2 turned off */
        case 2:
        {
          STM_EVAL_LEDOff(LED3);
          STM_EVAL_LEDOn(LED2);
          Index++;
          break;
        }
        /* LD4 turned on, LD3 turned off */
        case 3:
        {
          STM_EVAL_LEDOff(LED4);
          STM_EVAL_LEDOn(LED3);
          Index++;
          break;
        }
        default:
          break;
      }
      /* Reset Index to replay leds switch on sequence  */
      if (Index == 4)
      {
        Index = 0;
      }
    }
  }
}
/**
  * @brief  This function handles External line 0 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI0_IRQHandler(void)
{
  LowPowerStatus = 0x01;

  if (EXTI_GetITStatus(KEY_BUTTON_EXTI_LINE) != RESET)
  {
    /* Clear the EXTI Line 0 */
    EXTI_ClearITPendingBit(KEY_BUTTON_EXTI_LINE);
  }
}

/**
  * @brief  This function handles External lines 15 to 10 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI15_10_IRQHandler(void)
{
  while (STM_EVAL_PBGetState(BUTTON_DOWN) == Bit_RESET)
  {
  }
  /* DOWN function */
  Set_DOWNStatus();

  /* Clear the EXTI Line 10 */
  EXTI_ClearITPendingBit(EXTI_Line10);
}

/**
  * @brief  This function handles I2C1 Error interrupt request.
  * @param  None
  * @retval None
  */
void I2C1_ER_IRQHandler(void)
{
  /* Check on I2C1 SMBALERT flag and clear it */
  if (I2C_GetITStatus(LM75_I2C, I2C_IT_SMBALERT))
  {
    I2C_ClearITPendingBit(LM75_I2C, I2C_IT_SMBALERT);
    SMbusAlertOccurred++;
  }
  
  /* Check on I2C1 Time out flag and clear it */
  if (I2C_GetITStatus(LM75_I2C, I2C_IT_TIMEOUT))
  {
    I2C_ClearITPendingBit(LM75_I2C, I2C_IT_TIMEOUT);
  }
  
  /* Check on I2C1 Arbitration Lost flag and clear it */
  if (I2C_GetITStatus(LM75_I2C, I2C_IT_ARLO))
  {
    I2C_ClearITPendingBit(LM75_I2C, I2C_IT_ARLO);
  }

  /* Check on I2C1 PEC error flag and clear it */
  if (I2C_GetITStatus(LM75_I2C, I2C_IT_PECERR))
  {
    I2C_ClearITPendingBit(LM75_I2C, I2C_IT_PECERR);
  }
  
  /* Check on I2C1 Overrun/Underrun error flag and clear it */
  if (I2C_GetITStatus(LM75_I2C, I2C_IT_OVR))
  {
    I2C_ClearITPendingBit(LM75_I2C, I2C_IT_OVR);
  }
  
  /* Check on I2C1 Acknowledge failure error flag and clear it */
  if (I2C_GetITStatus(LM75_I2C, I2C_IT_AF))
  {
    I2C_ClearITPendingBit(LM75_I2C, I2C_IT_AF);
  }
  
  /* Check on I2C1 Bus error flag and clear it */
  if (I2C_GetITStatus(LM75_I2C, I2C_IT_BERR))
  {
    I2C_ClearITPendingBit(LM75_I2C, I2C_IT_BERR);
  }
}

/**
  * @brief  This function handles RTC Alarm interrupt request.
  * @param  None
  * @retval None
  */
void RTC_Alarm_IRQHandler(void)
{
  /* Clear the Alarm A Pending Bit */
  RTC_ClearITPendingBit(RTC_IT_ALRA);

  /* Clear the Alarm B Pending Bit */
  RTC_ClearITPendingBit(RTC_IT_ALRB);

  AlarmStatus = 1;

  /* Clear the EXTI Line 17/ */
  EXTI_ClearITPendingBit(EXTI_Line17);
}

/**
  * @brief  This function handles TIM6 global interrupt request.
  * @param  None
  * @retval None
  */
void TIM6_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET)
  {
    /* Set DAC Channel1 DHR register */
    DAC_SetChannel1Data(DAC_Align_8b_R, Buffer2[WaveCounter++]);

    if (WaveCounter == _MAX_SS - 1)
    {
      WaveCounter = 0;
      f_read (&F, Buffer2, _MAX_SS, &BytesRead);
    }

    /* If we reach the WaveDataLength of the wave to play */
    if (Decrement_WaveDataLength() == 0)
    {
      /* Stop wave playing */
      WavePlayer_Stop();
    }

    /* Clear TIM6 update interrupt */
    TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
  }
}

/**
  * @brief  This function handles COMP interrupt request.
  * @param  None
  * @retval None
  */
void COMP_IRQHandler(void)
{
  COMP_InitTypeDef     COMP_InitStructure;
  
  if ( EXTI_GetITStatus(EXTI_Line22) == SET)
  {
    /* COMP2 init */
    /* In this example, the lower threshold is set to 3VREFINT/4 ~ 3 *1.22 / 4 ~ 0.915 V
       but can be changed to other available possibilities */
    COMP_InitStructure.COMP_InvertingInput = COMP_InvertingInput_3_4VREFINT;
    COMP_InitStructure.COMP_OutputSelect = COMP_OutputSelect_None;
    COMP_InitStructure.COMP_Speed = COMP_Speed_Slow;
    COMP_Init(&COMP_InitStructure);
    
    /* Delay to Comparator startup time 32MHz* 7us = 640 */
    Delay(0xFFFF);

    /* Initialize the luminosity index */
    ContrastIndex = 0x02;

    /* Check if COMP2 output level is high */
    if (COMP_GetOutputLevel(COMP_Selection_COMP2) != COMP_OutputLevel_Low)
    {
      /* Initialize the luminosity index */
      ContrastIndex = 0x01;

      /* COMP2 init */
      /* In this example, the lower threshold is set to VREFINT ~  1.22 V
         but can be changed to other available possibilities */
      COMP_InitStructure.COMP_InvertingInput = COMP_InvertingInput_VREFINT;
      COMP_Init(&COMP_InitStructure);
      
      /* Delay to Comparator startup time 32MHz* 20us = 640 */
      Delay(0xFFFF);
    
      /* Check if COMP2 output level is high */
      if (COMP_GetOutputLevel(COMP_Selection_COMP2) != COMP_OutputLevel_Low)
      {
        /* Initialize the luminosity index */
        ContrastIndex = 0x00;
      }
    }

    /*!< Enable LCD peripheral */
    LCD_Cmd(DISABLE);

    /*!< Configure the LCD Contrast */
    LCD_ContrastConfig(LCDContrast[ContrastIndex]);

    /*!< Wait Until the LCD FCR register is synchronized */
    LCD_WaitForSynchro();

    /*!< Enable LCD peripheral */
    LCD_Cmd(ENABLE);

    /* Clear EXTI Line22 (COMP2) interrupt pending bit */
    EXTI_ClearITPendingBit(EXTI_Line22);
  }
}

/**
  * @brief  This function handles USB Low Priority interrupts requests.
  * @param  None
  * @retval None
  */
void USB_LP_IRQHandler(void)
{
  USB_Istr();
}

/******************************************************************************/
/*                 STM32L1xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32l1xx_md.s).                                            */
/******************************************************************************/
/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/


/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/******************* (C) COPYRIGHT 2012 STMicroelectronics *****END OF FILE****/
