/********************************************************************************/
/* lcd.c                                                                        */
/* STM32F103VET6                                                                */
/* (Lee ChangWoo HL2IRW  hl2irw@kpu.ac.kr 011-726-6860)                 	*/
/* stm32f103ve_mp3								*/
/********************************************************************************/
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "hwdefs.h"
#include "color.h"
#include "font.h"


//#define  ILI9320    0  /* 0x9320 */
//#define  ILI9325    1  /* 0x9325 */
//#define  ILI9328    2  /* 0x9328 */
//#define  ILI9331    3  /* 0x9331 */
//#define  SSD1298    4  /* 0x8999 */
//#define  SSD1289    5  /* 0x8989 */
//#define  ST7781     6  /* 0x7783 */
//#define  LGDP4531   7  /* 0x4531 */
//#define  SPFD5408B  8  /* 0x5408 */
//#define  R61505U    9  /* 0x1505 0x0505 */
//#define  HX8347D    10 /* 0x0047 */
//#define  HX8347A    11 /* 0x0047 */
//#define  LGDP4535   12 /* 0x4535 */
//#define  SSD2119    13 /* 3.5 LCD 0x9919 */


#define LCD_DATA_PORT		GPIOE
#define LCD_CTL_PORT		GPIOD

#define LCD_DATA0		GPIO_Pin_0
#define LCD_DATA1		GPIO_Pin_1
#define LCD_DATA2		GPIO_Pin_2
#define LCD_DATA3		GPIO_Pin_3
#define LCD_DATA4		GPIO_Pin_4
#define LCD_DATA5		GPIO_Pin_5
#define LCD_DATA6		GPIO_Pin_6
#define LCD_DATA7		GPIO_Pin_7
#define LCD_DATA8		GPIO_Pin_8
#define LCD_DATA9		GPIO_Pin_9
#define LCD_DATA10		GPIO_Pin_10
#define LCD_DATA11		GPIO_Pin_11
#define LCD_DATA12		GPIO_Pin_12
#define LCD_DATA13		GPIO_Pin_13
#define LCD_DATA14		GPIO_Pin_14
#define LCD_DATA15		GPIO_Pin_15

#define LCD_CS			GPIO_Pin_12
#define LCD_RS			GPIO_Pin_13
#define LCD_WR			GPIO_Pin_14
#define LCD_RD			GPIO_Pin_15

#define R0			0x00
#define R1			0x01
#define R2			0x02
#define R3			0x03
#define R4			0x04
#define R5			0x05
#define R6			0x06
#define R7			0x07
#define R8			0x08
#define R9			0x09
#define R10			0x0A
#define R12			0x0C
#define R13			0x0D
#define R14			0x0E
#define R15			0x0F
#define R16			0x10
#define R17			0x11
#define R18			0x12
#define R19			0x13
#define R20			0x14
#define R21			0x15
#define R22			0x16
#define R23			0x17
#define R24			0x18
#define R25			0x19
#define R26			0x1A
#define R27			0x1B
#define R28			0x1C
#define R29			0x1D
#define R30			0x1E
#define R31			0x1F
#define R32			0x20
#define R33			0x21
#define R34			0x22
#define R36			0x24
#define R37			0x25
#define R40			0x28
#define R41			0x29
#define R43			0x2B
#define R45			0x2D
#define R48			0x30
#define R49			0x31
#define R50			0x32
#define R51			0x33
#define R52			0x34
#define R53			0x35
#define R54			0x36
#define R55			0x37
#define R56			0x38
#define R57			0x39
#define R59			0x3B
#define R60			0x3C
#define R61			0x3D
#define R62			0x3E
#define R63			0x3F
#define R64			0x40
#define R65			0x41
#define R66			0x42
#define R67			0x43
#define R68			0x44
#define R69			0x45
#define R70			0x46
#define R71			0x47
#define R72			0x48
#define R73			0x49
#define R74			0x4A
#define R75			0x4B
#define R76			0x4C
#define R77			0x4D
#define R78			0x4E
#define R79			0x4F
#define R80			0x50
#define R81			0x51
#define R82			0x52
#define R83			0x53
#define R96			0x60
#define R97			0x61
#define R106			0x6A
#define R118			0x76
#define R128			0x80
#define R129			0x81
#define R130			0x82
#define R131			0x83
#define R132			0x84
#define R133			0x85
#define R134			0x86
#define R135			0x87
#define R136			0x88
#define R137			0x89
#define R139			0x8B
#define R140			0x8C
#define R141			0x8D
#define R143			0x8F
#define R144			0x90
#define R145			0x91
#define R146			0x92
#define R147			0x93
#define R148			0x94
#define R149			0x95
#define R150			0x96
#define R151			0x97
#define R152			0x98
#define R153			0x99
#define R154			0x9A
#define R157			0x9D
#define R192			0xC0
#define R193			0xC1
#define R229			0xE5

unsigned short foreground_color,background_color,device_code,maxx,maxy,ax,ay,max_col,max_row,tx,ty,eng_mode,page;
char buff[256],dumy_data[32];
const unsigned char s_bit[8] = {0x80,0x40,0x20,0x10,8,4,2,1};
volatile unsigned char hangul_mode;


const unsigned char han_tbl [3][32] = {  // 3 * 32
        {0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 1, 2, 3, 4, 5, 0, 0, 6, 7, 8, 9,10,11, 0, 0,12,13,14,15,16,17, 0, 0,18,19,20,21, 0, 0},
        {0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16, 0,17,18,19,20,21,22,23,24,25,26,27, 0, 0}};
const unsigned short loc1[8] = {0,20,40,60,80,100,120,140};
const unsigned short loc2[4] = {160,182,204,226};
const unsigned short loc3[4] = {248,276,304,332};

