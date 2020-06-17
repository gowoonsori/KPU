/********************************************************************************/
/* main.c                                                                       */
/* STM32F103VET6                                                                */
/* (Lee ChangWoo HL2IRW  hl2irw@kpu.ac.kr 011-726-6860)                 	*/
/* stm32f103ve_mp3								*/
/********************************************************************************/
#include "hwdefs.h"
#include <stdlib.h>
#include <math.h>

int option_value;
FLASH_Status FLASHStatus = FLASH_COMPLETE;

volatile unsigned short tick,jiffes,sec_tick,second,volume=64;
volatile unsigned char sw_value,sw_cnt1,sw_cnt2,sw_cnt3,sw_cnt4,sw_cnt5;
volatile unsigned char play_mp3,volume_flag;

extern volatile unsigned short rxcnt1,rxcnt2;
extern volatile unsigned char rxck1,rxck2,rxled,txled;

unsigned char fileBuff[512];
unsigned short xLoc, yLoc;
unsigned short old_volume;

int go_back(unsigned short x,unsigned short y);

void wait_ms (unsigned short delay)
{
      jiffes = 0;
      while (jiffes < delay) {
      }
}


void led_control (unsigned short led, unsigned short ctl)
{
      if (ctl == ON) {
         GPIO_SetBits(GPIOD,led);
      } else {
         GPIO_ResetBits(GPIOD,led);
      }
}


void Periph_Configuration (void)
{
      /* ADCCLK = PCLK2/6 */
      RCC_ADCCLKConfig(RCC_PCLK2_Div6);
      /* Enable GPIO and AFIO clocks */
      RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOF | RCC_APB2Periph_GPIOG | RCC_APB2Periph_AFIO, ENABLE);
      /* Enable USART1 clock */
      RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
      /* Enable USART2 clock */
      RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
      /* Enable ADC1, ADC2, ADC3 clock */
      RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_ADC2 | RCC_APB2Periph_ADC3, ENABLE);
      /* Enable TIM4 clock */
      RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
}


void GPIO_Configuration (void)
{
      // Port A
      GPIO_Init_Pin(GPIOA,TXD2,GPIO_Speed_50MHz,GPIO_Mode_AF_PP);
      GPIO_Init_Pin(GPIOA,RXD2,GPIO_Speed_50MHz,GPIO_Mode_IN_FLOATING);
      GPIO_Init_Pin(GPIOA,TXD1,GPIO_Speed_50MHz,GPIO_Mode_AF_PP);
      GPIO_Init_Pin(GPIOA,RXD1,GPIO_Speed_50MHz,GPIO_Mode_IN_FLOATING);
      // Port B
      // Port C
      // PORT D
      GPIO_Init_Pin(GPIOD,LED0,GPIO_Speed_50MHz,GPIO_Mode_Out_PP);
      GPIO_Init_Pin(GPIOD,LED1,GPIO_Speed_50MHz,GPIO_Mode_Out_PP);
      GPIO_Init_Pin(GPIOD,LED2,GPIO_Speed_50MHz,GPIO_Mode_Out_PP);
      GPIO_Init_Pin(GPIOD,LED3,GPIO_Speed_50MHz,GPIO_Mode_Out_PP);

      GPIO_Init_Pin(GPIOD,GPIO_Pin_0,GPIO_Speed_50MHz,GPIO_Mode_IPU);
      GPIO_Init_Pin(GPIOD,GPIO_Pin_1,GPIO_Speed_50MHz,GPIO_Mode_IPU);
      GPIO_Init_Pin(GPIOD,GPIO_Pin_2,GPIO_Speed_50MHz,GPIO_Mode_IPU);
      GPIO_Init_Pin(GPIOD,GPIO_Pin_3,GPIO_Speed_50MHz,GPIO_Mode_IPU);
      GPIO_Init_Pin(GPIOD,GPIO_Pin_4,GPIO_Speed_50MHz,GPIO_Mode_IPU);

      // Default Value
      led_control(LED0,OFF);
      led_control(LED1,OFF);
      led_control(LED2,OFF);
      led_control(LED3,OFF);
}


#ifdef VECT_TAB_RAM
/* vector-offset (TBLOFF) from bottom of SRAM. defined in linker script */
extern unsigned int _isr_vectorsram_offs;
void NVIC_Configuration (void)
{
      /* Set the Vector Table base location at 0x20000000+_isr_vectorsram_offs */
      NVIC_SetVectorTable(NVIC_VectTab_RAM, (unsigned int)&_isr_vectorsram_offs);
}
#else
extern unsigned int _isr_vectorsflash_offs;
void NVIC_Configuration (void)
{
      /* Set the Vector Table base location at 0x08000000+_isr_vectorsflash_offs */
      NVIC_SetVectorTable(NVIC_VectTab_FLASH, (unsigned int)&_isr_vectorsflash_offs);
}
#endif /* VECT_TAB_RAM */


