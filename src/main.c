/******************** (C) COPYRIGHT 2011 Henrik Alexandersson ********************
* File Name          : main.c
* Author             : Henrik Alexandeersson
* Version            : V1.0.0
* Date               : 01/09/2011
* Description        : Main program body
********************************************************************************
* CPU @ 8Mhz -> 0.125us / CPU tick 
* 
* 1sec = 0x7A1200
* 100ms = 0xC3500
* 1ms = 0x1F40
* 100us = 0x320;      GPIO_Remap1_CAN1    GPIO_PinRemapConfig(GPIO_Remap_CAN1 , ENABLE);
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include <stm32f10x.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_usart.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_exti.h>
#include <stm32f10x_can.h>
#include <stm32f10x_it.h>
#include <stm32f10x_adc.h>
#include <stm32f10x_tim.h>
#include <stdio.h>
#include "usart_printf.h"
#include "rtc.h"
/* Private typedef -----------------------------------------------------------*/
ADC_InitTypeDef ADC_InitStructure;
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
TIM_OCInitTypeDef TIM_OCInitStructure;
GPIO_InitTypeDef GPIO_InitStructure;

EXTI_InitTypeDef EXTI_InitStructure;
//USART_InitTypeDef USART_InitStructure;
RCC_ClocksTypeDef  RCC_Clocks;
NVIC_InitTypeDef  NVIC_InitStructure;
FlagStatus StatCTS, StatLBD, StatTXE, StatRXNE, StatIDLE, StatORE, StatNE, StatFE, StatPE, StatTC;

/* Private define ------------------------------------------------------------*/

#define Delay_1s 0x7A1200
#define Delay_100ms 0xC3500
#define Delay_10ms 0x13880
#define Delay_1ms 0x1F40
#define Delay_100us 0x320
#define Delay_1us 0x50
#define SPEED_PRESCALER 6                        //0.061728395
#define false 0
#define true !false

/* GPIO Input define ------------------------------------------------------------*/
#define COBRA_DIALING           GPIO_Pin_8
#define COBRA_BOARDSWITCH       GPIO_Pin_12
#define COBRA_PULSE             GPIO_Pin_9
#define BOARD_SWITCH            GPIO_Pin_15
#define BLUETOOTH_MODE          GPIO_Pin_1

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
int timerValue = 0;
uint16_t dialed_number = 0;
uint16_t boardFlag = 0, numberIndex = 0, pulseFlag = 0, dialingFlag = 0; 
extern int state;
int counter = 0;
int number[50] = {0};


/* Private function prototypes -----------------------------------------------*/
void TIM_Config(void);
void NVIC_Config(void);
void RCC_Config(void);
void GPIO_Config(void);
void EXTI_Configuration(void);
void Delay(uint32_t);
void sendPhoneNumber(int arr[], int size);
void acceptCall(void);
void hangUp(void);
void initBT(void);