const unsigned short hangul_chosung_tbl [19] = {
      0x8800, 0x8C00, 0X9000, 0x9400, 0x9800, 0x9C00, 0xA000, 0xA400,
      0xA800, 0xAC00, 0xB000, 0xB400, 0xB800, 0xBC00, 0xC000, 0xC400,
      0xC800, 0xCC00, 0xD000};

const unsigned short hangul_jungsung_tbl [21] = {
      0x8060, 0x8080, 0X80A0, 0x80C0, 0x80E0, 0x8140, 0x8160, 0x8180,
      0x81A0, 0x81C0, 0x81E0, 0x8240, 0x8260, 0x8280, 0x82A0, 0x82C0,
      0x82E0, 0x8340, 0x8360, 0x8380, 0x83A0};

const unsigned short hangul_jongsung_tbl[28] = {
      0x8001, 0x8002, 0x8003, 0x8004, 0x8005, 0x8006, 0x8007, 0x8008,
      0x8009, 0x800A, 0x800B, 0x800C, 0x800D, 0x800E, 0x800F, 0x8010,
      0x8011, 0x8013, 0x8014, 0x8015, 0x8016, 0x8017, 0x8018, 0x8019,
      0x801A, 0x801B, 0x801C, 0x801D};



void lcd_data_port_input (void)
{
      LCD_DATA_PORT->CRL = 0X88888888;
      LCD_DATA_PORT->CRH = 0X88888888;
      LCD_DATA_PORT->ODR = 0XFFFF;
}


void lcd_data_port_output (void)
{
      LCD_DATA_PORT->CRL = 0X33333333;
      LCD_DATA_PORT->CRH = 0X33333333;
      LCD_DATA_PORT->ODR = 0XFFFF;
}


void lcd_data_write (unsigned short value)
{
      LCD_DATA_PORT->ODR = value;
}


unsigned short lcd_data_read (void)
{
      return (LCD_DATA_PORT->IDR);
}


void lcd_cs_hi (void)
{
      GPIO_SetBits(LCD_CTL_PORT,LCD_CS);
}


void lcd_cs_lo (void)
{
      GPIO_ResetBits(LCD_CTL_PORT,LCD_CS);
}


void lcd_rs_hi (void)
{
      GPIO_SetBits(LCD_CTL_PORT,LCD_RS);
}


void lcd_rs_lo (void)
{
      GPIO_ResetBits(LCD_CTL_PORT,LCD_RS);
}


void lcd_wr_hi (void)
{
      GPIO_SetBits(LCD_CTL_PORT,LCD_WR);
}


void lcd_wr_lo (void)
{
      GPIO_ResetBits(LCD_CTL_PORT,LCD_WR);
}


void lcd_rd_hi (void)
{
      GPIO_SetBits(LCD_CTL_PORT,LCD_RD);
}


void lcd_rd_lo (void)
{
      GPIO_ResetBits(LCD_CTL_PORT,LCD_RD);
}


void lcd_wr_data (unsigned short data)
{
      lcd_rs_hi();
      lcd_cs_lo();
      lcd_data_write(data);
      lcd_wr_lo();
      lcd_wr_hi();
      lcd_cs_hi();
}


void lcd_wr_register (unsigned char data)
{
      lcd_rs_lo();
      lcd_cs_lo();
      lcd_data_write(data);
      lcd_wr_lo();
      lcd_wr_hi();
      lcd_cs_hi();
}


void lcd_write_register (unsigned char reg, unsigned short value)
{
      lcd_wr_register(reg);
      lcd_wr_data(value);
}


unsigned short lcd_read_register (unsigned char reg)
{
      unsigned short value;
      lcd_wr_register(reg);
      lcd_data_port_input();
      lcd_rs_hi();
      lcd_cs_lo();
      lcd_rd_lo();
      lcd_rd_hi();
      value = lcd_data_read();
      lcd_cs_hi();
      lcd_data_port_output();
      return value;
}