RAMFUNC void SysTick_Handler (void)
{
      static unsigned short cnt = 0;
      static unsigned char flip = 0;
      cnt++;
      if (cnt >= 500) {
         cnt = 0;
         /* alive sign */
         if (flip) {
            led_control(LED0,ON);
         } else {
            led_control(LED0,OFF);
            sec_tick = 1;
         }
      	 flip = !flip;
      }
      tick++;
      if (rxcnt1) rxck1++;
      if (rxcnt2) rxck2++;
      jiffes++;
}


void TIM4_IRQHandler (void)
{
      TIM_ClearITPendingBit(TIM4,TIM_IT_Update);
      static unsigned short cnt = 0;
      static unsigned char flip = 0;
      cnt++;
      if (cnt >= 500) {
         cnt = 0;
         /* alive sign */
         if (flip) {
            led_control(LED0,ON);
         } else {
            led_control(LED0,OFF);
            sec_tick = 1;
         }
      	 flip = !flip;
      }
      tick++;
      if (rxcnt1) rxck1++;
      if (rxcnt2) rxck2++;
      jiffes++;
}


void adc_init (void)
{
      ADC_InitTypeDef ADC_InitStructure;
      GPIO_Init_Pin(GPIOC,GPIO_Pin_5,GPIO_Speed_50MHz,GPIO_Mode_AIN);
      /* ADC1 registers reset */
      ADC_DeInit(ADC1);
      /* Enable ADC1 */
      ADC_Cmd(ADC1, ENABLE);
      /* ADC1 configuration */
      ADC_StructInit(&ADC_InitStructure);
      ADC_InitStructure.ADC_Mode = ADC_Mode_InjecSimult;
      ADC_InitStructure.ADC_ScanConvMode = ENABLE;
      ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
      ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
      ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
      ADC_InitStructure.ADC_NbrOfChannel = 1;
      ADC_Init(ADC1, &ADC_InitStructure);
      //Start calibration of ADC1
      ADC_StartCalibration(ADC1);
      // Wait for the end of ADCs calibration
      while (ADC_GetCalibrationStatus(ADC1)) {
      }
      /* ADC1 Injected conversions configuration */
      ADC_InjectedSequencerLengthConfig(ADC1,1);
      ADC_InjectedChannelConfig(ADC1,ADC_Channel_15,1,ADC_SampleTime_55Cycles5);
      /* ADC1 Injected conversions trigger is given by software and enabled */
      ADC_ExternalTrigInjectedConvConfig(ADC1, ADC_ExternalTrigInjecConv_None);
      ADC_ExternalTrigInjectedConvCmd(ADC1,ENABLE);
}


void timer4_init (void)
{
      TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
      NVIC_InitTypeDef NVIC_InitStructure;
      /* Enable the TIM4 Interrupt */
      NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
      NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
      NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
      NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
      NVIC_Init(&NVIC_InitStructure);
      /* Time base configuration */
      TIM_TimeBaseStructure.TIM_Period = (71999 / 2);		// 1KHz
      TIM_TimeBaseStructure.TIM_Prescaler = 1;
      TIM_TimeBaseStructure.TIM_ClockDivision = 0;
      TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
      TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x0000;
      TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
      TIM_Cmd(TIM4, ENABLE);
      TIM_ClearFlag(TIM4,TIM_FLAG_Update);
      TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);
}