/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : main
* Description    : Main program.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

    int main()
  {
    
    
    RCC_Config();  // to read and write to AFIO_remap AFIO_clock must first be eanble
    NVIC_Config();
    TIM_Config(); 
    GPIO_Config();
    EXTI_Configuration();
    initUsart();
    //initBT();
    
              // Only for debug the clock tree  *********************************************** 
          // Put the clock configuration into RCC_APB2PeriphClockCmd 
          RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); 
          /* Output clock on MCO pin ---------------------------------------------*/ 
          GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; 
          GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
          GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
          GPIO_Init(GPIOA, &GPIO_InitStructure); 
          //  RCC_MCOConfig(RCC_MCO_HSE); // Put on MCO pin the: freq. of external crystal 
          RCC_MCOConfig(RCC_MCO_SYSCLK);  // Put on MCO pin the: System clock selected  
          // 
    //RTCInit();
    
    RCC_GetClocksFreq(&RCC_Clocks);   
    
    writeStringUSART2("\033[2J"); //clear usart 2 screen
    writeStringUSART1("\033[2J");  //clear usart 1 screen
    writeStringUSART1("     <<<<- Welcome To Hinkens Cobra Bluetooth ->>>>\n\n\r");


    
    

    while(1)
    {

        if(GPIO_ReadInputDataBit(GPIOB, COBRA_BOARDSWITCH)== false) writeStringUSART1("On Hook\n\r");
        else            //Luren är lyft
        {
            writeStringUSART1("Phone is off hook!\n\r Dial: \n\r");
            acceptCall(); //accept incoming call
            Delay(Delay_10ms);
            //for(counter = 0 ; counter < 10 ; counter++)
            while(TIM_GetITStatus(TIM2, TIM_IT_Update) == RESET) //
            {
                
                dialed_number = 0;
                while(GPIO_ReadInputDataBit(GPIOB, COBRA_DIALING)== true && GPIO_ReadInputDataBit(GPIOB, COBRA_BOARDSWITCH)== true)//Wait for user to start dialing
                {
                  //writeStringUSART1("Timer Counter: %d\r\n",TIM_GetCounter(TIM2));
                  if(state == 1 && dialingFlag == true) //check if timer has run out and user has started a call
                  {
                    writeStringUSART1("time is up dialing number: ");
                    for (int x = 0; x < counter; x++)
                    {
                      //printf("%d", number[x] );
                    }
                    sendPhoneNumber(number, counter);
                    writeStringUSART1("\r\n");
                    dialingFlag = false;
                    state =  0;
                    counter = 0;
                  }
                    
                }
                Delay(Delay_100ms); //Wait for switch to debounce
          
                while(GPIO_ReadInputDataBit(GPIOB, COBRA_DIALING) == false)
                {    
                     dialingFlag = true;
                     state = 0;
                     TIM_SetCounter(TIM2, 0);
                     TIM_Cmd(TIM2, ENABLE); //Start timer
                     
                     //RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
                     Delay(Delay_10ms);
                     Delay(Delay_10ms);
                      //Now count how many times the mechnical switch toggles
                     if(GPIO_ReadInputDataBit(GPIOB, COBRA_PULSE)== false && pulseFlag == 1)
                     {
                       pulseFlag = 0;
                       dialed_number ++;
                     }
                     else if(GPIO_ReadInputDataBit(GPIOB, COBRA_PULSE) == true && pulseFlag == 0)
                     {
                       pulseFlag = 1;
                        //Do notthing just idle..
                     }
                     
                  Delay(Delay_10ms);   

                }
                Delay(Delay_100ms);
                if(dialed_number != 0) // to handle the error of a extra loop after dialed number
                {
                  dialed_number--; //Rotary always has one extra closure that must be taken off
                  number[counter] = dialed_number;
                  counter ++;
                }

                if(GPIO_ReadInputDataBit(GPIOB, COBRA_BOARDSWITCH)== false) //Hunng upp
                {
                  hangUp(); //hang-up command to bluettooh
                  writeStringUSART1("Hung up!\n\r");
                  counter = 0;
                  break;
                }

              
                //printf("%d\n\r", dialed_number);
                    
            }//end for loop
        }
        
        Delay(Delay_100ms);
    
    }//end while(1)

  }//end main()
  