unsigned short lcd_read_data(void)
{
      unsigned short value;
      lcd_data_port_input();
      lcd_rs_hi();
      lcd_cs_lo();
      lcd_rd_lo();
      lcd_rd_hi();
      value = lcd_data_read();
      lcd_cs_hi();
      lcd_data_port_output();
      return value;
}
void draw_apple_logo(unsigned short x,unsigned short y){
	lcd_draw_line(x+32,y,x+34,y);
	lcd_draw_line(x+30,y+1,x+34,y+1);
	lcd_draw_line(x+28,y+2,x+34,y+2);
	lcd_draw_line(x+27,y+3,x+33,y+3);
	lcd_draw_line(x+26,y+4,x+33,y+4);
	lcd_draw_line(x+26,y+5,x+33,y+5);
	lcd_draw_line(x+25,y+6,x+32,y+6);
	lcd_draw_line(x+25,y+7,x+32,y+7);
	lcd_draw_line(x+24,y+8,x+31,y+8);
	lcd_draw_line(x+24,y+9,x+30,y+9);
	lcd_draw_line(x+24,y+10,x+29,y+10);
	lcd_draw_line(x+24,y+11,x+28,y+11);
	lcd_draw_line(x+24,y+12,x+27,y+12);
	lcd_draw_line(x+13,y+13,x+15,y+13);lcd_draw_line(x+31,y+13,x+37,y+13);
	lcd_draw_line(x+9,y+14,x+19,y+14);lcd_draw_line(x+29,y+14,x+39,y+14);
	lcd_draw_line(x+8,y+15,x+22,y+15);lcd_draw_line(x+26,y+15,x+41,y+15);
	lcd_draw_line(x+6,y+16,x+42,y+16);
	lcd_draw_line(x+5,y+17,x+43,y+17);
	lcd_draw_line(x+5,y+18,x+43,y+18);
	lcd_draw_line(x+4,y+19,x+43,y+19);
	lcd_draw_line(x+3,y+20,x+41,y+20);
	lcd_draw_line(x+3,y+21,x+40,y+21);
	lcd_draw_line(x+2,y+22,x+40,y+22);
	lcd_draw_line(x+2,y+23,x+39,y+23);
	lcd_draw_line(x+2,y+24,x+39,y+24);
	lcd_draw_line(x+1,y+25,x+38,y+25);
	lcd_draw_line(x+1,y+26,x+38,y+26);
	lcd_draw_line(x+1,y+27,x+38,y+27);	
	lcd_draw_line(x+1,y+28,x+38,y+28);
	lcd_draw_line(x+1,y+29,x+38,y+29);
	lcd_draw_line(x+1,y+30,x+38,y+30);
	lcd_draw_line(x+1,y+31,x+38,y+31);
	lcd_draw_line(x+1,y+32,x+38,y+32);
	lcd_draw_line(x+1,y+33,x+39,y+33);
	lcd_draw_line(x+2,y+34,x+39,y+34);
	lcd_draw_line(x+2,y+35,x+40,y+35);
	lcd_draw_line(x+2,y+36,x+40,y+36);
	lcd_draw_line(x+2,y+37,x+41,y+37);
	lcd_draw_line(x+3,y+38,x+42,y+38);
	lcd_draw_line(x+3,y+39,x+44,y+39);
	lcd_draw_line(x+3,y+40,x+45,y+40);
	lcd_draw_line(x+3,y+41,x+45,y+41);
	lcd_draw_line(x+4,y+42,x+45,y+42);
	lcd_draw_line(x+4,y+43,x+44,y+43);
	lcd_draw_line(x+5,y+44,x+44,y+44);
	lcd_draw_line(x+5,y+45,x+43,y+45);
	lcd_draw_line(x+6,y+46,x+43,y+46);
	lcd_draw_line(x+6,y+47,x+42,y+47);
	lcd_draw_line(x+7,y+48,x+42,y+48);
	lcd_draw_line(x+8,y+49,x+41,y+49);
	lcd_draw_line(x+9,y+50,x+40,y+50);
	lcd_draw_line(x+10,y+51,x+39,y+51);
	lcd_draw_line(x+11,y+52,x+38,y+52);
	lcd_draw_line(x+12,y+53,x+19,y+53);lcd_draw_line(x+30,y+53,x+37,y+53);
	lcd_draw_line(x+13,y+54,x+18,y+54);lcd_draw_line(x+32,y+54,x+36,y+54);
}

void lcd_write_ram_prepare (void)
{
      lcd_wr_register(R34);
}


void lcd_write_ram (unsigned short rgb_code)
{
      lcd_wr_data(rgb_code);
}


unsigned short lcd_bgr_to_rgb (unsigned short c)
{
      unsigned short r,g,b,rgb;
      b = (c >> 0) & 0x1f;
      g = (c >> 5) & 0x3f;
      r = (c >> 11) & 0x1f;
      rgb = (b << 11) + (g << 5) + (r << 0);
      return (rgb);
}


void lcd_set_cursor (unsigned char xpos, unsigned short ypos)
{
      if ((device_code == 0x8999) || (device_code == 0x8989) || (device_code == 0x9919)) {
	 lcd_write_register(0x004E,xpos);
	 lcd_write_register(0X004F,ypos);
      } else {
	 lcd_write_register(0x0020,xpos);
	 lcd_write_register(0X0021,ypos);
      }
}


unsigned short lcd_get_pixel (unsigned char x,unsigned short y)
{
      unsigned short value;
      lcd_set_cursor(x,y);
      lcd_wr_register(R34);
      lcd_data_port_input();
      lcd_rs_hi();
      lcd_cs_lo();;
      lcd_rd_lo();;
      lcd_rd_hi();
      //dummy READ
      lcd_rd_lo();;
      lcd_rd_hi();
      value = lcd_data_read();
      lcd_cs_hi();
      lcd_data_port_output();
      return lcd_bgr_to_rgb(value);
}


void lcd_display_on (void)
{
      lcd_write_register(R7,0x0173);
}


void lcd_display_off (void)
{
      lcd_write_register(R7,0x0);
}


void lcd_delay (unsigned short count)
{
      wait_ms(count);
}


void lcd_window_max (unsigned short x,unsigned short y,unsigned short x_end,unsigned short y_end)
{

      if ((device_code == 0x8999) || (device_code == 0x8989)) {
	 lcd_write_register(0x44,x | ((x_end - 1) << 8));
	 lcd_write_register(0x45,y);
	 lcd_write_register(0x46,y_end - 1);
      } else {
	 lcd_write_register(0x50,x);		/* Horizontal GRAM Start Address      */
	 lcd_write_register(0x51,x_end - 1);	/* Horizontal GRAM End   Address (-1) */
	 lcd_write_register(0x52,y);		/* Vertical   GRAM Start Address      */
	 lcd_write_register(0x53,y_end - 1);	/* Vertical   GRAM End   Address (-1) */
      }
}


void lcd_set_pixel (unsigned char x,unsigned short y)
{
      lcd_set_cursor(x,y);
      lcd_wr_register(R34);
      lcd_wr_data(foreground_color);
}


void lcd_clear (unsigned short color)
{
       unsigned int idx = 0;
       lcd_set_cursor(0x00,0x0000);
       lcd_write_ram_prepare();
       for (idx=0;idx<76800;idx++) {
	   lcd_wr_data(color);
       }
}


