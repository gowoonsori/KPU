/********************************************************************************/
/* ads7843.c                                                                    */
/* STM32F103VET6                                                                */
/* (Lee ChangWoo HL2IRW  hl2irw@kpu.ac.kr 011-726-6860)                 	*/
/* stm32f103ve_mp3								*/
/********************************************************************************/
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "hwdefs.h"


#define KEY_DOWN		0x01
#define KEY_UP			0x00


typedef struct
{
	unsigned short x0;
	unsigned short y0;
	unsigned short x;
	unsigned short y;
	unsigned short xfac;
	unsigned short yfac;
	short xoff;
	short yoff;
} pen_holder;


#define PEN  			PC_INPUT(4)	//PC4 INT
#define DOUT 			PA_INPUT(6)	//PA6  MISO
#define TDIN 			PA_OUTPUT(7)	//PA7  MOSI
#define TCLK 			PA_OUTPUT(5)	//PA5  SCLK
#define TCS  			PC_OUTPUT(6)	//PC6  CS
#define USER			PA_INPUT(8)	//PA8  USER SW


#define CMD_RDY			0x90
#define CMD_RDX			0xD0
#define READ_TIMES		15
#define LOST_VAL		5
#define ERR_RANGE		50

pen_holder pen_point;
volatile unsigned char pen_status,pen_enable,pen_count;
extern unsigned short foreground_color,background_color;
extern volatile unsigned short tick,jiffes;



void ads7843_write_byte (unsigned char num)
{
      unsigned char count = 0;
      for (count=0;count<8;count++) {
	  if (num & 0x80) TDIN = 1;else TDIN = 0;
	  num <<= 1;
	  TCLK = 0;
	  TCLK = 1;
      }
      TDIN = 1;
}


unsigned short ads7843_read_adc (unsigned char cmd)
{
      unsigned char i;
      unsigned char count = 0;
      unsigned short num = 0;
      TCLK = 0;
      TCS = 0;
      ads7843_write_byte(cmd);
      for (i=100;i>0;i--);
      TCLK = 1;
      TCLK = 0;
      for (count=0;count<16;count++) {
	  num <<= 1;
	  TCLK = 0;
	  TCLK = 1;
	  if (DOUT) num++;
      }
      num >>= 4;
      TCS = 1;
      return(num);
}


unsigned short ads7843_read_xy (unsigned char xy)
{
      unsigned short i, j;
      unsigned short buf[READ_TIMES];
      unsigned short sum = 0;
      unsigned short temp;
      for (i=0;i<READ_TIMES;i++) {
	  buf[i] = ads7843_read_adc(xy);
      }
      for (i=0;i<READ_TIMES-1; i++) {
	  for (j=i+1;j<READ_TIMES;j++) {
	      if (buf[i] > buf[j]) {
		 temp = buf[i];
		 buf[i] = buf[j];
	 	 buf[j] = temp;
	      }
	  }
      }
      sum = 0;
      for (i=LOST_VAL;i<READ_TIMES-LOST_VAL;i++) sum += buf[i];
      temp = sum / (READ_TIMES - 2 * LOST_VAL);
      return temp;
}


unsigned char read_ads7843 (unsigned short *x,unsigned short *y)
{
      unsigned short xtemp,ytemp;
      xtemp = ads7843_read_xy(CMD_RDX);
      ytemp = ads7843_read_xy(CMD_RDY);
      if ((xtemp < 100) || (ytemp < 100)) return 0;
      *x = xtemp;
      *y = ytemp;
      return 1;
}


unsigned char read_ads7843_2(unsigned short *x,unsigned short *y)
{
      unsigned short x1,y1;
      unsigned short x2,y2;
      unsigned char flag;
      flag = read_ads7843(&x1,&y1);
      if (flag == 0) return(0);
      flag = read_ads7843(&x2,&y2);
      if (flag == 0) return(0);
      if ((((x2 <= x1) && (x1 < x2 + ERR_RANGE)) || ((x1 <= x2) && (x2 < x1 + ERR_RANGE))) && (((y2 <= y1) && (y1 < y2 + ERR_RANGE)) || ((y1 <= y2) && (y2 < y1 + ERR_RANGE)))) {
         *x = (x1 + x2) / 2;
         *y = (y1 + y2) / 2;
         return 1;
      } else return 0;
}


