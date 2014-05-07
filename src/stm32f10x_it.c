/**
  ******************************************************************************
  * @file    CAN/Normal/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.1.2
  * @date    09/28/2009
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2009 STMicroelectronics</center></h2>
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "dtmf.h"
#include "usart_printf.h"
#include <stm32f10x_exti.h>

/** @addtogroup STM32F10x_StdPeriph_Examples
  * @{
  */

/** @addtogroup CAN_Normal
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
char inValue = 20;
int *state_ptr;
int state = 0;
 
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
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
  {
  }
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
  {
  }
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
  {
  }
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
  {
  }
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
//void SysTick_Handler(void)
//{
//}

/******************************************************************************/
/*            STM32F10x Peripherals Interrupt Handlers                        */
/******************************************************************************/

/*******************************************************************************
* Function Name  : EXTI9_5_IRQHandler
* Description    : This function handles External lines 15 to 10 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI9_5_IRQHandler(void)
{
  /*state_ptr = &state;
  uint16_t buff = 0;
   for(int counter = 0 ; counter < 10 ; counter++)
   {
     buff += GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_8); 
   }
   //printf("BUFF == %d\n\r", buff);
   if(buff == 0)*state_ptr = 1 ; //stable low
   else if(buff == 10)*state_ptr = 2; //stable high
   else if (buff != 0 || buff !=10)*state_ptr = 0; //unstable*/
  EXTI_ClearITPendingBit(EXTI_Line8);
}
    
/*******************************************************************************
* Function Name  : EXTI15_10_IRQHandler
* Description    : This function handles External lines 15 to 10 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI15_10_IRQHandler(void)
{
  /*state_ptr = &state;
  uint16_t buff = 0;
   for(int counter = 0 ; counter < 10 ; counter++)
   {
     buff += GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14); 
   }
   printf("BUFF == %d\n\r", buff);
   if(buff == 0)*state_ptr = 1 ; //stable low
   else if(buff == 10)*state_ptr = 2; //stable high
   else if (buff != 0 || buff !=10)*state_ptr = 0; //unstable*/
  EXTI_ClearITPendingBit(EXTI_Line14);
}
 /*******************************************************************************
* Function Name  : TIM2 Intterupt handler
* Description    : .
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM2_IRQHandler()
{
        state = 0;
	static int i = 0;
        if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
        {
          state = 1;
          TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
          
          // Blinking
          if (i == 0) 
          {
            GPIO_WriteBit(GPIOB, GPIO_Pin_5, Bit_RESET);
            i = 1;
          } 
          
          else 
          {
            GPIO_WriteBit(GPIOB, GPIO_Pin_5, Bit_SET);
            i = 0;
          }
        
        }
        
	
}

/**
  * @brief  This function handles USART1 global interrupt request.
  * @param  None
  * @retval None
  */


void USART1_IRQHandler(void)
{
  
  
  if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
  {
    /* Read one byte from the receive data register */
     inValue = USART_ReceiveData(USART1);

  }
  if (inValue == 'q')
  {
    writeStringUSART1("interrupt funkar\r\n");
    GPIO_WriteBit(GPIOA, GPIO_Pin_1, Bit_RESET); // set bluetooth in command mode
    
    for (uint32_t i = 0; i < 0x13880; i++)
    {
      asm("nop");
    }
    //writeStringUSART2("AT+"); //Send command type "A" = dial number
    writeStringUSART2("SK,08");
  
    for (uint32_t i = 0; i < 0xC3500; i++)
    {
      asm("nop");
    }
    writeStringUSART2("\r\n");
    
    for (uint32_t i = 0; i < 0x13880; i++)
    {
      asm("nop");
    }
    GPIO_WriteBit(GPIOA, GPIO_Pin_1, Bit_SET);
    writeStringUSART1("Increase volume\r\n"); //Send command type "A" = dial number
    
    writeStringUSART1("interrupt funkar\r\n");
    GPIO_WriteBit(GPIOA, GPIO_Pin_1, Bit_RESET); // set bluetooth in command mode
    
    for (uint32_t i = 0; i < 0x13880; i++)
    {
      asm("nop");
    }
    //writeStringUSART2("AT+"); //Send command type "A" = dial number
    writeStringUSART2("SD,08");
  
    for (uint32_t i = 0; i < 0xC3500; i++)
    {
      asm("nop");
    }
    writeStringUSART2("\r\n");
    
    for (uint32_t i = 0; i < 0x13880; i++)
    {
      asm("nop");
    }
    GPIO_WriteBit(GPIOA, GPIO_Pin_1, Bit_SET);
    writeStringUSART1("Increase volume\r\n"); //Send command type "A" = dial number
  }
  
  if (inValue == 'w')
  {
    writeStringUSART1("interrupt funkar\r\n");
    GPIO_WriteBit(GPIOA, GPIO_Pin_1, Bit_RESET); // set bluetooth in command mode
    
    for (uint32_t i = 0; i < 0x13880; i++)
    {
      asm("nop");
    }
    writeStringUSART2("AV, 15"); //Send command type "A" = dial number
  
    for (uint32_t i = 0; i < 0xC3500; i++)
    {
      asm("nop");
    }
    writeStringUSART2("\r\n");
    
    for (uint32_t i = 0; i < 0x13880; i++)
    {
      asm("nop");
    }
    
    GPIO_WriteBit(GPIOA, GPIO_Pin_1, Bit_SET);
    writeStringUSART1("Increase speech volume\r\n"); //Send command type "A" = dial number
    
  }
  //printf("Input: %c\n\r", inValue);
  //writeStringUSART2("D\r\n");

  //USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
  //dtmf_set(DTMF_3);
  
  
  
}
/**
  * @brief  This function handles USART1 global interrupt request.
  * @param  None
  * @retval None
  */


void USART2_IRQHandler(void)
{
  
  
  if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
  {
    /* Read one byte from the receive data register */
     inValue = USART_ReceiveData(USART2);

  }
  
  //printf("Input: %c\n\r", inValue);

  //USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
  //dtmf_set(DTMF_3);
  
  
  
}
/**
  * @brief  This function handles CAN1 global RX interrupt.
  * @param  None
  * @retval None
  */
void USB_LP_CAN1_RX0_IRQHandler(void)
{

}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
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


/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