void lcd_init (void)
{
      maxx = 240;
      maxy = 320;
      ax = 0;
      ay = 0;
      page = 0;
      eng_mode = 0;
      hangul_mode = 0;
      max_col = (maxx / 8);
      max_row = (maxy / 16);
      foreground_color = WHITE;
      background_color = BLACK;
      GPIO_Init_Pin(LCD_DATA_PORT,LCD_DATA0,GPIO_Speed_50MHz,GPIO_Mode_Out_PP);
      GPIO_Init_Pin(LCD_DATA_PORT,LCD_DATA1,GPIO_Speed_50MHz,GPIO_Mode_Out_PP);
      GPIO_Init_Pin(LCD_DATA_PORT,LCD_DATA2,GPIO_Speed_50MHz,GPIO_Mode_Out_PP);
      GPIO_Init_Pin(LCD_DATA_PORT,LCD_DATA3,GPIO_Speed_50MHz,GPIO_Mode_Out_PP);
      GPIO_Init_Pin(LCD_DATA_PORT,LCD_DATA4,GPIO_Speed_50MHz,GPIO_Mode_Out_PP);
      GPIO_Init_Pin(LCD_DATA_PORT,LCD_DATA5,GPIO_Speed_50MHz,GPIO_Mode_Out_PP);
      GPIO_Init_Pin(LCD_DATA_PORT,LCD_DATA6,GPIO_Speed_50MHz,GPIO_Mode_Out_PP);
      GPIO_Init_Pin(LCD_DATA_PORT,LCD_DATA7,GPIO_Speed_50MHz,GPIO_Mode_Out_PP);
      GPIO_Init_Pin(LCD_DATA_PORT,LCD_DATA8,GPIO_Speed_50MHz,GPIO_Mode_Out_PP);
      GPIO_Init_Pin(LCD_DATA_PORT,LCD_DATA9,GPIO_Speed_50MHz,GPIO_Mode_Out_PP);
      GPIO_Init_Pin(LCD_DATA_PORT,LCD_DATA10,GPIO_Speed_50MHz,GPIO_Mode_Out_PP);
      GPIO_Init_Pin(LCD_DATA_PORT,LCD_DATA11,GPIO_Speed_50MHz,GPIO_Mode_Out_PP);
      GPIO_Init_Pin(LCD_DATA_PORT,LCD_DATA12,GPIO_Speed_50MHz,GPIO_Mode_Out_PP);
      GPIO_Init_Pin(LCD_DATA_PORT,LCD_DATA13,GPIO_Speed_50MHz,GPIO_Mode_Out_PP);
      GPIO_Init_Pin(LCD_DATA_PORT,LCD_DATA14,GPIO_Speed_50MHz,GPIO_Mode_Out_PP);
      GPIO_Init_Pin(LCD_DATA_PORT,LCD_DATA15,GPIO_Speed_50MHz,GPIO_Mode_Out_PP);
      GPIO_Init_Pin(LCD_CTL_PORT,LCD_CS,GPIO_Speed_50MHz,GPIO_Mode_Out_PP);
      GPIO_Init_Pin(LCD_CTL_PORT,LCD_RS,GPIO_Speed_50MHz,GPIO_Mode_Out_PP);
      GPIO_Init_Pin(LCD_CTL_PORT,LCD_RD,GPIO_Speed_50MHz,GPIO_Mode_Out_PP);
      GPIO_Init_Pin(LCD_CTL_PORT,LCD_WR,GPIO_Speed_50MHz,GPIO_Mode_Out_PP);
      lcd_delay(50);
      lcd_write_register(0x0000,0x0001);
      lcd_delay(50);
      device_code = lcd_read_register(0x0000);
      //s_printf(" Lcd Init -> DeviceCode %4X \r\n",device_code);
      if ((device_code == 0x9325) || (device_code == 0x9328)) {
  	 lcd_write_register(0x00e7,0x0010);
         lcd_write_register(0x0000,0x0001);
         lcd_write_register(0x0001,0x0100);
         lcd_write_register(0x0002,0x0700);
         lcd_write_register(0x0003,0x1030);
         lcd_write_register(0x0004,0x0000);
         lcd_write_register(0x0008,0x0207);
         lcd_write_register(0x0009,0x0000);
         lcd_write_register(0x000a,0x0000);
         lcd_write_register(0x000c,0x0001);
         lcd_write_register(0x000d,0x0000);
         lcd_write_register(0x000f,0x0000);
         lcd_write_register(0x0010,0x0000);
         lcd_write_register(0x0011,0x0007);
         lcd_write_register(0x0012,0x0000);
         lcd_write_register(0x0013,0x0000);
         lcd_delay(50);
         lcd_write_register(0x0010,0x1590);
         lcd_write_register(0x0011,0x0227);
         lcd_delay(50);
         lcd_write_register(0x0012,0x009c);
         lcd_delay(50);
         lcd_write_register(0x0013,0x1900);
         lcd_write_register(0x0029,0x0023);
         lcd_write_register(0x002b,0x000e);
         lcd_delay(50);
         lcd_write_register(0x0020,0x0000);
         lcd_write_register(0x0021,0x013f);
	 lcd_delay(50);
         lcd_write_register(0x0030,0x0007);
         lcd_write_register(0x0031,0x0707);
         lcd_write_register(0x0032,0x0006);
         lcd_write_register(0x0035,0x0704);
         lcd_write_register(0x0036,0x1f04);
         lcd_write_register(0x0037,0x0004);
         lcd_write_register(0x0038,0x0000);
         lcd_write_register(0x0039,0x0706);
         lcd_write_register(0x003c,0x0701);
         lcd_write_register(0x003d,0x000f);
         lcd_delay(50);
         lcd_write_register(0x0050,0x0000);
         lcd_write_register(0x0051,0x00ef);
         lcd_write_register(0x0052,0x0000);
         lcd_write_register(0x0053,0x013f);
         lcd_write_register(0x0060,0xa700);
         lcd_write_register(0x0061,0x0001);
         lcd_write_register(0x006a,0x0000);
         lcd_write_register(0x0080,0x0000);
         lcd_write_register(0x0081,0x0000);
         lcd_write_register(0x0082,0x0000);
         lcd_write_register(0x0083,0x0000);
         lcd_write_register(0x0084,0x0000);
         lcd_write_register(0x0085,0x0000);
         lcd_write_register(0x0090,0x0010);
         lcd_write_register(0x0092,0x0000);
         lcd_write_register(0x0093,0x0003);
         lcd_write_register(0x0095,0x0110);
         lcd_write_register(0x0097,0x0000);
         lcd_write_register(0x0098,0x0000);
         lcd_write_register(0x0007,0x0133);
         lcd_write_register(0x0020,0x0000);
         lcd_write_register(0x0021,0x013f);
      }	else if ((device_code == 0x9320) || (device_code == 0x9300)) {
	 lcd_write_register(0x00,0x0000);
	 lcd_write_register(0x01,0x0100);
	 lcd_write_register(0x02,0x0700);
	 lcd_write_register(0x03,0x1030);
	 lcd_write_register(0x04,0x0000);
	 lcd_write_register(0x08,0x0202);
	 lcd_write_register(0x09,0x0000);
	 lcd_write_register(0x0a,0x0000);
	 lcd_write_register(0x0c,(1 << 0));
	 lcd_write_register(0x0d,0x0000);
	 lcd_write_register(0x0f,0x0000);
	 lcd_delay(50);
	 lcd_write_register(0x07,0x0101);
	 lcd_delay(50);
	 lcd_write_register(0x10,(1 << 12) | (0 << 8) | (1 << 7) | (1 << 6) | (0 << 4));
	 lcd_write_register(0x11,0x0007);
	 lcd_write_register(0x12,(1 << 8) | (1 << 4) | (0 << 0));
	 lcd_write_register(0x13,0x0b00);
	 lcd_write_register(0x29,0x0000);
	 lcd_write_register(0x2b,(1 << 14) | (1 << 4));
	 lcd_write_register(0x50,0);
	 lcd_write_register(0x51,239);
	 lcd_write_register(0x52,0);
	 lcd_write_register(0x53,319);
	 lcd_write_register(0x60,0x2700);
	 lcd_write_register(0x61,0x0001);
	 lcd_write_register(0x6a,0x0000);
	 lcd_write_register(0x80,0x0000);
	 lcd_write_register(0x81,0x0000);
	 lcd_write_register(0x82,0x0000);
	 lcd_write_register(0x83,0x0000);
	 lcd_write_register(0x84,0x0000);
	 lcd_write_register(0x85,0x0000);
	 lcd_write_register(0x90,(0 << 7) | (16 << 0));
	 lcd_write_register(0x92,0x0000);
	 lcd_write_register(0x93,0x0001);
	 lcd_write_register(0x95,0x0110);
	 lcd_write_register(0x97,(0 << 8));
	 lcd_write_register(0x98,0x0000);
	 lcd_write_register(0x07,0x0173);
      }	else if (device_code == 0x1505)	{
         lcd_write_register(0x0007,0x0000);
         lcd_delay(50);
         lcd_write_register(0x0012,0x011C);
         lcd_write_register(0x00A4,0x0001);
         lcd_write_register(0x0008,0x000F);
         lcd_write_register(0x000A,0x0008);
         lcd_write_register(0x000D,0x0008);
         lcd_write_register(0x0030,0x0707);
         lcd_write_register(0x0031,0x0007);
         lcd_write_register(0x0032,0x0603);
         lcd_write_register(0x0033,0x0700);
         lcd_write_register(0x0034,0x0202);
         lcd_write_register(0x0035,0x0002);
         lcd_write_register(0x0036,0x1F0F);
         lcd_write_register(0x0037,0x0707);
         lcd_write_register(0x0038,0x0000);
         lcd_write_register(0x0039,0x0000);
         lcd_write_register(0x003A,0x0707);
         lcd_write_register(0x003B,0x0000);
         lcd_write_register(0x003C,0x0007);
         lcd_write_register(0x003D,0x0000);
         lcd_delay(50);
         lcd_write_register(0x0007,0x0001);
         lcd_write_register(0x0017,0x0001);
         lcd_delay(50);
         lcd_write_register(0x0010,0x17A0);
         lcd_write_register(0x0011,0x0217);
         lcd_write_register(0x0012,0x011E);
         lcd_write_register(0x0013,0x0F00);
         lcd_write_register(0x002A,0x0000);
         lcd_write_register(0x0029,0x000A);
         lcd_write_register(0x0012,0x013E);
         lcd_write_register(0x0050,0x0000);
         lcd_write_register(0x0051,0x00EF);
         lcd_write_register(0x0052,0x0000);
         lcd_write_register(0x0053,0x013F);
         lcd_write_register(0x0060,0x2700);
         lcd_write_register(0x0061,0x0001);
         lcd_write_register(0x006A,0x0000);
         lcd_write_register(0x0080,0x0000);
         lcd_write_register(0x0081,0x0000);
         lcd_write_register(0x0082,0x0000);
         lcd_write_register(0x0083,0x0000);
         lcd_write_register(0x0084,0x0000);
         lcd_write_register(0x0085,0x0000);
         lcd_write_register(0x0090,0x0013);
         lcd_write_register(0x0092,0x0300);
         lcd_write_register(0x0093,0x0005);
         lcd_write_register(0x0095,0x0000);
         lcd_write_register(0x0097,0x0000);
         lcd_write_register(0x0098,0x0000);
         lcd_write_register(0x0001,0x0100);
         lcd_write_register(0x0002,0x0700);
         lcd_write_register(0x0003,0x1030);
         lcd_write_register(0x0004,0x0000);
         lcd_write_register(0x000C,0x0000);
         lcd_write_register(0x000F,0x0000);
         lcd_write_register(0x0020,0x0000);
         lcd_write_register(0x0021,0x0000);
         lcd_write_register(0x0007,0x0021);
         lcd_delay(200);
         lcd_write_register(0x0007,0x0061);
         lcd_delay(200);
         lcd_write_register(0x0007,0x0173);
         lcd_delay(200);
      }	else if (device_code == 0x8989) {
	 lcd_write_register(0x0000,0x0001);
    	 lcd_write_register(0x0003,0xA8A4);
    	 lcd_write_register(0x000C,0x0000);
    	 lcd_write_register(0x000D,0x080C);
    	 lcd_write_register(0x000E,0x2B00);
    	 lcd_write_register(0x001E,0x00B0);
    	 lcd_write_register(0x0001,0x2B3F);
    	 lcd_write_register(0x0002,0x0600);
    	 lcd_write_register(0x0010,0x0000);
    	 lcd_write_register(0x0011,0x6070);
    	 lcd_write_register(0x0005,0x0000);
    	 lcd_write_register(0x0006,0x0000);
    	 lcd_write_register(0x0016,0xEF1C);
    	 lcd_write_register(0x0017,0x0003);
    	 lcd_write_register(0x0007,0x0233);
    	 lcd_write_register(0x000B,0x0000);
    	 lcd_write_register(0x000F,0x0000);
    	 lcd_write_register(0x0041,0x0000);
    	 lcd_write_register(0x0042,0x0000);
    	 lcd_write_register(0x0048,0x0000);
    	 lcd_write_register(0x0049,0x013F);
    	 lcd_write_register(0x004A,0x0000);
    	 lcd_write_register(0x004B,0x0000);
    	 lcd_write_register(0x0044,0xEF00);
    	 lcd_write_register(0x0045,0x0000);
    	 lcd_write_register(0x0046,0x013F);
    	 lcd_write_register(0x0030,0x0707);
    	 lcd_write_register(0x0031,0x0204);
    	 lcd_write_register(0x0032,0x0204);
    	 lcd_write_register(0x0033,0x0502);
    	 lcd_write_register(0x0034,0x0507);
    	 lcd_write_register(0x0035,0x0204);
    	 lcd_write_register(0x0036,0x0204);
    	 lcd_write_register(0x0037,0x0502);
    	 lcd_write_register(0x003A,0x0302);
    	 lcd_write_register(0x003B,0x0302);
    	 lcd_write_register(0x0023,0x0000);
    	 lcd_write_register(0x0024,0x0000);
    	 lcd_write_register(0x0025,0x8000);
    	 lcd_write_register(0x004f,0);
    	 lcd_write_register(0x004e,0);
      }	else if (device_code == 0x8999) {
	 /************* Start Initial Sequence **********/
	 lcd_write_register(0x00,0x0001);
	 lcd_write_register(0x01,0x3B3F);
	 lcd_write_register(0x02,0x0600);
	 /************* Power control setup ***********/
	 lcd_write_register(0x0C,0x0007);
	 lcd_write_register(0x0D,0x0006);
	 lcd_write_register(0x0E,0x3200);
	 lcd_write_register(0x1E,0x00BB);
	 lcd_write_register(0x03,0x6A64);
	 //************ RAM position control **********/
	 lcd_write_register(0x0F,0x0000);
	 lcd_write_register(0x44,0xEF00);
	 lcd_write_register(0x45,0x0000);
	 lcd_write_register(0x46,0x013F);
	 /************* Adjust the Gamma Curve ********/
	 lcd_write_register(0x30,0x0000);
	 lcd_write_register(0x31,0x0706);
	 lcd_write_register(0x32,0x0206);
	 lcd_write_register(0x33,0x0300);
	 lcd_write_register(0x34,0x0002);
	 lcd_write_register(0x35,0x0000);
	 lcd_write_register(0x36,0x0707);
	 lcd_write_register(0x37,0x0200);
	 lcd_write_register(0x3A,0x0908);
	 lcd_write_register(0x3B,0x0F0D);
	 /************* Special command **************/
	 lcd_write_register(0x28,0x0006);
	 lcd_write_register(0x2F,0x12EB);
	 lcd_write_register(0x26,0x7000);
	 lcd_write_register(0x20,0xB0E3);
	 lcd_write_register(0x27,0x0044);
	 lcd_write_register(0x2E,0x7E45);
	 /************* Turn On display **************/
	 lcd_write_register(0x10,0x0000);
	 lcd_delay(30);
	 lcd_write_register(0x11,0x6870);
	 lcd_write_register(0x07,0x0033);
      }
      lcd_delay(500);
      lcd_clear(BLACK);
}