/*
void sw_read (void)
{
      if (GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_0) == RESET) {	// Select
         sw_cnt1++;
         if (sw_cnt1 >= 10) {
            sw_cnt1 = 0;
            if ((sw_value & 0x01) == 0)	{
               sw_value |= 0x01;
               if (play_mp3) play_mp3 = 0;else play_mp3 = 1;
            }
         }
      } else {
      	 sw_cnt1 = 0;
      	 sw_value &= ~(0x01);
      }
      if (GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_1) == RESET) {	// Down
         sw_cnt2++;
         if (sw_cnt2 >= 10) {
            sw_cnt2 = 0;
            if ((sw_value & 0x02) == 0)	{
               sw_value |= 0x02;
               play_prev();
            }
         }
      } else {
      	 sw_cnt2 = 0;
      	 sw_value &= ~(0x02);
      }
      if (GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_2) == RESET) {	// Left
         sw_cnt3++;
         if (sw_cnt3 >= 10) {
            sw_cnt3 = 0;
            if ((sw_value & 0x04) == 0)	{
               sw_value |= 0x04;
               mp3_seek(0);
            }
         }
      } else {
         sw_cnt3 = 0;
      	 sw_value &= ~(0x04);
      }
      if (GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_3) == RESET) {	// Right
         sw_cnt4++;
         if (sw_cnt4 >= 10) {
            sw_cnt4 = 0;
            if ((sw_value & 0x08) == 0)	{
               sw_value |= 0x08;
               mp3_seek(1);
            }
         }
      } else {
         sw_cnt4 = 0;
      	 sw_value &= ~(0x08);
      }
      if (GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_4) == RESET) {	// Up
         sw_cnt5++;
         if (sw_cnt5 >= 10) {
            sw_cnt5 = 0;
            if ((sw_value & 0x10) == 0)	{
               sw_value |= 0x10;
               play_next();
            }
         }
      } else {
         sw_cnt5 = 0;
      	 sw_value &= ~(0x10);
      }
}

*/

void main_display(){
     FLASH_Lock();
      //timer4_init();
      serial_init();
      lcd_init();
	  touch_init();
      adc_init();
	  mp3_init();
	  //init_rtc();
	  draw_apple_logo(350,570);
	  lcd_draw_rectangle(10,0,1000,150);
	  
	  lcd_printf(2,14,"<");
	  lcd_printf(26,14,">");
	  if (play_mp3) lcd_printf(14,14,"II");
	  else lcd_printf(14,14,"I>");
	  lcd_printf(14,10,">>");
	  //lcd_printf(13,18,"<<");
	  lcd_printf(26,10,"+");
	  lcd_printf(2,10,"-");
	  lcd_printf(1,18,"list");
	  lcd_printf(25,18,"inf");

}

void touch_read (unsigned short *x,unsigned short *y){

	
	if ((*x>=600&&*x<=1050)&&(*y>=600&&*y<=800)) {	// Select
        sw_cnt1++;
        if (sw_cnt1 >= 40) {
            sw_cnt1 = 0;
            if ((sw_value & 0x01) == 0)	{
               sw_value |= 0x01;
               if (play_mp3) {
			   play_mp3 = 0;
			   lcd_printf(14,14,"I>");
			   }
			   
			   else {
			   play_mp3 = 1;
			   lcd_printf(14,14,"II");
			   }
			   
            }
        }
    } 
	 else {
      	sw_cnt1 = 0;
      	sw_value &= ~(0x01);
    }
    if ((*x>=1300&&*x<=1800)&&(*y>=600&&*y<=800)) {	// Right
         sw_cnt2++;
         if (sw_cnt2 >= 40) {
            sw_cnt2 = 0;
            if ((sw_value & 0x02) == 0)	{
               sw_value |= 0x02;
               play_prev();
            }
         }
    } 
	else {
      	sw_cnt2 = 0;
      	sw_value &= ~(0x02);
    }
	
    if ((*x==3000)&&(*y==3000)) {	// -2���
        sw_cnt3++;
        if (sw_cnt3 >= 40) {
            sw_cnt3 = 0;
            if ((sw_value & 0x04) == 0)	{
               sw_value |= 0x04;
               mp3_seek(0);
            }
        }
    } 
	else{
        sw_cnt3 = 0;
      	sw_value &= ~(0x04);
    }
	
    if ((*x>=600&&*x<=1000)&&(*y>=900&&*y<=1150)) {	// x2���
        sw_cnt4++;
        if (sw_cnt4 >= 40) {
            sw_cnt4 = 0;
            if ((sw_value & 0x08) == 0)	{
               sw_value |= 0x08;
               mp3_seek(1);
            }
        }
    } 
	else {
        sw_cnt4 = 0;
      	sw_value &= ~(0x08);
    }
    if ((*x>=100&&*x<=400)&&(*y>=600&&*y<=800)) {	// Left
        sw_cnt5++;
        if (sw_cnt5 >= 40) {
            sw_cnt5 = 0;
            if ((sw_value & 0x10) == 0)	{
               sw_value |= 0x10;
               play_next();
            }
        }
	} 
	else {
        sw_cnt5 = 0;
      	sw_value &= ~(0x10);
    }	
	if ((*x>=1300&&*x<=1800)&&(*y>=900&&*y<=1150)) {	// volume up
		volume--;
		if(volume==0)volume++;
	} 
	
	
	if ((*x>=100&&*x<=400)&&(*y>=900&&*y<=1150)) {	//volume down
		volume++;
		if(volume>127)volume=127;
	} 
	if ((*x>=1300&&*x<=1800)&&(*y>=200&&*y<=400)) {	// information
		unsigned short x2,y2;
		display_id3v1_tag();
		lcd_printf(1,18,"                         <--");
		while(1){
			touch_process(&x2,&y2);
			if(go_back(x2,y2)){
				xLoc=1,yLoc=1;
				break;
			}
		}
	} 
	if ((*x>=100&&*x<=400)&&(*y>=200&&*y<=400)) {	// playlist
		unsigned short x2,y2;
		display_id3v1_tag();
		show_playlist();
		lcd_printf(1,18,"                         <--");
		while(1){
			touch_process(&x2,&y2);
			if(go_back(x2,y2)){
				xLoc=1,yLoc=1;
				break;
			}
		}
	} 
	
	xLoc=0;
	yLoc=0;
			   		   
}