void pen_interrupt_set (unsigned char enable)
{
      if (enable) EXTI->IMR |= (1 << 4);else EXTI->IMR &= ~(1 << 4);
}


unsigned char read_tp_once (void)
{
      int idx = 0,flag;
      pen_status = KEY_UP;
      pen_interrupt_set(0);
      flag = 1;
      wait_ms(5);
      read_ads7843_2(&pen_point.x,&pen_point.y);
      while (flag) {
      	    if ((PEN) || (idx >= 500)) {
      	       flag = 0;
      	    }
      	    wait_ms(2);
      	    idx++;
      }
      EXTI_ClearFlag(EXTI_Line4);
      pen_interrupt_set(1);
      if (idx >= 500) return 0;else return 1;
}


void drow_touch_point (unsigned char x,unsigned short y)
{
      lcd_draw_line(x - 12,y,x + 13,y);
      lcd_draw_line(x,y - 12,x,y + 13);
      lcd_set_pixel(x + 1,y + 1);
      lcd_set_pixel(x - 1,y + 1);
      lcd_set_pixel(x + 1,y - 1);
      lcd_set_pixel(x - 1,y - 1);
      lcd_draw_circle(x,y,6);
}


void touch_adjust (void)
{
      unsigned short pos_temp[4][2];
      unsigned char cnt;
      unsigned short d1,d2;
      unsigned int tem1,tem2;
      unsigned short fac;
      int temp;
      cnt = 0;
      //set_color(color);
      //set_background_color(bcolor);
      drow_touch_point(20,20);
      pen_status = KEY_UP;
      pen_point.xfac = 0;
      pen_enable = 1;
      while (1) {
      	    if (tick) {
      	       tick = 0;
               if (pen_enable == 0) {
      	          pen_count++;
      	          if (pen_count >= 10) {
      	             pen_count = 0;
      	             pen_enable = 1;
      	          }
               }
            }
	    if (pen_status == KEY_DOWN) {
               if (PEN) {
      	          pen_status = KEY_UP;
                  EXTI_ClearFlag(EXTI_Line4);
                  pen_interrupt_set(1);
               }
	       led_control(LED3,OFF);
	       if (read_tp_once()) {
		  pos_temp[cnt][0] = pen_point.x;
		  pos_temp[cnt][1] = pen_point.y;
		  cnt++;
		  if (cnt >= 5) cnt = 0;
	       }
	       switch (cnt) {
	       	  case 1:
		    lcd_clear(background_color);
		    drow_touch_point(220,20);
		    break;
		  case 2:
		    lcd_clear(background_color);
		    drow_touch_point(20,300);
		    break;
	  	  case 3:
		    lcd_clear(background_color);
		    drow_touch_point(220,300);
		    break;
	 	  case 4:
		    tem1 = abs(pos_temp[0][0] - pos_temp[1][0]);
		    tem2 = abs(pos_temp[0][1] - pos_temp[1][1]);
		    tem1 *= tem1;
		    tem2 *= tem2;
		    d1 = sqrt(tem1 + tem2);
		    tem1 = abs(pos_temp[2][0] - pos_temp[3][0]);
		    tem2 = abs(pos_temp[2][1] - pos_temp[3][1]);
		    tem1 *= tem1;
		    tem2 *= tem2;
		    d2 = sqrt(tem1 + tem2);
		    fac = (d1 * 10000) / d2;
		    if ((fac < 9500) || (fac > 10500) || (d1 == 0) || (d2 == 0)) {
		       cnt = 0;
		       lcd_clear(background_color);
		       drow_touch_point(20,20);
		       lcd_printf(1,2,"ER1 fac %4d",fac);
		       lcd_printf(1,3,"1 X: %4d Y:%4d",pos_temp[0][0],pos_temp[0][1]);
		       lcd_printf(1,4,"2 X: %4d Y:%4d",pos_temp[1][0],pos_temp[1][1]);
		       lcd_printf(1,5,"3 X: %4d Y:%4d",pos_temp[2][0],pos_temp[2][1]);
		       lcd_printf(1,6,"4 X: %4d Y:%4d",pos_temp[3][0],pos_temp[3][1]);
		       lcd_printf(1,8,"D1: %4d D2: %4d",d1,d2);
		       continue;
		    }
		    tem1 = abs(pos_temp[0][0] - pos_temp[2][0]);
		    tem2 = abs(pos_temp[0][1] - pos_temp[2][1]);
		    tem1 *= tem1;
		    tem2 *= tem2;
		    d1 = sqrt(tem1 + tem2);
		    tem1 = abs(pos_temp[1][0] - pos_temp[3][0]);
		    tem2 = abs(pos_temp[1][1] - pos_temp[3][1]);
		    tem1 *= tem1;
		    tem2 *= tem2;
		    d2 = sqrt(tem1 + tem2);
		    fac = (d1 * 10000) / d2;
		    if ((fac < 9500) || (fac > 10500)) {
		       cnt = 0;
		       lcd_clear(background_color);
		       drow_touch_point(20,20);
		       lcd_printf(1,2,"ER2 fac %4d",fac);
		       lcd_printf(1,3,"1 X: %4d Y:%4d",pos_temp[0][0],pos_temp[0][1]);
		       lcd_printf(1,4,"2 X: %4d Y:%4d",pos_temp[1][0],pos_temp[1][1]);
		       lcd_printf(1,5,"3 X: %4d Y:%4d",pos_temp[2][0],pos_temp[2][1]);
		       lcd_printf(1,6,"4 X: %4d Y:%4d",pos_temp[3][0],pos_temp[3][1]);
		       lcd_printf(1,8,"D1: %4d D2: %4d",d1,d2);
		       continue;
		    }
		    tem1 = abs(pos_temp[1][0] - pos_temp[2][0]);
		    tem2 = abs(pos_temp[1][1] - pos_temp[2][1]);
		    tem1 *= tem1;
		    tem2 *= tem2;
		    d1 = sqrt(tem1 + tem2);
		    tem1 = abs(pos_temp[0][0] - pos_temp[3][0]);
		    tem2 = abs(pos_temp[0][1] - pos_temp[3][1]);
		    tem1 *= tem1;
		    tem2 *= tem2;
		    d2 = sqrt(tem1 + tem2);
		    fac = (d1 * 10000) / d2;
		    if ((fac < 9500) || (fac > 10500)) {
		       cnt = 0;
		       lcd_clear(background_color);
		       drow_touch_point(20,20);
		       lcd_printf(1,2,"ER3 fac %4d",fac);
		       lcd_printf(1,3,"1 X: %4d Y:%4d",pos_temp[0][0],pos_temp[0][1]);
		       lcd_printf(1,4,"2 X: %4d Y:%4d",pos_temp[1][0],pos_temp[1][1]);
		       lcd_printf(1,5,"3 X: %4d Y:%4d",pos_temp[2][0],pos_temp[2][1]);
		       lcd_printf(1,6,"4 X: %4d Y:%4d",pos_temp[3][0],pos_temp[3][1]);
		       lcd_printf(1,8,"D1: %4d D2: %4d",d1,d2);
		       continue;
		    }
		    pen_point.xfac = (200 * 10000) / (pos_temp[1][0] - pos_temp[0][0]);
		    tem1 = (unsigned int)(240 * 10000);
		    tem2 = (unsigned int)(pen_point.xfac * (pos_temp[1][0] + pos_temp[0][0]));
		    temp = tem1 - tem2;
		    temp = temp / 20000;
		    pen_point.xoff = (short)temp;
		    pen_point.yfac = (280 * 10000) / (pos_temp[0][1] - pos_temp[2][1]);
		    tem1 = (unsigned int)(320 * 10000);
		    tem2 = (unsigned int)(pen_point.yfac * (pos_temp[2][1] + pos_temp[0][1]));
		    temp = tem1 - tem2;
		    temp = temp / 20000;
		    pen_point.yoff = (short)temp;
                    lcd_clear(background_color);
		    lcd_printf(5,2,"Adjust OK!");
                    lcd_printf(5,3,"Xf: %4d Xoff:%4d",pen_point.xfac,pen_point.xoff);
		    lcd_printf(5,4,"Yf: %4d Yoff:%4d",pen_point.yfac,pen_point.yoff);
		    lcd_printf(5,5,"1 X: %4d Y:%4d",pos_temp[0][0],pos_temp[0][1]);
		    lcd_printf(5,6,"2 X: %4d Y:%4d",pos_temp[1][0],pos_temp[1][1]);
		    lcd_printf(5,7,"3 X: %4d Y:%4d",pos_temp[2][0],pos_temp[2][1]);
		    lcd_printf(5,8,"4 X: %4d Y:%4d",pos_temp[3][0],pos_temp[3][1]);
		    wait_ms(500);
		    lcd_printf(5,9,"Press toutch");
		    return;
	       }
	    }
      }
      cnt = 1;
      while (cnt) {
      	    if (tick) {
      	       tick = 0;
               if (pen_enable == 0) {
      	          pen_count++;
      	          if (pen_count >= 10) {
      	             pen_count = 0;
      	             pen_enable = 1;
      	          }
               }
            }
	    if (pen_status == KEY_DOWN) {
               if (PEN) {
      	          pen_status = KEY_UP;
                  EXTI_ClearFlag(EXTI_Line4);
                  pen_interrupt_set(1);
		  lcd_clear(background_color);
                  cnt = 0;
               }
	    }
      }
}