void lcd_fill (unsigned char xsta,unsigned short ysta,unsigned char xend,unsigned short yend,unsigned short color)
{
      unsigned int n;
      lcd_write_register(R80,xsta);
      lcd_write_register(R81,xend);
      lcd_write_register(R82,ysta);
      lcd_write_register(R83,yend);
      lcd_set_cursor(xsta,ysta);
      lcd_write_ram_prepare();
      n = (unsigned int)(yend - ysta + 1) *(xend - xsta + 1);
      while (n--) {
      	    lcd_wr_data(color);
      }
      lcd_write_register(R80,0x0000);
      lcd_write_register(R81,0x00EF);
      lcd_write_register(R82,0x0000);
      lcd_write_register(R83,0x013F);
}


void lcd_draw_line (unsigned char x1, unsigned short y1, unsigned char x2, unsigned short y2)
{
      unsigned short x, y, t;
      if ((x1 == x2) && (y1 == y2)) lcd_set_pixel(x1, y1);
      else if (abs(y2 - y1) > abs(x2 - x1)) {
	 if (y1 > y2) {
	    t = y1;
  	    y1 = y2;
	    y2 = t;
	    t = x1;
	    x1 = x2;
	    x2 = t;
         }
	 for (y=y1;y<y2;y++) {
	     x = (unsigned int)(y - y1) * (x2 - x1) / (y2 - y1) + x1;
	     lcd_set_pixel(x, y);
	 }
      }	else {
	 if (x1 > x2) {
	    t = y1;
   	    y1 = y2;
     	    y2 = t;
            t = x1;
	    x1 = x2;
	    x2 = t;
	 }
	 for (x=x1;x<=x2;x++) {
	     y =(unsigned int)(x - x1) * (y2 - y1) / (x2 - x1) + y1;
	     lcd_set_pixel(x,y);
	 }
      }
}