/*******************************************************************************
* Function Name  : initBT
* Description    : configs bluetooth
* Input          : void
* Output         : None
* Return         : None
*******************************************************************************/  
void initBT(void)
{
  GPIO_WriteBit(GPIOA, BLUETOOTH_MODE, Bit_RESET); // set bluetooth in command mode
  Delay(Delay_10ms);
  writeStringUSART2("SN,BLUE-COBRA"); //Send command type "A" = dial number
  
  Delay(Delay_100ms);
  writeStringUSART2("\r\n");
  Delay(Delay_10ms);
  GPIO_WriteBit(GPIOA, BLUETOOTH_MODE, Bit_SET);
  writeStringUSART1("SN,BLUE-COBRA\r\n"); //Send command type "A" = dial number
}
/*******************************************************************************
* Function Name  : sendPhoneNumber
* Description    : sends phonenumber to bluetooth
* Input          : int array, int size
* Output         : None
* Return         : None
*******************************************************************************/  
void sendPhoneNumber(int arr[], int size)
{
  char str[1];
  GPIO_WriteBit(GPIOA, BLUETOOTH_MODE, Bit_RESET); // set bluetooth in command mode
  Delay(Delay_10ms);
  writeStringUSART2("A,"); //Send command type "A" = dial number
  writeStringUSART1("Sending phone number command: A,");
  
  for (int x = 0; x < size; x++)
  {
   sprintf(str, "%d", arr[x] ); // int to array 
   writeStringUSART2(str); //Send phone number one by one
   writeStringUSART1(str);
  }
  Delay(Delay_100ms);
  writeStringUSART2("\r\n");
  writeStringUSART1("\r\n");
   
}
/*******************************************************************************
* Function Name  : acceptCall
* Description    : sends accept incomming call command to bluetooth
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/ 
void acceptCall(void)
{  
  GPIO_WriteBit(GPIOA, BLUETOOTH_MODE, Bit_RESET); // set bluetooth in command mode
  Delay(Delay_10ms);
  writeStringUSART2("C");
  Delay(Delay_100ms);
  writeStringUSART2("\r\n");
  Delay(Delay_10ms);
  writeStringUSART1("Sending Answer Command \r\n");
  GPIO_WriteBit(GPIOA, BLUETOOTH_MODE, Bit_SET);
}
/*******************************************************************************
* Function Name  : hangUp
* Description    : sends hang-up command to bluetooth
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void hangUp(void)
{

  GPIO_WriteBit(GPIOA, BLUETOOTH_MODE, Bit_RESET); // set bluetooth in command mode
  Delay(Delay_10ms);
  writeStringUSART2("E");
  Delay(Delay_100ms);
  writeStringUSART2("\r\n");
  Delay(Delay_10ms);
  GPIO_WriteBit(GPIOA, BLUETOOTH_MODE, Bit_SET);
  writeStringUSART1("Send hang-up command");
}
/*******************************************************************************
* Function Name  : NVIC_Config
* Description    : Configures the NVIC for interuppts.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NVIC_Config(void)
{

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
  
  
    /* Enable the USART! Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
  
    /* Enable TIM2 Interrupt  */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

     /* Enable the EXTI9_5 Interrupt */
 /*   NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);*/
    
     /* Enable the EXTI15_10 Interrupt */
    /*NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);*/

}
/*******************************************************************************
* Function Name  : RCC_Config
* Description    : Set up Periph clocks for GPIO, CAN, USART.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RCC_Config(void)
{
    SystemInit();
    /* GPIO Periph clock enable */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOC, ENABLE); /* Enable GPIO clock. Går att stänga av för att spara ström och förbättra EMC egenskaper men behöver slås på efter reset för att aktivera porten*/
  
    /* USART GPIO Periph clock enable */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
  
    
    //Enable ADC1 clock
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    
    // Enable TIM2
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
}
/*******************************************************************************
* Function Name  : GPIO_Config
* Description    : Set up GPIO ports.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void GPIO_Config(void)
 {

    /* Configure PB.05 - board LED as output push-pull */
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_5; //| GPIO_Pin_1; /* Vilka pinnar vi vill påverka för denna inställning */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; /* Vilken typ vi vill ställa in pinnarna till */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; /* Högsta arbetshastighet för pinnarna, lägre hastighet bättre EMC egenskaper */
    GPIO_Init(GPIOB, &GPIO_InitStructure); /* Överför och aktivera själva inställningen vi satt upp ovan i GPIO_InitStructure */
    
    /* Configure PB.15 - board Switch as input pull up */
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_15; /* Vilka pinnar vi vill påverka för denna inställning */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; /* Vilken typ vi vill ställa in pinnarna till */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; /* Högsta arbetshastighet för pinnarna, lägre hastighet bättre EMC egenskaper */
    GPIO_Init(GPIOB, &GPIO_InitStructure); /* Överför och aktivera själva inställningen vi satt upp ovan i GPIO_InitStructure */
    
    /* Configure PB.12 - Cobra enable signal as intput push-pull */
    GPIO_InitStructure.GPIO_Pin =  COBRA_BOARDSWITCH; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
    GPIO_Init(GPIOB, &GPIO_InitStructure); 
    
    //OOOOBBBBBBSSSS need to be change to PB09
    /* Configure PB.09 - Cobra pulse signal as input pull up */
    GPIO_InitStructure.GPIO_Pin = COBRA_PULSE;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    /* Configure PB.08 - Cobra dialing signal as input pull up */
    GPIO_InitStructure.GPIO_Pin = COBRA_DIALING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
 
    /* Configure PA.01 - Bluetooth mode IPU */
    GPIO_InitStructure.GPIO_Pin = BLUETOOTH_MODE;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
        /* Configure PC.4, PC.5 - Analog Input */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
   }
  
  
  /*******************************************************************************
* unction Name : EXTI_Configuration.
* Description   : Configure the EXTI lines for Key and Tamper push buttons.
* Input         : None.
* Output        : None.
* Return value  : The direction value.
******************************************************************************/
void EXTI_Configuration(void)
{
    /* Connect Button EXTI Line to Button GPIO Pin */
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource14);  //GPIOb14
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource8);

  /* Configure EXTI_Line8 line to generate an interrupt on rising & falling edges */  
  EXTI_InitStructure.EXTI_Line = EXTI_Line8;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  /* Clear the EXTI_Line1 line pending bit */
  EXTI_ClearITPendingBit(EXTI_Line8);
  
  /* Configure EXTI_Line14 line to generate an interrupt on rising & falling edges */  
  EXTI_InitStructure.EXTI_Line = EXTI_Line14;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  /* Clear the EXTI_Line1 line pending bit */
  EXTI_ClearITPendingBit(EXTI_Line14);
}

