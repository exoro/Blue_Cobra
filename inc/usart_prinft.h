/**
  ******************************************************************************
  * @file    usart_printf.h
  * @author  Henrik ALexandersson
  * @version V1.0.0
  * @date    12/04/28
  * @brief   adds printf functionality to usart
  ******************************************************************************
  * @copy
  *	Feel free to copy
  *
  */ 
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USART_PRINTF_H
#define __USART_PRINTF_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
//#include "stdio.h"
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/   
/* Exported_Functions --------------------------------------------------------*/ 
//int fputc(int ch, FILE* f);
void initUsart(void);
void writeCharUSART1(char);
void writeStringUSART1(char *);
void writeCharUSART2(char);
void writeStringUSART2(char *);
#ifdef __cplusplus
}
#endif

#endif /* __USART_PRINTF_H */