void lcd_draw_rectangle (unsigned char x1, unsigned short y1, unsigned char x2, unsigned short y2)
{
      lcd_draw_line(x1,y1,x2,y1);
      lcd_draw_line(x1,y1,x1,y2);
      lcd_draw_line(x1,y2,x2,y2);
      lcd_draw_line(x2,y1,x2,y2);
}


void lcd_draw_circle (unsigned char x0,unsigned short y0,unsigned char r)
{
      int a,b;
      int di;
      a = 0;
      b = r;
      di = 3 - (r << 1);
      while (a <= b) {
	    lcd_set_pixel(x0-b,y0-a);
	    lcd_set_pixel(x0+b,y0-a);
	    lcd_set_pixel(x0-a,y0+b);
	    lcd_set_pixel(x0-b,y0-a);
	    lcd_set_pixel(x0-a,y0-b);
	    lcd_set_pixel(x0+b,y0+a);
	    lcd_set_pixel(x0+a,y0-b);
	    lcd_set_pixel(x0+a,y0+b);
	    lcd_set_pixel(x0-b,y0+a);
	    a++;
	    if (di < 0) {
	       di += 4 * a + 6;
	    } else {
	       di += 10 + 4 * (a - b);
	       b--;
	    }
	    lcd_set_pixel(x0+a,y0+b);
      }
}