/*******************************************************************************
* Function Name  : TIM_Config
* Description    : Setup TIM1 timer interrupt 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM_Config()
{
    
    /* TIM2 configuration */
    TIM_TimeBaseStructure.TIM_Period = 4000;
    TIM_TimeBaseStructure.TIM_Prescaler = 7200; //~8.5 sec timer
    TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;    
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x0000;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    //TIM_OCStructInit(&TIM_OCInitStructure);

    /* Output Compare Timing Mode configuration: Channel1 */
    //TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;
    //TIM_OCInitStructure.TIM_Pulse = 0x0;  
    //TIM_OC1Init(TIM2, &TIM_OCInitStructure);

    /* TIM2 enable counter */
    //TIM_Cmd(TIM2, ENABLE);

    /* Immediate load of TIM2 Precaler value */
    TIM_PrescalerConfig(TIM2, ((SystemCoreClock/1200) - 1), TIM_PSCReloadMode_Immediate);

    /* Clear TIM2 update pending flag */
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);

    /* Enable TIM2 Update interrupt */
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
}
    
/*******************************************************************************
* Function Name  : Delay
* Description    : Performs a delay loop.
* Input          : uint32_t number of loops to delay 
* Output         : None
* Return         : None
*******************************************************************************/
void Delay(uint32_t tick)
{
  uint32_t i = 0;
  for (i = 0; i < tick; i++)
  {
    asm("nop");
  }
  
}





/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Uncomment the line below to expanse the "assert_param" macro in the 
   Standard Peripheral Library drivers code */
 /*#define USE_FULL_ASSERT    1 */

/* Exported macro ------------------------------------------------------------*/
#ifdef  USE_FULL_ASSERT

/*******************************************************************************
* Macro Name     : assert_param
* Description    : The assert_param macro is used for function's parameters check.
* Input          : - expr: If expr is false, it calls assert_failed function
*                    which reports the name of the source file and the source
*                    line number of the call that failed. 
*                    If expr is true, it returns no value.
* Return         : None
*******************************************************************************/ 
  #define assert_param(expr) ((expr) ? (void)0 : assert_failed((uint8_t *)__FILE__, __LINE__))
/* Exported functions ------------------------------------------------------- */
  void assert_failed(uint8_t* file, uint32_t line);
#else
  #define assert_param(expr) ((void)0)
#endif /* USE_FULL_ASSERT */


/******************* (C) COPYRIGHT 2011 HenrikLAlexandersson *****END OF FILE****/