int select_music(unsigned short x,unsigned short y){
	if ((x>=1300&&x<=1800)&&(y>=200&&y<=400)) {	// go back
       main_display();
	   mp3_displayinit();
	   return 1;
	}
	return 0;
}
int go_back(unsigned short x,unsigned short y){
	if ((x>=1300&&x<=1800)&&(y>=200&&y<=400)) {	// go back
       main_display();
	   mp3_displayinit();
	   return 1;
	}
	return 0;
}

int main (void)
{
      int count,sec,msec,sum_adc,adc_count;
      //unsigned short ad_value;
      /* System Clocks Configuration */
      Periph_Configuration();
      /* NVIC configuration */
      NVIC_Configuration();
      /* Configure the GPIO ports */
      GPIO_Configuration();
      /* Setup SysTick Timer for 1 millisecond interrupts, also enables Systick and Systick-Interrupt */
      if (SysTick_Config(SystemCoreClock / 1000)) {
         /* Capture error */
         while (1);
      }
      /* 4 bit for pre-emption priority, 0 bits for subpriority */
      NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
      FLASH_Unlock();
      option_value = FLASH_GetReadOutProtectionStatus();
      if (option_value == 0) {
         FLASHStatus = FLASH_EraseOptionBytes();
         FLASHStatus = FLASH_ReadOutProtection(ENABLE);
         NVIC_SystemReset();
      }
	  
	main_display();
	
      count = 0;
      sec = 0;
      msec = 0;
      old_volume = -1;
      sum_adc = 0;
      adc_count = 0;
      //ad_value = 0;
	  
      while (1) {
      	    mp3_play();
            if (tick) {
               tick = 0;
               count++;
               msec++;
               touch_process(&xLoc,&yLoc);
			   touch_read(&xLoc,&yLoc);
			   
               //sw_read();
               if (count == 100) {
               	  //led_control(LED3,ON);
               } else {
               	  if (count >= 200) {
               	     count = 0;
               	     //led_control(LED3,OFF);
               	  }
               }
               if (msec >= 500) {
               	  msec = 0;
		  sec++;
               	  //s_printf(USART1," Usart 1 %d \r\n",sec);
               	  //s_printf(USART2," Serial Port 2 \r\n");
               	  second++;
               }
               if ((rxcnt1) && (rxck1 >= 3)) {
               	  rxck1 = 0;
               	  receive_serial1();
               }
               if ((rxcnt2) && (rxck2 >= 3)) {
               	  rxck1 = 0;
               	  receive_serial2();
               }
               if (txled) led_control(LED1,ON);
               if (rxled) led_control(LED2,ON);
               if (txled == 0) led_control(LED1,OFF);
               if (rxled == 0) led_control(LED2,OFF);
               if (ADC_GetFlagStatus(ADC1,ADC_FLAG_JEOC)) {
                  sum_adc += (ADC_GetInjectedConversionValue(ADC1,ADC_InjectedChannel_1) & 0x0FFF);
                  adc_count++;
               }
               if (adc_count >= 50) {
			   /*  
               	  ad_value = sum_adc / adc_count;      	  
               	  adc_count = 0;
               	  sum_adc = 0;
	     	  ad_value = (ad_value * 330) / 4096;
		  volume = (ad_value * 255) / 330;
		  volume = abs(volume - 255) / 2;
		  */
		  if (volume != old_volume) {
		     old_volume = volume;
		     volume_flag = 1;
					
			 
                     lcd_printf(2,1,"Volume %d ",255 - (volume + 127));
                  }
	       }
               /* Clear the ADC1 JEOC pending flag */
               ADC_ClearFlag(ADC1, ADC_FLAG_JEOC);
               ADC_SoftwareStartInjectedConvCmd(ADC1,ENABLE);
            }
      }
}