void set_color (unsigned short color)
{
      foreground_color = color;
}


void set_background_color (unsigned short color)
{
      background_color = color;
}


void set_pixel (unsigned short x, unsigned short y, unsigned short color)
{
      unsigned short temp = foreground_color;
      foreground_color = color;
      lcd_set_pixel(x,y);
      foreground_color = temp;
}


void out_lcd (char wchar)
{
      int idx;
      for (idx=0;idx<8;idx++) {
          if ((s_bit[idx] & wchar) == 0) set_pixel(tx,ty,background_color); else set_pixel(tx,ty,foreground_color);
          tx++;
      }
}


unsigned short uni_to_kssm2 (unsigned short wchar)
{
      unsigned short cho_sung,jung_sung,jong_sung,unicode;
      unsigned short temp = 0x0000;
      unsigned short unicode_hangul_base = 0xAC00;
      unsigned short unicode_hangul_last = 0xD79F;
      unsigned short cho,jung,jong,result;
      temp = wchar;
      if ((temp < unicode_hangul_base) || (temp > unicode_hangul_last)) return temp;
      unicode = temp - unicode_hangul_base;
      cho_sung = unicode / (21 * 28);
      unicode = unicode % (21 * 28);
      jung_sung = unicode / 28;
      unicode = unicode % 28;
      jong_sung = unicode;
      cho = hangul_chosung_tbl[cho_sung];
      jung = hangul_jungsung_tbl[jung_sung];
      jong = hangul_jongsung_tbl[jong_sung];
      result = (cho | jung | jong);
      return result;
}


unsigned short uni_to_kssm (unsigned short wchar)
{
      unsigned short cho,joong,jong;
      unsigned short result = 0x8000;
      cho = 2 + (wchar - 0xAC00) / (21 * 28);
      joong = (wchar - 0xAC00) % (21 * 28) / 28;
      if (joong < 5) joong += 3;
      else if (joong < 11) joong += 5;
      else if (joong < 17) joong += 7;
      else joong += 9;
      jong  = (wchar - 0xAC00) % 28;
      if (jong < 17) jong++;
      else jong += 2;
      result |= cho << 10;
      result |= joong << 5;
      result |= jong;
      return result;
}



unsigned short ks5601_to_kssm (unsigned short wchar)
{
      unsigned char xh,xl;
      unsigned short i,result;
      xh = ((wchar >> 8) & 0xFF);
      xl = (wchar & 0x00FF);
      if ((xh >= 0xB0) && (xl >= 0xA0)) {
         xh = xh - 0xB0;
         xl = xl - 0xA0;
         i = xl + xh * 96;
         result = KS_CODE[i][0] << 8 | KS_CODE[i][1];
         return result;
      }
      return 0;
}