void touch_init (void)
{
      NVIC_InitTypeDef NVIC_InitStructure;
      EXTI_InitTypeDef EXTI_InitStructure;
      //TCLK, TDIN
      GPIO_Init_Pin(GPIOA,GPIO_Pin_5,GPIO_Speed_50MHz,GPIO_Mode_Out_PP);
      GPIO_Init_Pin(GPIOA,GPIO_Pin_7,GPIO_Speed_50MHz,GPIO_Mode_Out_PP);
      //DOUT
      GPIO_Init_Pin(GPIOA,GPIO_Pin_6,GPIO_Speed_50MHz,GPIO_Mode_IPU);
      //TCS
      GPIO_Init_Pin(GPIOC,GPIO_Pin_6,GPIO_Speed_50MHz,GPIO_Mode_Out_PP);
      //PEN
      GPIO_Init_Pin(GPIOC,GPIO_Pin_4,GPIO_Speed_50MHz,GPIO_Mode_IPU);
      //USER
      GPIO_Init_Pin(GPIOA,GPIO_Pin_8,GPIO_Speed_50MHz,GPIO_Mode_IPU);
      read_ads7843(&pen_point.x,&pen_point.y);
      /* Connect PEN EXTI Line to Key Button GPIO Pin */
      GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource4);
      /* Configure PEN EXTI Line to generate an interrupt on falling edge */
      EXTI_InitStructure.EXTI_Line = EXTI_Line4;
      EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
      EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
      EXTI_InitStructure.EXTI_LineCmd = ENABLE;
      EXTI_Init(&EXTI_InitStructure);
      /* Generate software interrupt: simulate a falling edge applied on PEN EXTI line */
      //EXTI_GenerateSWInterrupt(EXTI_Line4);
      /* Enable the EXTI Line4 Interrupt */
      NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
      NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
      NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
      NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
      NVIC_Init(&NVIC_InitStructure);
      //touch_adjust();
}


