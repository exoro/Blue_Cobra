/*******************************************************************************
* File Name          : usart_printf.c
* Author             : Henrik Alexandersson
* Version            : V1.0.0
* Date               : 2012/05/14
* Description        : setup USART and enable printf() 
*                      
********************************************************************************
  * @copy
  *	Feel free to copy
  *
  */ 

/* Includes ------------------------------------------------------------------*/
#include "usart_printf.h"
#include <stm32f10x_usart.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/



void initUsart()
{
    USART_InitTypeDef USART_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef  NVIC_InitStructure;
    
    /* USART GPIO Periph clock enable */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
  
    /* Start USART clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
  
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
  
    /* Enable the USART1 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    /* Enable the USART2 Interrupt */   
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
         /* USARTx configured as follow:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
         */
    
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    
  
     
      /* Configure USART1 Tx as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
      
    /* Configure USART1 Rx as input floating */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
      /* Configure USART2 Tx as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
      
    /* Configure USART2 Rx as input floating */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    /* Enable USART1 Receive */
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    /* Enable USART2 Receive */
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    
    /* USART1 configuration */
    USART_Init(USART1, &USART_InitStructure);
    /* USART2 configuration */
    USART_Init(USART2, &USART_InitStructure);
      
    /* Enable USART1 */
    USART_Cmd(USART1, ENABLE);
    /* Enable USART2 */
    USART_Cmd(USART2, ENABLE);
      
}
/*******************************************************************************
* Function Name  : writeCharUSART2
* Description    : Writes a char to USART and checks that writing is done.
* Input          : uint16_t char to write
* Output         : None
* Return         : None
*******************************************************************************/
void writeCharUSART2(char ch)
{
  while(!USART_GetFlagStatus(USART2, USART_FLAG_TC)){}
  USART_SendData(USART2, ch);
}

void writeStringUSART2(char *ch)
{
  while(*ch != '\0')
  {
    writeCharUSART2(*ch);
    ch++;
  }
}
/*******************************************************************************
* Function Name  : writeCharUSART1
* Description    : Writes a char to USART and checks that writing is done.
* Input          : uint16_t char to write
* Output         : None
* Return         : None
*******************************************************************************/
void writeCharUSART1(char ch)
{
  while(!USART_GetFlagStatus(USART1, USART_FLAG_TC)){}
  USART_SendData(USART1, ch);
}

void writeStringUSART1(char *ch)
{
  while(*ch != '\0')
  {
    writeCharUSART1(*ch);
    ch++;
  }
}
/*******************************************************************************
* Function Name  : fputc 
* Description    : redirect printf() to USART1
* Input          : Standard printf function
* Output         : None
* Return         : None
*******************************************************************************/
/*int fputc(int ch, FILE* f)
{
    // Place your implementation of fputc here 
    // e.g. write a character to the USART 
    USART_SendData(USART1, (uint8_t) ch);
  
    // Loop until the end of transmission 
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
    {}
  
    return ch;
}*/  

/************************END OF FILE*******************************************/