void put_eng (char cdata)
{
      unsigned short val,pi;
      if (cdata < 0x20) {
         if (cdata == 0x0D) ax = 0;
         if (cdata == 0x0A) {
            ay++;
            if (ay >= max_row) {
               ay -= 1;
               ax = 0;
            }
         }
      } else {
	 val = (unsigned short)cdata;
         ty = ay * 16;
         for (pi=0;pi<16;pi++) {
             tx = ax * 8;
             if (eng_mode == 0) {
                dumy_data[pi] = eng_font[val][pi];
             } else {
                if (page == 0) dumy_data[pi] = eng_font[val*2][pi];else dumy_data[pi] = eng_font[val*2+1][pi];
             }
             out_lcd(dumy_data[pi]);
             ty++;
         }
         ax++;
         if (ax >= max_col) {
            ax = 0;
            ay++;
            if (ay >= max_row) {
               ay -= 1;
            }
         }
      }
}


void put_han (unsigned short c)
{
      unsigned char i,first,mid,last,b1,b2,b3;
      unsigned short cc,hlx1,hlx2,hlx3;
      if (hangul_mode == 0) cc = ks5601_to_kssm(c);else cc = uni_to_kssm(c);
      first = ((cc & 0x7C00) >> 10);
      mid = ((cc & 0x03E0) >> 5);
      last = (cc & 0x001F);
      first = han_tbl[0][first];
      mid = han_tbl[1][mid];
      last = han_tbl[2][last];
      b1 = 0;
      b2 = 0;
      b3 = 0;
      switch (mid) {
      	 case 1:
      	 case 3:
      	 case 10:
           b3 = 0;
           break;
         case 5:
         case 7:
         case 12:
         case 15:
         case 17:
         case 20:
         case 21:
           b3 = 1;
           break;
         case 2:
         case 4:
         case 6:
         case 8:
         case 11:
         case 16:
           b3 = 2;
           break;
         case 9:
         case 13:
         case 14:
         case 18:
         case 19:
           b3 = 3;
           break;
      }
      switch (first) {
         case 1:
         case 16:
           if (last == 0) b2 = 0;else b2 = 2;
           break;
         default:
           if (last == 0) b2 = 1;else b2 = 3;
      }
      switch (mid) {
         case 1:
         case 2:
         case 3:
         case 4:
         case 5:
         case 6:
         case 7:
         case 8:
         case 21:
           if (last == 0) b1 = 0; else b1 = 5;
           break;
         case 9:
         case 13:
         case 19:
           if (last == 0) b1 = 1;else b1 = 6;
           break;
         case 14: case 18:
           if (last == 0) b1 = 2;else b1 = 6;
           break;
         case 10:
         case 11:
         case 12:
         case 20:
           if (last == 0) b1 = 3;else b1 = 7;
           break;
         case 15:
         case 16:
         case 17:
           if (last == 0) b1 = 4;else b1 = 7;
           break;
      }
      if ((mid == 0) && (last == 0)) b1 = 0;
      for (i=0;i<16;i++) {
          if (first != 0) {
             hlx1 = first + loc1[b1];
             dumy_data[i*2] = kor_font[hlx1][i*2];
             dumy_data[i*2+1] = kor_font[hlx1][i*2+1];
          }
          if (mid != 0) {
             hlx2 = mid + loc2[b2];
             dumy_data[i*2] = dumy_data[i*2] | kor_font[hlx2][i*2];
             dumy_data[i*2+1] = dumy_data[i*2+1] | kor_font[hlx2][i*2+1];
          }
          if (last != 0) {
             hlx3 = last + loc3[b3];
             dumy_data[i*2] = dumy_data[i*2] | kor_font[hlx3][i*2];
             dumy_data[i*2+1] = dumy_data[i*2+1] | kor_font[hlx3][i*2+1];
          }
      }
      ty = ay * 16;
      for (i=0;i<16;i++) {
          tx = ax * 8;
          out_lcd(dumy_data[i*2]);
          ty++;
      }
      ty = ay * 16;
      for (i=0;i<16;i++) {
          tx = (ax+1) * 8;
          out_lcd(dumy_data[i*2+1]);
          ty++;
      }
      ax += 2;
      if (ax >= max_col) {
         ax = 0;
         ay++;
         if (ay >= max_row) {
            ay -= 1;
         }
      }
}


void put_engxy (unsigned short x,unsigned short y,char pdata)
{
      ax = x;
      ay = y;
      put_eng(pdata);
}


void put_hanxy (unsigned short x,unsigned short y,unsigned short pdata)
{
      ax = x;
      ay = y;
      if (pdata >= 0xAC00) {
         if (pdata >= 0xB0A1) {
            put_han(ks5601_to_kssm(pdata));
         } else {
            put_han(uni_to_kssm(pdata));
         }
      } else {
         put_han(pdata);
      }
}


void lput_char (char *str)
{
      unsigned char len,n;
      unsigned short hangul_char;
      len = strlen(str);
      for (n=0;n<len;n++) {
      	  if ((str[n] & 0x80) == 0) {
             put_eng(str[n]);
          } else {
             hangul_char = str[n];
             hangul_char = hangul_char << 8;
             hangul_char = hangul_char | str[n + 1];
             put_han(hangul_char);
             n++;
          }
      }
}


void lput_char_xy (unsigned short x,unsigned short y,char *str)
{
      unsigned char len,n;
      unsigned short hangul_char;
      ax = x;
      ay = y;
      len = strlen(str);
      for (n=0;n<len;n++) {
      	  if ((str[n] & 0x80) == 0) {
             put_eng(str[n]);
          } else {
             hangul_char = str[n];
             hangul_char = hangul_char << 8;
             hangul_char = hangul_char | str[n + 1];
             put_han(hangul_char);
             n++;
          }
      }
}


void lcd_printf (unsigned short x,unsigned short y,char *form,...)
{
      va_list argptr;
      ax = x;
      ay = y;
      va_start(argptr,form);
      vsprintf(buff,form,argptr);
      lput_char(buff);
      va_end(argptr);
}