void EXTI4_IRQHandler (void)
{
      if (EXTI_GetITStatus(EXTI_Line4) != RESET) {
    	 EXTI_ClearITPendingBit(EXTI_Line4);
    	 if (pen_enable) {
    	    led_control(LED3,ON);
	    pen_status = KEY_DOWN;
	    pen_enable = 0;
	 }
      }
}


void draw_point (unsigned short x,unsigned short y)
{
	lcd_set_pixel(x,y);
	lcd_set_pixel(x + 1,y);
	lcd_set_pixel(x,y + 1);
	lcd_set_pixel(x + 1,y + 1);
}


void touch_process (unsigned short *x,unsigned short *y)
{
      if (pen_status == KEY_DOWN) {
	 led_control(LED3,OFF);
	 pen_interrupt_set(0);
	 if (read_ads7843_2(&pen_point.x,&pen_point.y)) {
	    pen_point.x0 = pen_point.x;
	    pen_point.y0 = pen_point.y;
		*x=pen_point.x0;
		*y=pen_point.y0;
	    //lcd_printf(5,3,"X:%5d Y%5d ",pen_point.x0,pen_point.y0);
	    //draw_point(pen_point.x0,pen_point.y0);
         }
         if (PEN) {
      	    pen_status = KEY_UP;
            EXTI_ClearFlag(EXTI_Line4);
            pen_interrupt_set(1);
         }
      }
      if (pen_enable == 0) {
      	 pen_count++;
      	 if (pen_count >= 2) {
      	    pen_count = 0;
      	    pen_enable = 1;
      	 }
      }
      if (USER == 0) {
      	 lcd_clear(background_color);
      }
}